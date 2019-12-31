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
	defectDetecter = Q_NULLPTR;
	detectThread = Q_NULLPTR;
	fileSyncThread = Q_NULLPTR;
	recheckUI = Q_NULLPTR;
	detectState = DefectDetecter::Default;
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
	delete fileSyncThread; //文件同步线程
	fileSyncThread = Q_NULLPTR;
	delete recheckUI; //复查界面
	recheckUI = Q_NULLPTR;
}


/****************** 界面的初始化与重置 *******************/

void DetectUI::init()
{
	//选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//设置检测界面的聚焦策略
	this->setFocusPolicy(Qt::ClickFocus);

	//在界面右上角显示时间
	this->showCurrentTime();

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

	//重置缺陷检测模块
	runtimeParams->productID.resetModelType(); //重置产品id
	ui.label_modelType->setText(runtimeParams->productID.modelType); //重置型号
	this->reset(); //重置检测界面

	//对绘图控件GraphicsView的初始化设置
	this->initGraphicsView();

	//缺陷检测器
	delete defectDetecter;
	defectDetecter = new DefectDetecter;
	connect(defectDetecter, SIGNAL(updateDetectState_detecter(int)), this, SLOT(do_updateDetectState_detecter(int)));
	connect(defectDetecter, SIGNAL(detectFinished_detectThread(bool)), this, SLOT(on_detectFinished_detectThread(bool)));

	//检测线程（调用检测器进行检测）
	delete detectThread;
	detectThread = new DetectThread;
	detectThread->setAdminConfig(adminConfig);
	detectThread->setUserConfig(userConfig);
	detectThread->setRuntimeParams(runtimeParams);
	detectThread->setSampleImages(&cvmatSamples);
	detectThread->setDetectResult(&detectResult);
	detectThread->setDefectDetecter(defectDetecter);
	detectThread->init();

	//运动控制
	connect(motionControler, SIGNAL(moveToInitialPosFinished_motion(int)), this, SLOT(on_moveToInitialPosFinished_motion(int)), Qt::UniqueConnection);
	connect(motionControler, SIGNAL(motionResetFinished_motion(int)), this, SLOT(on_motionResetFinished_motion(int)), Qt::UniqueConnection);
	connect(motionControler, SIGNAL(moveForwardFinished_motion(int)), this, SLOT(on_moveForwardFinished_motion(int)), Qt::UniqueConnection);

	//相机控制
	connect(cameraControler, SIGNAL(initCamerasFinished_camera(int)), this, SLOT(on_initCamerasFinished_camera(int)), Qt::UniqueConnection);
	connect(cameraControler, SIGNAL(takePhotosFinished_camera(int)), this, SLOT(on_takePhotosFinished_camera(int)), Qt::UniqueConnection);

	//转换线程
	imgConvertThread.setCvMats(&cvmatSamples);
	imgConvertThread.setQPixmaps(&qpixmapSamples);
	imgConvertThread.setCurrentRow(&currentRow_show);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);
	connect(&imgConvertThread, SIGNAL(convertFinished_convertThread()), this, SLOT(on_convertFinished_convertThread()), Qt::UniqueConnection);

	//复查界面
	delete recheckUI;
	recheckUI = new RecheckUI();
	recheckUI->setUserConfig(userConfig);
	recheckUI->setRuntimeParams(runtimeParams);
	recheckUI->init();
	connect(recheckUI, SIGNAL(recheckFinished_recheckUI()), this, SLOT(on_recheckFinished_recheckUI()), Qt::UniqueConnection);


	//文件同步线程
	delete fileSyncThread;
	fileSyncThread = new FileSyncThread();
	fileSyncThread->setAdminConfig(adminConfig);
	fileSyncThread->setUserConfig(userConfig);
	fileSyncThread->setRuntimeParams(runtimeParams);
}

//在界面上显示时间
void DetectUI::showCurrentTime()
{
	//定时器
	QTimer *timer = new QTimer(this);
	timer->start(0);
	//使用定时器信号槽，尽快更新时间的显示
	connect(timer, &QTimer::timeout, [=]() {
		QDateTime current_time = QDateTime::currentDateTime();
		//显示时间，格式为：年-月-日 时：分：秒 周几
		QString StrCurrentTime = current_time.toString("yyyy/MM/dd hh:mm:ss");
		//时间显示格式可自由设定，如hh:mm ddd 显示 时：分 周几 
		//具体格式见函数QString QDateTime::​toString(const QString & format) const
		ui.label_currentTime->setText(StrCurrentTime);
	});
}

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
	ui.pushButton_modelType->setEnabled(enable);
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

