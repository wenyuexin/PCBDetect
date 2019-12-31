#include "ExtractUI.h"

using pcb::UserConfig;
using pcb::RuntimeParams;
using cv::Mat;


ExtractUI::ExtractUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	adminConfig = Q_NULLPTR;
	userConfig = Q_NULLPTR;
	runtimeParams = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;
	templSettingUI = Q_NULLPTR;
	templExtractor = Q_NULLPTR;
	extractThread = Q_NULLPTR;
	initCounter = 0;
}

ExtractUI::~ExtractUI()
{
	qDebug() << "~ExtractUI";

	deletePointersInItemArray(itemArray); //删除图元矩阵中的指针
	deletePointersInCvMatArray(cvmatSamples); //删除cvmatSamples中的指针
	deletePointersInQPixmapArray(qpixmapSamples);//删除qpixmapSamples中的指针
	delete extractThread; //提取线程
	extractThread = Q_NULLPTR;
	delete templExtractor; //模板提取器
	templExtractor = Q_NULLPTR;
	delete templSettingUI; //模板设置界面
	templSettingUI = Q_NULLPTR;
}


/****************** 界面的初始化与重置 *******************/

void ExtractUI::init()
{
	//多屏状态下选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//设置检测界面的聚焦策略
	this->setFocusPolicy(Qt::ClickFocus);

	//在界面右上角显示时间
	this->showCurrentTime();

	//重置提取界面
	this->reset();

	//对绘图控件GraphicsView的初始化设置
	this->initGraphicsView();

	//模板设置界面
	delete templSettingUI;
	templSettingUI = new TemplSettingUI();
	templSettingUI->setAdminConfig(adminConfig);
	templSettingUI->setRuntimeParams(runtimeParams);
	templSettingUI->setCvMatArray(&cvmatSamples);
	templSettingUI->setQPixmapArray(&qpixmapSamples);
	templSettingUI->init();
	connect(templSettingUI, SIGNAL(maskRoiIsSet_templSettingUI()), this, SLOT(on_maskRoiIsSet_templSettingUI()));
	connect(templSettingUI, SIGNAL(segThreshIsSet_templSettingUI()), this, SLOT(on_segThreshIsSet_templSettingUI()));
	connect(templSettingUI, SIGNAL(modelNumIsSet_templSettingUI()), this, SLOT(on_modelNumIsSet_templSettingUI()));
	connect(templSettingUI, SIGNAL(showExtractUI_templSettingUI()), this, SLOT(do_showExtractUI_templSettingUI()));
	connect(templSettingUI, SIGNAL(settingFinished_templSettingUI()), this, SLOT(on_settingFinished_templSettingUI()));

	//模板提取器
	delete templExtractor;
	templExtractor = new TemplateExtractor;
	connect(templExtractor, SIGNAL(extractState_extractor(int)), this, SLOT(update_extractState_extractor(int)));

	//模板提取线程
	delete extractThread;
	extractThread = new ExtractThread();
	extractThread->setAdminConfig(adminConfig);
	extractThread->setUserConfig(userConfig);
	extractThread->setRuntimeParams(runtimeParams);
	extractThread->setSampleImages(&cvmatSamples);
	extractThread->setTemplateExtractor(templExtractor);
	extractThread->init();

	//运动控制
	connect(motionControler, SIGNAL(moveToInitialPosFinished_motion(int)), this, SLOT(on_moveToInitialPosFinished_motion(int)), Qt::UniqueConnection);
	connect(motionControler, SIGNAL(moveForwardFinished_motion(int)), this, SLOT(on_moveForwardFinished_motion(int)), Qt::UniqueConnection);
	connect(motionControler, SIGNAL(motionResetFinished_motion(int)), this, SLOT(on_motionResetFinished_motion(int)), Qt::UniqueConnection);

	//相机控制
	connect(cameraControler, SIGNAL(initCamerasFinished_camera(int)), this, SLOT(on_initCamerasFinished_camera(int)), Qt::UniqueConnection);
	connect(cameraControler, SIGNAL(takePhotosFinished_camera(int)), this, SLOT(on_takePhotosFinished_camera(int)), Qt::UniqueConnection);

	//转换线程
	imgConvertThread.setCvMats(&cvmatSamples);
	imgConvertThread.setQPixmaps(&qpixmapSamples);
	imgConvertThread.setCurrentRow(&currentRow_show);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);
	connect(&imgConvertThread, SIGNAL(convertFinished_convertThread()), this, SLOT(on_convertFinished_convertThread()), Qt::UniqueConnection);
}

