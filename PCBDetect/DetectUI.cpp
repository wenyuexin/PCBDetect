#include "DetectUI.h"
#include <time.h>

using pcb::UserConfig;
using pcb::RuntimeParams;


DetectUI::DetectUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	//成员变量初始化
	adminConfig = Q_NULLPTR;
	userConfig = Q_NULLPTR;
	runtimeParams = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;
	serialNumberUI = Q_NULLPTR;
	defectDetecter = Q_NULLPTR;
	detectThread = Q_NULLPTR;
	detectState = DefectDetecter::Default;
}

void DetectUI::init()
{
	//选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//设置检测界面的聚焦策略
	this->setFocusPolicy(Qt::ClickFocus);

	//加载并显示默认的指示灯图标
	IconFolder = QDir::currentPath() + "/Icons";
	QPixmap greyIcon = QPixmap(IconFolder + "/grey.png"); //grey
	defaultIcon = greyIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio);
	ui.label_indicator->setPixmap(defaultIcon); //加载

	//加载其他指示灯图标
	QPixmap redIcon = QPixmap(IconFolder + "/red.png"); //red
	lightOnIcon = redIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio);
	QPixmap greenIcon = QPixmap(IconFolder + "/green.png"); //grey
	lightOffIcon = greenIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio);

	//重置缺陷检测界面
	this->reset();

	//对绘图控件GraphicsView的初始化设置
	this->initGraphicsView();

	//产品序号识别界面
	delete serialNumberUI;
	serialNumberUI = new SerialNumberUI();
	serialNumberUI->setAdminConfig(adminConfig);
	serialNumberUI->setRuntimeParams(runtimeParams);
	serialNumberUI->setCvMatArray(&cvmatSamples);
	serialNumberUI->setQPixmapArray(&qpixmapSamples);
	serialNumberUI->init();
	connect(serialNumberUI, SIGNAL(recognizeFinished_serialNumUI()), this, SLOT(on_recognizeFinished_serialNumUI()));
	connect(serialNumberUI, SIGNAL(switchImage_serialNumUI()), this, SLOT(on_switchImage_serialNumUI()));
	connect(serialNumberUI, SIGNAL(showPreviousUI_serialNumUI()), this, SLOT(do_showPreviousUI_serialNumUI()));

	//缺陷检测器
	delete defectDetecter;
	defectDetecter = new DefectDetecter;
	connect(defectDetecter, SIGNAL(updateDetectState_detecter(int)), this, SLOT(do_updateDetectState_detecter(int)));
	connect(defectDetecter, SIGNAL(detectFinished_detectThread(bool)), this, SLOT(on_detectFinished_detectThread(bool)));

	//检测线程
	delete detectThread;
	detectThread = new DetectThread;
	detectThread->setAdminConfig(adminConfig);
	detectThread->setUserConfig(userConfig);
	detectThread->setRuntimeParams(runtimeParams);
	detectThread->setSampleImages(&cvmatSamples);
	detectThread->setDetectResult(&detectResult);
	detectThread->setDefectDetecter(defectDetecter);
	detectThread->initDefectDetecter();

	//运动控制
	connect(motionControler, SIGNAL(resetControlerFinished_motion(int)), this, SLOT(on_resetControlerFinished_motion(int)));
	connect(motionControler, SIGNAL(moveToInitialPosFinished_motion(int)), this, SLOT(on_moveToInitialPosFinished_motion()));
	connect(motionControler, SIGNAL(moveForwardFinished_motion(int)), this, SLOT(on_moveForwardFinished_motion()));
	
	//相机控制
	connect(cameraControler, SIGNAL(initCamerasFinished_camera(int)), this, SLOT(on_initCamerasFinished_camera(int)));
	connect(cameraControler, SIGNAL(takePhotosFinished_camera(int)), this, SLOT(on_takePhotosFinished_camera(int)));
	
	//转换线程
	imgConvertThread.setCvMats(&cvmatSamples);
	imgConvertThread.setQPixmaps(&qpixmapSamples);
	imgConvertThread.setCurrentRow(&currentRow_show);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);
	connect(&imgConvertThread, SIGNAL(convertFinished_convertThread()), this, SLOT(on_convertFinished_convertThread()));
}