//选择产品型号
void DetectUI::on_pushButton_modelType_clicked()
{
	QString caption = pcb::chinese("请选择产品型号");
	QString path = pcb::selectDirPath(this, caption, userConfig->TemplDirPath);
	QString dirName = QDir(path).dirName().trimmed(); //文件夹名称，即产品型号
	if (dirName == "" || !path.startsWith(userConfig->TemplDirPath)) return;
	ui.label_modelType->setText(dirName);
	runtimeParams->productID.setModelType(dirName); //显示获取到的文件夹名称
}

//开始检测
void DetectUI::on_pushButton_start_clicked()
{
	if (runtimeParams->currentRow_extract != -1 || detectThread->isRunning()) return;
	qDebug() << ">>>>>>>>>>>>>>>>>>>>" << pcb::chinese("开始运行") << endl;
	ui.label_status->setText(pcb::chinese("开始运行")); //更新状态
	this->setPushButtonsEnabled(false); //禁用按键
	this->reset(); //重置检测子模块
	
	//如果产品型号无效
	if (!runtimeParams->productID.isModelTypeValid()) {
		on_pushButton_modelType_clicked(); //先获取产品类型
	}
	runtimeParams->productID.update(); //更新产品序号

	//获取样本图像
	if (runtimeParams->DeveloperMode) { //开发者模式
		pcb::delay(10);
		readSampleImages(); //直接从硬盘上读图
	}
	else { //标准模式
		//运动到初始拍照位置
		ui.label_status->setText(pcb::chinese("运动结构前进中"));
		qApp->processEvents();
		motionControler->setOperation(MotionControler::MoveToInitialPos);
		motionControler->start();
	}
}

//返回主界面
void DetectUI::on_pushButton_return_clicked()
{
	this->setPushButtonsEnabled(false); //设置按键
	this->reset(); //重置检测子界面，清空缓存数据
	this->setPushButtonsEnabled(true); //设置按键
	emit showDetectMainUI(); //发送返回信号
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
	case Qt::Key_Space:
		qDebug() << "==================== Space";
		//敲击空格键进入复查界面

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
	qDebug() << "====================" << pcb::chinese("开始读取样本图") << endl;

	//选取样本图像文件
	QString sampleDirPath = pcb::selectDirPath(this, 
		pcb::chinese("请选取样本图像"), userConfig->SampleDirPath);

	//读取目录下的样本图像
	QDir dir(sampleDirPath);
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
		if (iPhotographing < 0 || iPhotographing >= runtimeParams->nPhotographing) continue;
		if (iCamera < 0 || iCamera >= runtimeParams->nCamera) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //样本图的路径
		cv::Mat img = cv::imread(filepath.toStdString(), cv::IMREAD_COLOR);
		cvmatSamples[iPhotographing][iCamera] = new cv::Mat(img);
	}

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("图像读取结束：")
		<< (t2 - t1) << "ms" << endl;

	//图像类型转换
	currentRow_show = 0;
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
	qApp->processEvents();

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("显示分图：") << (t2 - t1)
		<< "ms ( currentRow_show =" << currentRow_show << ")" << endl;
}


/******************** 字符识别 ********************/

//通过鼠标双击打开字符识别界面
//void DetectUI::mouseDoubleClickEvent(QMouseEvent *event)
//{
//	if (currentRow_show == -1) return;//若没有显示对应的样本图则直接返回
//	if (event->button() == Qt::RightButton) return;//忽略右键点击
//
//	QPoint graphicsViewPos = ui.graphicsView->pos();
//	QSize graphicsViewSize = ui.graphicsView->size();
//	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
//	QPoint mousePosition(event->x(), event->y());
//	if (!graphicsViewRect.contains(mousePosition)) return;//保证点击的位置在视图中
//
//	//判断鼠标点击的是哪个分图
//	QPointF posInScene = ui.graphicsView->mapToScene(event->pos());//点击位置相对于场景的坐标
//	int gridRowIdx = (int) (posInScene.y() / gridSize.height());//计算点击位置在第几行
//	int gridColIdx = (int) (posInScene.x() / gridSize.width());//计算点击位置在第几列
//
//	if (gridRowIdx <= currentRow_show && qpixmapSamples[gridRowIdx][gridColIdx] != Q_NULLPTR) {
//		serialNumberUI->showSampleImage(gridRowIdx, gridColIdx);
//		pcb::delay(3);//延迟
//		serialNumberUI->showFullScreen();//显示序号识别界面
//		pcb::delay(10);//延迟
//		this->hide();
//	}
//}