//在界面上显示时间
void ExtractUI::showCurrentTime()
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
void ExtractUI::initGraphicsView()
{
	initItemGrid(itemGrid);//初始化图元网格
	initPointersInItemArray(itemArray);//初始化itemArray
	initPointersInCvMatArray(cvmatSamples);//初始化cvmatSamples
	initPointersInQPixmapArray(qpixmapSamples);//初始化qpixmapSamples

	//初始化若干用于监视程序运行状态的变量
	currentRow_show = -1; //显示行号
	runtimeParams->currentRow_extract = -1; //检测行号
	eventCounter = 0; //事件计数器

	//视图控件的设置
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //设置聚焦策略
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用水平滚动条
	ui.graphicsView->setScene(&scene); //在视图中添加场景
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //设置垂直滑动条的位置
}

//重置模板提取界面
void ExtractUI::reset()
{
	templSettingUI->reset();//重置序号识别界面

	removeItemsFromGraphicsScene(); //移除场景中已经加载的图元
	deletePointersInItemArray(itemArray); //删除图元矩阵中的指针
	deletePointersInCvMatArray(cvmatSamples); //删除cvmatSamples中的指针
	deletePointersInQPixmapArray(qpixmapSamples);//删除qpixmapSamples中的指针

	ui.label_status->setText(""); //清空状态栏
	currentRow_show = -1; //显示行号的复位
	runtimeParams->currentRow_extract = -1; //提取行号的复位
	eventCounter = 0; //事件计数器
	ui.graphicsView->centerOn(0, 0); //垂直滑条复位
	qApp->processEvents();
}

//按键设置
void ExtractUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_start->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}


/********* 图元矩阵和样本图像矩阵的初始化和删除等操作 ***********/

//初始化图元网格
void ExtractUI::initItemGrid(pcb::ItemGrid &itemGrid)
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
	scene.setSceneRect(1, 1, sceneSize.width() + 2, sceneSize.height() + 2);

	//生成绘图网点
	QSizeF spacingBlock = QSizeF(ItemSpacing, ItemSpacing);
	gridSize = itemSize + spacingBlock; //每个网格的尺寸

	//判断itemGrid是否执行过初始化
	if (itemGrid.size() > 0) itemGrid.clear();

	//初始化赋值
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //行
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //列
			QPointF pos(ItemSpacing, ItemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		itemGrid.append(posList);
	}
}

//初始化图元矩阵中的指针 - ItemArray
void ExtractUI::initPointersInItemArray(pcb::ItemArray &items)
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
void ExtractUI::deletePointersInItemArray(pcb::ItemArray &items)
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
void ExtractUI::initPointersInCvMatArray(pcb::CvMatArray &cvmats)
{
	//若执行过init函数，则先delete指针
	if (cvmats.size() > 0) {
		deletePointersInCvMatArray(cvmats);
	}

	//根据相机个数和拍照次数对矩阵进行初始化
	cvmats.resize(runtimeParams->nPhotographing);
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //行
		cvmats[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
			cvmats[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//删除样本图像向量中的指针 - CvMatArray
void ExtractUI::deletePointersInCvMatArray(pcb::CvMatArray &cvmats)
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
void ExtractUI::initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
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
void ExtractUI::deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
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
void ExtractUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);
	}
}


/***************** 按键响应 *****************/