DetectUI::~DetectUI()
{
	qDebug() << "~DetectUI";

	deletePointersInItemArray(itemArray); //删除图元矩阵中的指针
	deletePointersInCvMatArray(cvmatSamples); //删除cvmatSamples中的指针
	deletePointersInQPixmapArray(qpixmapSamples);//删除qpixmapSamples中的指针
	delete detectThread; //检测线程
	detectThread = Q_NULLPTR;
	delete defectDetecter; //缺陷检测器
	defectDetecter = Q_NULLPTR;
	delete serialNumberUI; //产品序号识别界面
	serialNumberUI = Q_NULLPTR;
}


/****************** 界面的初始化与重置 *******************/

//对绘图控件GraphicsView的初始化设置
void DetectUI::initGraphicsView()
{
	initItemGrid(itemGrid);//初始化图元网格
	initPointersInItemArray(itemArray);//初始化itemArray
	initPointersInCvMatArray(cvmatSamples);//初始化cvmatSamples
	initPointersInQPixmapArray(qpixmapSamples);//初始化qpixmapSamples

	//初始化若干用于监视程序运行状态的变量
	currentRow_show = -1; //显示行号
	runtimeParams->currentRow_detect = -1; //检测行号
	eventCounter = 0; //事件计数器

	//视图控件的设置
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //设置聚焦策略
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用水平滚动条
	ui.graphicsView->setScene(&scene); //在视图中添加场景
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //设置垂直滑动条的位置
}

//重置检测子界面
void DetectUI::reset()
{
	serialNumberUI->reset();//重置序号识别界面

	ui.label_status->setText(""); //清空状态栏
	ui.label_indicator->setPixmap(defaultIcon); //切换指示灯
	ui.label_result->setText("-"); //检测结果
	removeItemsFromGraphicsScene(); //移除场景中已经加载的图元
	deletePointersInItemArray(itemArray); //删除图元矩阵中的指针
	deletePointersInCvMatArray(cvmatSamples); //删除cvmatSamples中的指针
	deletePointersInQPixmapArray(qpixmapSamples);//删除qpixmapSamples中的指针
	currentRow_show = -1; //显示行号的复位
	runtimeParams->currentRow_detect = -1; //检测行号的复位
	eventCounter = 0; //事件计数器
	ui.graphicsView->centerOn(0, 0); //垂直滑条复位
	qApp->processEvents();
}

//按键设置
void DetectUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_start->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}


/********* 图元矩阵和样本图像矩阵的初始化和删除等操作 ***********/

//初始化图元网格
void DetectUI::initItemGrid(pcb::ItemGrid &grid)
{
	//基本参数
	int nCamera = runtimeParams->nCamera; //相机个数
	int nPhotographing = runtimeParams->nPhotographing; //拍摄次数
	QString SampleDirPath = userConfig->SampleDirPath; //sample文件夹的路径 

	//计算总间距
	QSizeF totalSpacing; //总间距
	totalSpacing.setWidth(ItemSpacing * (nCamera + 1)); //间距总宽度
	totalSpacing.setHeight(ItemSpacing * (nPhotographing + 1)); //间距总高度

	//计算图元尺寸
	QSizeF viewSize = ui.graphicsView->size(); //视图尺寸
	itemSize.setWidth(1.0 * (viewSize.width() - totalSpacing.width()) / nCamera); //图元宽度
	qreal itemAspectRatio = adminConfig->ImageAspectRatio; //宽高比
	itemSize.setHeight(1.0 * itemSize.width() / itemAspectRatio); //图元高度

	//计算场景尺寸
	sceneSize = totalSpacing;
	sceneSize += QSizeF(itemSize.width()*nCamera, itemSize.height()*nPhotographing);
	scene.setSceneRect(1, 1, sceneSize.width() + 1, sceneSize.height() + 1);

	//生成绘图网点
	QSizeF spacingBlock = QSizeF(ItemSpacing, ItemSpacing);
	gridSize = itemSize + spacingBlock; //每个网格的尺寸

	//判断itemGrid是否执行过初始化
	if (grid.size() > 0) grid.clear();

	//初始化赋值
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //行
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //列
			QPointF pos(ItemSpacing, ItemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		grid.append(posList);
	}
}