//从序号识别界面获得产品序号之后
//void DetectUI::on_recognizeFinished_serialNumUI()
//{
//	//判断是直接从本地读图，调用相机获取图像
//	if (runtimeParams->DeveloperMode) { //开发者模式
//		//判断对应样本文件夹是否存在
//		runtimeParams->currentSampleDir = userConfig->SampleDirPath + "/"
//			+ runtimeParams->sampleModelNum + "/"
//			+ runtimeParams->sampleBatchNum + "/" + runtimeParams->sampleNum;
//		if (!QFileInfo(runtimeParams->currentSampleDir).exists()) {
//			runtimeParams->showMessageBox(this, RuntimeParams::Invalid_serialNum);
//			setPushButtonsEnabled(true); return;
//		}
//		
//		//直接显示新的样本行
//		if (currentRow_show + 1 < runtimeParams->nPhotographing) { 
//			currentRow_show += 1; //更新显示行号
//			readSampleImages(); //读图 - 相当于相机拍照		
//		}
//	}
//	else { //标准模式
//		if (detectThread->isRunning()) return;
//
//		//判断对应模板文件夹是否存在
//		runtimeParams->currentTemplDir = userConfig->TemplDirPath + "/"
//			+ runtimeParams->sampleModelNum;
//		if (!QFileInfo(runtimeParams->currentTemplDir).exists()) {
//			//runtimeParams->showMessageBox(this, RuntimeParams::Invalid_serialNum);
//			qDebug() << pcb::chinese("没有找到与型号对应的模板文件夹");
//			//return;
//		}
//
//		if (eventCounter >= 1) {
//			detectSampleImages(); //检测
//		}
//	}	
//}

//显示序号识别界面的上一级界面
//void DetectUI::do_showPreviousUI_serialNumUI()
//{
//	if (!runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)
//		&& runtimeParams->DeveloperMode) 
//	{
//		this->setPushButtonsEnabled(true); //启用按键
//	}
//	
//	this->showFullScreen();
//	pcb::delay(10);//延迟
//	serialNumberUI->hide();
//}


/******************** 运动控制 ********************/