//开始提取新的PCB板
void ExtractUI::on_pushButton_start_clicked()
{
	if (runtimeParams->currentRow_extract != -1 || extractThread->isRunning()) return;
	qDebug() << ">>>>>>>>>>>>>>>>>>>>" << pcb::chinese("开始运行") << endl;
	ui.label_status->setText(pcb::chinese("开始运行"));
	this->setPushButtonsEnabled(false); //禁用按键
	this->reset(); //重置模板提取子模块

	if (runtimeParams->DeveloperMode) { //开发者模式
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
void ExtractUI::on_pushButton_return_clicked()
{
	this->setPushButtonsEnabled(false); //设置按键
	this->reset(); //重置模板提取界面，清空缓存数据
	this->setPushButtonsEnabled(true); //设置按键
	emit showDetectMainUI(); //发送返回信号
}


/***************** 获取外部信号 ******************/

//暂时使用敲击键盘按键模拟外部信号
void ExtractUI::keyPressEvent(QKeyEvent *event)
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
void ExtractUI::readSampleImages()
{
	clock_t t1 = clock();
	qDebug() << "====================" << pcb::chinese("开始读取样本图")  << endl;

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
		//if (iPhotographing != currentRow_show) continue;
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
void ExtractUI::showSampleImages()
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
	qDebug() << "====================" << pcb::chinese("分图显示结束：") << (t2 - t1)
		<< "ms ( currentRow_show =" << currentRow_show << ")" << endl;
}


/******************** 模板设置界面 ********************/

//通过鼠标双击进而xxx
void ExtractUI::mouseDoubleClickEvent(QMouseEvent *event)
{
	return;
}

//通过鼠标单击打开模板设置界面
void ExtractUI::mousePressEvent(QMouseEvent *event)
{
	if (currentRow_show == -1) return;//若没有显示对应的样本图则直接返回
	if (event->button() == Qt::RightButton) return;//忽略右键点击

	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	QPoint mousePosition(event->x(), event->y());
	if (!graphicsViewRect.contains(mousePosition)) return;

	QPointF posInScene = ui.graphicsView->mapToScene(event->pos());//点击位置相对于场景的坐标
	int gridRowIdx = (int)(posInScene.y() / gridSize.height());//计算点击位置是整图中的第几行
	int gridColIdx = (int)(posInScene.x() / gridSize.width());//计算点击位置是整图中的第几列

	if (gridRowIdx <= currentRow_show && qpixmapSamples[gridRowIdx][gridColIdx] != Q_NULLPTR) {
		templSettingUI->showSampleImage(gridRowIdx, gridColIdx);
		templSettingUI->showFullScreen();//显示序号识别界面
		pcb::delay(10);//延迟
		this->hide(); //隐藏提取界面
	}
}


//掩膜区域坐标获取结束之后
void ExtractUI::on_maskRoiIsSet_templSettingUI()
{
	on_settingFinished_templSettingUI();
}

//获取图像分割阈值之后
void ExtractUI::on_segThreshIsSet_templSettingUI()
{
	on_settingFinished_templSettingUI();
}

//获取产品型号之后
void ExtractUI::on_modelNumIsSet_templSettingUI()
{
	on_settingFinished_templSettingUI();
}


//模板参数都已设置时
void ExtractUI::on_settingFinished_templSettingUI()
{
	//如果图像都已拍完但未开始提取
	if (extractionHasNotStartedAndAllImagesHasBeenDisplayed()) {
		this->setPushButtonsEnabled(true); //启用按键
	}

	//如果模板参数无效，则提示用户设置
	if (!templSettingUI->isReadyForExtract()) {
		ui.label_status->setText(pcb::chinese("请在模板设置界面\n输入相关参数"));
		qApp->processEvents();
		pcb::delay(10); //延迟
		return;
	}

	if (eventCounter >= 1 && !extractThread->isRunning() && runtimeParams->productID.isModelTypeValid()) {
		extractTemplateImages(); //提取
	}
}

//隐藏模板设置界面，显示模板提取界面
void ExtractUI::do_showExtractUI_templSettingUI()
{
	//如果图像都已显示，同时图像都已拍完但未开始提取
	if (extractionHasNotStartedAndAllImagesHasBeenDisplayed()) {
		this->setPushButtonsEnabled(true); //启用按键
	}

	this->showFullScreen();
	pcb::delay(10);//延迟
	templSettingUI->hide();
}


/******************** 运动控制 ********************/

//到初始达拍照位置
void ExtractUI::on_moveToInitialPosFinished_motion(int errorcode)
{
	if (motionControler->getCaller() != 1) return;

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
void ExtractUI::on_moveForwardFinished_motion(int errorcode)
{
	if (motionControler->getCaller() != 1) return;

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

//复位结束
void ExtractUI::on_motionResetFinished_motion(int errorcode)
{
	if (motionControler->getCaller() != 1) return;

	//复位失败
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//已经拍完所有分图，但未开始提取
	if (extractionHasNotStartedAndAllImagesHasBeenDisplayed()) {
		//如果模板参数无效，则仍然进行提示
		if (!templSettingUI->isReadyForExtract()) {
			ui.label_status->setText(pcb::chinese("请在模板设置界面\n输入相关参数"));
			qApp->processEvents();
		}
		//启用开始键和返回键
		if (!motionControler->isRunning()) {
			this->setPushButtonsEnabled(true);
		}
	}
}


/********************** 相机控制 **********************/

//更新相机控制器的参数
void ExtractUI::refreshCameraControler()
{
	if (cameraControler->getCaller() != 1) return;

	cameraControler->setCurrentRow(&currentRow_show);//设置行号
	cameraControler->setCvMatSamples(&cvmatSamples);//设置图像
	cameraControler->setOperation(CameraControler::TakePhotos);//设置相机操作
}

//相机初始化结束
void ExtractUI::on_initCamerasFinished_camera(int)
{
	if (cameraControler->getCaller() != 1) return;
}

//相机拍摄结束
void ExtractUI::on_takePhotosFinished_camera(int)
{
	if (cameraControler->getCaller() != 1) return;

	//调用图像类型转换线程
	imgConvertThread.start();
}


/******************** 图像转换线程 ********************/

//图像转换结束，在界面上显示图像，然后提取模板
void ExtractUI::on_convertFinished_convertThread()
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
	if (!templSettingUI->isReadyForExtract()) {
		//如果序号识别界面不在显示状态（即本界面显示中）则进行提示
		ui.label_status->setText(pcb::chinese("请在模板设置界面\n设置相关参数"));
		qApp->processEvents();
	}

	//当前行显示结束之后
	pcb::delay(10); //延迟
	if (runtimeParams->DeveloperMode) { //开发者模式
		//开始转换下一行
		currentRow_show += 1;
		if (currentRow_show < runtimeParams->nPhotographing) {
			pcb::delay(300);
			imgConvertThread.start();
		}
		//如果所有图像显示完，但是未开始提取
		if (extractionHasNotStartedAndAllImagesHasBeenDisplayed()) {
			this->setPushButtonsEnabled(true);
		}
	}
	else { //标准模式 - 显示结束后之前驱动机械结构运动
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

	//判断是否执行提取操作
	if (templSettingUI->isReadyForExtract() //模板相关参数都已正确设置
		&& eventCounter > 0 && !extractThread->isRunning()) //有未提取的行，提取线程没运行
	{
		extractTemplateImages(); //提取
	}
}


/******************** 模板提取线程 ********************/

//如果所有图像显示完，但是未开始提取
bool ExtractUI::extractionHasNotStartedAndAllImagesHasBeenDisplayed()
{
	bool flag = (runtimeParams->currentRow_extract == -1); //未开始提取
	flag &= (currentRow_show == runtimeParams->nPhotographing - 1); //所有图像已经显示完
	return flag;
}

//提取当前的一行样本图像
void ExtractUI::extractTemplateImages()
{
	//禁用开始键和返回键
	this->setPushButtonsEnabled(false);

	//更新提取行号
	runtimeParams->currentRow_extract += 1;

	//更新状态栏
	ui.label_status->setText(pcb::chinese("正在提取第") +
		QString::number(runtimeParams->currentRow_extract + 1) + 
		pcb::chinese("行模板"));
	qApp->processEvents();

	//开启检测线程
	extractThread->start();
}

//提取结束后更新状态
void ExtractUI::update_extractState_extractor(int state)
{
	//当提取完一行图像
	if ((TemplateExtractor::ExtractState)state == TemplateExtractor::Finished) { 
		//更新事件计数器
		eventCounter -= 1; 

		//更新状态栏
		ui.label_status->setText(pcb::chinese("第") +
			QString::number(runtimeParams->currentRow_extract + 1) +
			pcb::chinese("行提取结束"));
		qApp->processEvents();
		pcb::delay(10); //延迟

		//检查是否有未处理的事件
		while (extractThread->isRunning()) pcb::delay(50); //等待提取线程结束
		if (runtimeParams->currentRow_extract == runtimeParams->nPhotographing - 1) { //当前PCB提取结束
			pcb::delay(10);
			ui.label_status->setText(pcb::chinese("提取结束，系统就绪")); //更新状态栏
			qApp->processEvents();
			runtimeParams->currentRow_extract += 1;
			this->setPushButtonsEnabled(true); //启用按键
		}
		else { //当前PCB未提取完
			if (eventCounter <= 0) return;
			extractTemplateImages(); //提取下一行分图
		}
	}
}