//初始化图元矩阵中的指针 - ItemArray
void DetectUI::initPointersInItemArray(pcb::ItemArray &items)
{
	if (items.size() > 0) {
		deletePointersInItemArray(items);//若执行过init函数，则先delete指针
	}
	
	items.resize(runtimeParams->nPhotographing); //设置大小
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //行
		items[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) { //列
			items[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//删除图元矩阵中的指针 - ItemArray
void DetectUI::deletePointersInItemArray(pcb::ItemArray &items)
{
	for (int iPhotographing = 0; iPhotographing < items.size(); iPhotographing++) {
		int vectorSize = items[iPhotographing].size();
		for (int iCamera = 0; iCamera < vectorSize; iCamera++) {
			delete items[iPhotographing][iCamera];
			items[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//初始化样本图像向量中的指针 - CvMatArray
void DetectUI::initPointersInCvMatArray(pcb::CvMatArray &cvmats)
{
	if (cvmats.size() > 0) {
		deletePointersInCvMatArray(cvmats);//若执行过init函数，则先delete指针
	}
	
	cvmats.resize(runtimeParams->nPhotographing);
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //行
		cvmats[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
			cvmats[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//删除样本图像向量中的指针 - CvMatArray
void DetectUI::deletePointersInCvMatArray(pcb::CvMatArray &cvmats)
{
	for (int iPhotographing = 0; iPhotographing < cvmats.size(); iPhotographing++) {
		int vectorSize = cvmats[iPhotographing].size();
		for (int iCamera = 0; iCamera < vectorSize; iCamera++) {
			delete cvmats[iPhotographing][iCamera];
			cvmats[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//初始化样本图像向量中的指针 - QPixmapArray
void DetectUI::initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
{
	if (qpixmaps.size() > 0) {
		deletePointersInQPixmapArray(qpixmaps);//若执行过init函数，则先delete指针
	}
	
	qpixmaps.resize(runtimeParams->nPhotographing);
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //行
		qpixmaps[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
			qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//删除样本图像向量中的指针 - QPixmapArray
void DetectUI::deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
{
	for (int iPhotographing = 0; iPhotographing < qpixmaps.size(); iPhotographing++) {
		int vectorSize = qpixmaps[iPhotographing].size();
		for (int iCamera = 0; iCamera < vectorSize; iCamera++) {
			delete qpixmaps[iPhotographing][iCamera];
			qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//移除场景中已经加载的图元
void DetectUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);
	}
}


/****************** 按键响应 *******************/

//开始检测新的PCB板
void DetectUI::on_pushButton_start_clicked()
{
	ui.label_status->setText(pcb::chinese("开始运行")); //更新状态
	this->setPushButtonsEnabled(false); //禁用按键
	this->reset(); //重置检测子模块
	
	if (runtimeParams->DeveloperMode) { //开发者模式
		serialNumberUI->reset(); //重置
		serialNumberUI->showFullScreen(); //直接显示产品序号界面
		pcb::delay(10);//延迟
		this->hide(); //隐藏检测界面
	}
	else { //标准模式
		if (runtimeParams->currentRow_detect == -1 && !detectThread->isRunning()) {
			//运动到初始拍照位置
			ui.label_status->setText(pcb::chinese("运动结构前进中"));
			qApp->processEvents();
			motionControler->setOperation(MotionControler::MoveToInitialPos);
			motionControler->start();
		}
	}
}

//返回
void DetectUI::on_pushButton_return_clicked()
{
	//设置按键
	this->setPushButtonsEnabled(false);
	//重置检测子界面，清空缓存数据
	this->reset(); 

	//运动结构复位
	//if (!runtimeParams->DeveloperMode) {
	//	ui.label_status->setText(pcb::chinese("运动结构复位中"));
	//	qApp->processEvents();
	//	motionControler->setOperation(MotionControler::ResetControler);
	//	motionControler->start();
	//	while (motionControler->isRunning()) pcb::delay(100);//等待复位结束
	//	if (!motionControler->isReady()) {
	//		motionControler->showMessageBox(this);
	//		pcb::delay(10);
	//	}
	//}

	//设置按键
	this->setPushButtonsEnabled(true);
	//发送返回信号
	emit showDetectMainUI();
}



/***************** 获取外部信号 ******************/

//暂时使用敲击键盘按键模拟外部信号
void DetectUI::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_PageUp:
		qDebug() << "==================== Key_PageUp";
		break;
	case Qt::Key_PageDown:
		qDebug() << "==================== Key_PageDown";
		break;
	case Qt::Key_Up:
		qDebug() << "==================== Up";
		break;
	case Qt::Key_Down:
		qDebug() << "==================== Down";
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << "==================== Enter";
		break;
	default:
		break;
	}
}


/***************** 读图、显示与提取 *****************/

//读取相机组拍摄的一组分图 - 直接从硬盘上读图
void DetectUI::readSampleImages()
{
	clock_t t1 = clock();
	qDebug() << "====================" << pcb::chinese("读取分图")
		<< "( currentRow_show =" << currentRow_show << ")" << endl;

	//更新行号和状态栏
	ui.label_status->setText(pcb::chinese("正在读取第") +
		QString::number(currentRow_show + 1) +
		pcb::chinese("行分图"));//更新状态
	qApp->processEvents();

	//读取目录下的样本图像
	QDir dir(runtimeParams->currentSampleDir);
	dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNumberOfSampleImage(); return; }

	//读取并储存图像 - 从硬盘上读图 (用于临时调试)
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1; //0 1 ... nPhotographing-1
		int iCamera = idxs[1].toInt() - 1; //0 1 ... nCamera-1
		if (iPhotographing != currentRow_show) continue;
		if (iPhotographing < 0 || iPhotographing >= runtimeParams->nPhotographing) continue;
		if (iCamera < 0 || iCamera >= runtimeParams->nCamera) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //样本图的路径
		cv::Mat img = cv::imread(filepath.toStdString(), cv::IMREAD_COLOR);
		cvmatSamples[currentRow_show][iCamera] = new cv::Mat(img);
	}

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("分图读取结束：")
		<< (t2 - t1) << "ms ( currentRow_show =" << currentRow_show << ")" << endl;

	//图像类型转换
	imgConvertThread.start();
}


//显示相机组拍摄的一组分图（图像显示网格中的一行）
void DetectUI::showSampleImages()
{
	qDebug() << "====================" << pcb::chinese("显示分图")
		<< " ( currentRow_show =" << currentRow_show << ")" << endl;
	clock_t t1 = clock();

	QSize _itemSize(itemSize.width(), itemSize.height());
	if (ItemSpacing == 0) _itemSize += QSize(2, 2); //防止出现缝隙
	for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
		QPixmap scaledImg = (*qpixmapSamples[currentRow_show][iCamera]).scaled(_itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //定义图元
		item->setPos(itemGrid[currentRow_show][iCamera]); //图元的显示位置
		itemArray[currentRow_show][iCamera] = item; //存入图矩阵
	}

	//加载相机组新拍摄的一行图元
	for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//视图显示设置
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //设置垂直滚动条的位置
	ui.graphicsView->show();//显示

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("分图显示结束：") << (t2 - t1)
		<< "ms ( currentRow_show =" << currentRow_show << ")" << endl;
}


/******************** 字符识别 ********************/

//通过鼠标双击打开字符识别界面
void DetectUI::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (currentRow_show == -1) return;//若没有显示对应的样本图则直接返回
	if (event->button() == Qt::RightButton) return;//忽略右键点击

	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	QPoint mousePosition(event->x(), event->y());

	//判断鼠标点击的是哪个分图
	if (!graphicsViewRect.contains(mousePosition)) return;
	QPoint relativePos = mousePosition - graphicsViewPos; //相对位置
	int gridRowIdx = (int) (relativePos.y() / gridSize.height());//点击位置在第几行
	int gridColIdx = (int) (relativePos.x() / gridSize.width());//点击位置在第几列

	if (true && gridRowIdx <= currentRow_show) {
		serialNumberUI->showSampleImage(gridRowIdx, gridColIdx);
		pcb::delay(3);//延迟
		serialNumberUI->showFullScreen();//显示序号识别界面
		pcb::delay(10);//延迟
		this->hide();
	}
}

//从序号识别界面获得产品序号之后
void DetectUI::on_recognizeFinished_serialNumUI()
{
	//判断是直接从本地读图，调用相机获取图像
	if (runtimeParams->DeveloperMode) { //开发者模式
		//判断对应样本文件夹是否存在
		runtimeParams->currentSampleDir = userConfig->SampleDirPath + "/"
			+ runtimeParams->sampleModelNum + "/"
			+ runtimeParams->sampleBatchNum + "/" + runtimeParams->sampleNum;
		if (!QFileInfo(runtimeParams->currentSampleDir).exists()) {
			runtimeParams->showMessageBox(this, RuntimeParams::Invalid_serialNum);
			return;
		}
		
		//直接显示新的样本行
		if (currentRow_show + 1 < runtimeParams->nPhotographing) { 
			currentRow_show += 1; //更新显示行号
			readSampleImages(); //读图 - 相当于相机拍照		
		}
	}
	else { //标准模式
		if (detectThread->isRunning()) return;

		if (runtimeParams->currentRow_detect == runtimeParams->nPhotographing - 1)
			reset();//重置检测子模块

		//判断对应模板文件夹是否存在
		runtimeParams->currentTemplDir = userConfig->TemplDirPath + "/"
			+ runtimeParams->sampleModelNum;
		if (!QFileInfo(runtimeParams->currentTemplDir).exists()) {
			//runtimeParams->showMessageBox(this, RuntimeParams::Invalid_serialNum);
			qDebug() << pcb::chinese("没有找到与型号对应的模板文件夹");
			//return;
		}

		if (eventCounter >= 1)
			detectSampleImages(); //检测
	}	
}

//显示序号识别界面的上一级界面
void DetectUI::do_showPreviousUI_serialNumUI()
{
	if (!runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)
		&& runtimeParams->DeveloperMode) 
	{
		this->setPushButtonsEnabled(true); //启用按键
	}
	
	this->showFullScreen();
	pcb::delay(10);//延迟
	serialNumberUI->hide();
}


/******************** 运动控制 ********************/

//复位结束
void DetectUI::on_resetControlerFinished_motion(int caller)
{
	//复位失败
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//已经拍完所有分图，但未开始检测
	if (currentRow_show == runtimeParams->nPhotographing - 1
		&& runtimeParams->currentRow_detect == -1)
	{
		ui.label_status->setText(pcb::chinese("请在序号识别界面\n")
			+ pcb::chinese("获取产品序号"));
		qApp->processEvents();
		pcb::delay(10); //延迟
	}
}

//到初始达拍照位置
void DetectUI::on_moveToInitialPosFinished_motion()
{
	//检查运动结构的状态
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//调用相机进行拍照
	if (currentRow_show + 1 < runtimeParams->nPhotographing) {
		currentRow_show += 1; //更新显示行号

		ui.label_status->setText(pcb::chinese("正在拍摄第") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("行分图"));//更新状态
		qApp->processEvents();

		cameraControler->start(); //拍照
	}
}

//运动结构前进结束
void DetectUI::on_moveForwardFinished_motion()
{
	//检查运动结构的状态
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//调用相机进行拍照
	if (currentRow_show + 1 < runtimeParams->nPhotographing) {
		currentRow_show += 1; //更新显示行号

		ui.label_status->setText(pcb::chinese("正在拍摄第") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("行分图"));//更新状态
		qApp->processEvents();

		cameraControler->start(); //拍照
	}
}


/******************** 相机控制 ********************/

//更新相机控制器的参数
void DetectUI::refreshCameraControler()
{
	cameraControler->setCurrentRow(&currentRow_show);//设置行号
	cameraControler->setCvMatSamples(&cvmatSamples);//设置图像容器
	cameraControler->setOperation(CameraControler::TakePhotos);//设置相机操作
}

//相机初始化结束
void DetectUI::on_initCamerasFinished_camera(int)
{
}

//相机拍摄结束
void DetectUI::on_takePhotosFinished_camera(int)
{
	//调用图像类型转换线程
	imgConvertThread.start();
}


/******************** 图像转换线程 ********************/

//图像转换结束，在界面上显示图像，然后提取模板
void DetectUI::on_convertFinished_convertThread()
{
	//更新事件计数器
	eventCounter += 1;

	//更新状态栏
	if (runtimeParams->DeveloperMode) {
		ui.label_status->setText(pcb::chinese("第") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("行读取结束"));
	}
	else {
		ui.label_status->setText(pcb::chinese("第") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("行拍摄结束"));
	}
	qApp->processEvents();
	pcb::delay(10); //延迟

	//在界面上显示样本图
	showSampleImages();

	//更新状态栏
	if (!runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)
		&& !runtimeParams->DeveloperMode) 
	{
		ui.label_status->setText(pcb::chinese("请在序号识别界面\n")
			+ pcb::chinese("获取产品序号"));
		qApp->processEvents();
		pcb::delay(10); //延迟
	}

	//显示结束后之前驱动机械结构运动
	pcb::delay(10); //延迟
	if (!runtimeParams->DeveloperMode) {
		if (currentRow_show + 1 < runtimeParams->nPhotographing) {
			ui.label_status->setText(pcb::chinese("运动结构前进中"));
			qApp->processEvents();
			motionControler->setOperation(MotionControler::MoveForward);
			motionControler->start(); //运动结构前进
		}
		else { //当前PCB已拍完
			ui.label_status->setText(pcb::chinese("运动结构复位中"));
			qApp->processEvents();
			motionControler->setOperation(MotionControler::MotionReset);
			motionControler->start(); //运动结构复位

			//如果此时还没开始提取，则可以点击返回按键
			if (runtimeParams->currentRow_detect == -1) {
				ui.pushButton_return->setEnabled(true);
			}
		}
	}
	else { //开发者模式
	}

	//判断是否执行检测操作
	if (runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)
		&& eventCounter >= 1 && !detectThread->isRunning())
	{
		detectSampleImages(); //检测
	}
}


/******************** 模板提取线程 ********************/

//检测当前的一行样本图像
void DetectUI::detectSampleImages()
{
	//禁用返回键
	ui.pushButton_return->setEnabled(false);

	//更新提取行号
	runtimeParams->currentRow_detect += 1;

	//更新状态栏
	ui.label_status->setText(pcb::chinese("正在检测第") +
		QString::number(runtimeParams->currentRow_detect + 1) +
		pcb::chinese("行分图"));
	qApp->processEvents();

	//开启检测线程
	detectThread->start();
}

//提取结束后更新状态
void DetectUI::do_updateDetectState_detecter(int state)
{
	if ((DefectDetecter::DetectState)state == DefectDetecter::Finished) { //检测完一行图像
		//更新事件计数器
		eventCounter -= 1; 

		//更新状态栏
		ui.label_status->setText(QString::fromLocal8Bit("第") +
			QString::number(runtimeParams->currentRow_detect + 1) +
			QString::fromLocal8Bit("行检测结束"));//更新状态栏
		qApp->processEvents();

		//显示新的样本行
		if (currentRow_show + 1 < runtimeParams->nPhotographing
			&& runtimeParams->DeveloperMode) {
			currentRow_show += 1; //更新显示行号
			readSampleImages(); //读图 - 相当于相机拍照		
		}

		//检查是否有未处理的事件
		while (detectThread->isRunning()) pcb::delay(50); //等待提取线程结束
		while (motionControler->isRunning()) pcb::delay(100); //等待运动线程结束
		if (runtimeParams->currentRow_detect == runtimeParams->nPhotographing - 1) { //当前PCB检测结束
			runtimeParams->currentRow_detect = -1;
			this->setPushButtonsEnabled(true); //启用按键
		}
		else { //当前PCB未检测完
			if (eventCounter > 0) detectSampleImages(); //检测下一行分图
		}
	}
}


//检测线程结束后
void DetectUI::on_detectFinished_detectThread(bool qualified)
{
	//显示结果
	ui.label_indicator->setPixmap((qualified) ? lightOffIcon : lightOnIcon); //切换指示灯
	ui.label_result->setText((qualified) ? pcb::chinese("合格") : pcb::chinese("不合格"));
}