//到初始达拍照位置
void DetectUI::on_moveToInitialPosFinished_motion(int errorCode)
{
	if (motionControler->getCaller() != 2) return;

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
void DetectUI::on_moveForwardFinished_motion(int errorCode)
{
	if (motionControler->getCaller() != 2) return;

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

//运动复位结束
void DetectUI::on_motionResetFinished_motion(int errorCode)
{
	if (motionControler->getCaller() != 2) return;

	//复位失败
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//已经拍完所有分图，但未开始检测
	if (currentRow_show == runtimeParams->nPhotographing - 1
		&& runtimeParams->currentRow_detect == -1)
	{
		//如果产品id无效，则仍然进行提示
		if (false) {
			ui.label_status->setText(pcb::chinese("请输入产品ID \n"));
			qApp->processEvents();
		}

		//启用开始键和返回键
		if (!motionControler->isRunning()) {
			this->setPushButtonsEnabled(true);
		}
	}
}


/******************** 相机控制 ********************/

//更新相机控制器的参数
void DetectUI::refreshCameraControler()
{
	if (cameraControler->getCaller() != 2) return;

	cameraControler->setCurrentRow(&currentRow_show);//设置行号
	cameraControler->setCvMatSamples(&cvmatSamples);//设置图像容器
	cameraControler->setOperation(CameraControler::TakePhotos);//设置相机操作
}

//相机初始化结束
void DetectUI::on_initCamerasFinished_camera(int)
{
	if (cameraControler->getCaller() != 2) return;
}

//相机拍摄结束
void DetectUI::on_takePhotosFinished_camera(int)
{
	if (cameraControler->getCaller() != 2) return;

	//调用图像类型转换线程
	imgConvertThread.start();
}


/******************** 图像转换线程 ********************/

//图像转换结束，在界面上显示图像，然后提取模板
void DetectUI::on_convertFinished_convertThread()
{
	//更新事件计数器
	eventCounter += 1;

	//在界面上显示样本图
	this->showSampleImages();

	//更新状态栏
	QString str = pcb::chinese("第") + QString::number(currentRow_show + 1);
	if (runtimeParams->DeveloperMode) { //开发者模式
		ui.label_status->setText(str + pcb::chinese("行读取结束"));
	}
	else { //标准模式
		ui.label_status->setText(str + pcb::chinese("行拍摄结束"));
	}
	qApp->processEvents();
	pcb::delay(10); //延迟

	//更新状态栏
	if (!runtimeParams->productID.isModelTypeValid()) {
		//如果序号识别界面不在显示状态（即本界面显示中）则进行提示
		ui.label_status->setText(pcb::chinese("设置产品型号"));
		qApp->processEvents();
		return;
	}

	//当前行显示结束之后
	pcb::delay(10); //延迟
	if (runtimeParams->DeveloperMode) { //标准模式
		//开始转换下一行
		currentRow_show += 1;
		if (currentRow_show < runtimeParams->nPhotographing) {
			pcb::delay(300);
			imgConvertThread.start();
		}
		//如果所有图像显示完，但是未开始提取
		if (detectionHasNotStartedAndAllImagesHasBeenDisplayed()) {
			this->setPushButtonsEnabled(true);
		}
	}
	else { //标准模式
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
		}
	}

	//判断是否执行检测操作
	if (runtimeParams->productID.isModelTypeValid() //产品型号正确
		&& eventCounter > 0 && !detectThread->isRunning()) //有未检测的行，检测线程没运行
	{
		detectSampleImages(); //检测
	}
}


/******************** 缺陷检测线程 ********************/

//如果所有图像显示完，但是未开始检测
bool DetectUI::detectionHasNotStartedAndAllImagesHasBeenDisplayed()
{
	bool flag = (runtimeParams->currentRow_detect == -1); //未开始检测
	flag &= (currentRow_show == runtimeParams->nPhotographing - 1); //所有图像已经显示完
	return flag;
}

//检测当前的一行样本图像
void DetectUI::detectSampleImages()
{
	//禁用开始键和返回键
	this->setPushButtonsEnabled(false);

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
		pcb::delay(10); //延迟

		//显示新的样本行
		if (currentRow_show + 1 < runtimeParams->nPhotographing
			&& runtimeParams->DeveloperMode) {
			currentRow_show += 1; //更新显示行号
			readSampleImages(); //读图 - 相当于相机拍照		
		}

		//检查是否有未处理的事件
		while (detectThread->isRunning()) pcb::delay(50); //等待提取线程结束
		if (eventCounter > 0) detectSampleImages(); //检测下一行分图
	}
}


//检测线程结束后
void DetectUI::on_detectFinished_detectThread(bool SampleIsQualified)
{
	//显示结果
	ui.label_indicator->setPixmap((SampleIsQualified) ? lightOffIcon : lightOnIcon); //切换指示灯
	ui.label_result->setText((SampleIsQualified) ? pcb::chinese("合格") : pcb::chinese("不合格"));
	qApp->processEvents();

	//若产品不合格则直接显示检测结果
	if (!SampleIsQualified) this->switchToRecheckUI();

	//此处向复查设备发送检测结果
	//ui.label_status->setText(pcb::chinese("正在发送检测结果")); //更新状态栏
	//qApp->processEvents();
	//fileSyncThread->start(); //发送当前检测结果

	ui.label_status->setText(pcb::chinese("检测结束，系统就绪")); //更新状态栏
	qApp->processEvents();

	//当前PCB检测结束
	if (runtimeParams->currentRow_detect == runtimeParams->nPhotographing - 1) {
		runtimeParams->currentRow_detect += 1;
		this->setPushButtonsEnabled(true); //启用按键
	}
}


/******************** 复查界面 ********************/

//从检测界面切换到复查界面
void DetectUI::switchToRecheckUI()
{
	//TODO 先转换数据 ？

	recheckUI->showFullScreen();
	pcb::delay(10);
	this->hide();
}

//隐藏复查界面，显示检测界面
void DetectUI::on_recheckFinished_recheckUI()
{
	this->showFullScreen();
	pcb::delay(10);
	recheckUI->hide();
}
