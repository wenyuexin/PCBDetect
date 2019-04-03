#include "TemplateUI.h"

using Ui::DetectConfig;
using Ui::DetectParams;
using cv::Mat;


TemplateUI::TemplateUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//设置检测界面的聚焦策略
	this->setFocusPolicy(Qt::ClickFocus);

	//产品序号识别界面
	connect(&serialNumberUI, SIGNAL(recognizeFinished_serialNumberUI()), this, SLOT(on_recognizeFinished_serialNumberUI()));
	connect(&serialNumberUI, SIGNAL(switchImage_serialNumberUI()), this, SLOT(on_switchImage_serialNumberUI()));

	//模板提取线程
	templExtractor = new TemplateExtractor;
	connect(templExtractor, SIGNAL(extractState_extractor(int)), this, SLOT(update_extractState_extractor(int)));

	templThread = new TemplateThread();
	templThread->setTemplateExtractor(templExtractor);
}

TemplateUI::~TemplateUI()
{
	deletePointersInItemArray(itemArray); //删除图元矩阵中的指针
	deletePointersInCvMatArray(cvmatSamples); //删除cvmatSamples中的指针
	deletePointersInQPixmapArray(qpixmapSamples);//删除qpixmapSamples中的指针
	delete templThread;
	delete templExtractor;
}

//因为对象实例的构造和实例指针传递的时序问题
//部分信号和槽函数不能直接在构造函数里连接，单独放这里
void TemplateUI::doConnect()
{
	//运动控制
	connect(motionControler, SIGNAL(moveForwardFinished_motion()), this, SLOT(on_moveForwardFinished_motion()));
	connect(motionControler, SIGNAL(resetControlerFinished_motion(int)), this, SLOT(on_resetControlerFinished_motion(int)));
	//相机控制
	connect(cameraControler, SIGNAL(initCamerasFinished_camera(int)), this, SLOT(on_initCamerasFinished_camera(int)));
	connect(cameraControler, SIGNAL(takePhotosFinished_camera(int)), this, SLOT(on_takePhotosFinished_camera(int)));
	//转换线程
	connect(&imgConvertThread, SIGNAL(convertFinished_convertThread()), this, SLOT(on_convertFinished_convertThread()));
}


/****************** 界面的初始化与重置 *******************/

//对绘图控件GraphicsView的初始化设置
void TemplateUI::initGraphicsView()
{
	initItemGrid(itemGrid);//初始化图元网格
	initPointersInItemArray(itemArray);//初始化itemArray
	initPointersInCvMatArray(cvmatSamples);//初始化cvmatSamples
	initPointersInQPixmapArray(qpixmapSamples);//初始化qpixmapSamples

	//初始化若干用于监视程序运行状态的变量
	currentRow_show = -1; //显示行号
	detectParams->currentRow_extract = -1; //检测行号
	eventCounter = 0; //事件计数器

	//配置转换线程
	imgConvertThread.setCvMats(&cvmatSamples);
	imgConvertThread.setQPixmaps(&qpixmapSamples);
	imgConvertThread.setCurrentRow(&currentRow_show);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);

	//产品序号识别界面
	serialNumberUI.setDetectParams(detectParams);
	serialNumberUI.setCvMatArray(&cvmatSamples);
	serialNumberUI.setQPixmapArray(&qpixmapSamples);

	//配置提取线程
	templThread->setDetectParams(detectParams);
	templThread->setDetectConfig(detectConfig);
	templExtractor->setDetectParams(detectParams);
	templExtractor->setDetectConfig(detectConfig);
	templExtractor->setSampleImages(&cvmatSamples);

	//视图控件的设置
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //设置聚焦策略
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用水平滚动条
	ui.graphicsView->setScene(&scene); //在视图中添加场景
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //设置垂直滑动条的位置
}

//重置模板提取界面
void TemplateUI::resetTemplateUI()
{
	removeItemsFromGraphicsScene(); //移除场景中已经加载的图元
	deletePointersInItemArray(itemArray); //删除图元矩阵中的指针
	deletePointersInCvMatArray(cvmatSamples); //删除cvmatSamples中的指针
	deletePointersInQPixmapArray(qpixmapSamples);//删除qpixmapSamples中的指针
	currentRow_show = -1; //显示行号的复位
	detectParams->currentRow_extract = -1; //提取行号的复位
	eventCounter = 0; //事件计数器
	ui.graphicsView->centerOn(0, 0); //垂直滑条复位
	qApp->processEvents();
}


/********* 图元矩阵和样本图像矩阵的初始化和删除等操作 ***********/

//初始化图元网格
void TemplateUI::initItemGrid(Ui::ItemGrid &grid)
{
	//基本参数
	int nCamera = detectConfig->nCamera; //相机个数
	int nPhotographing = detectConfig->nPhotographing; //拍摄次数
	QString SampleDirPath = detectConfig->SampleDirPath; //sample文件夹的路径 
	//QSize imageSize = config->imageSize; //原图尺寸

	//计算总间距
	QSize totalSpacing; //总间距
	totalSpacing.setWidth(itemSpacing * (nCamera + 1)); //间距总宽度
	totalSpacing.setHeight(itemSpacing * (nPhotographing + 1)); //间距总高度

	//计算图元尺寸
	QSize viewSize = ui.graphicsView->size(); //视图尺寸
	itemSize.setWidth(int((viewSize.width() - totalSpacing.width()) / nCamera)); //图元宽度
	//qreal itemAspectRatio = qreal(imageSize.width()) / imageSize.height(); //宽高比
	qreal itemAspectRatio = detectConfig->ImageAspectRatio; //宽高比
	itemSize.setHeight(int(itemSize.width() / itemAspectRatio)); //图元高度

	//计算场景尺寸
	sceneSize = totalSpacing;
	sceneSize += QSize(itemSize.width()*nCamera, itemSize.height()*nPhotographing);
	scene.setSceneRect(0, 0, sceneSize.width(), sceneSize.height());

	//生成绘图网点 -- 这里需要修改，不是第一次运行就需要清空itemGrid
	QSize spacingBlock = QSize(itemSpacing, itemSpacing);
	gridSize = itemSize + spacingBlock; //每个网格的尺寸

	//判断itemGrid是否执行过初始化
	if (grid.size() > 0) {
		grid.clear();
	}

	//初始化赋值
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //行
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //列
			QPointF pos(itemSpacing, itemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		grid.append(posList);
	}
}

//初始化图元矩阵中的指针 - ItemArray
void TemplateUI::initPointersInItemArray(Ui::ItemArray &items)
{
	if (items.size() > 0) {
		deletePointersInItemArray(items);//若执行过init函数，则先delete指针
	}
	else {
		items.resize(detectConfig->nPhotographing); //设置大小
		for (int iPhotographing = 0; iPhotographing < detectConfig->nPhotographing; iPhotographing++) { //行
			items[iPhotographing].resize(detectConfig->nCamera);
			for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) { //列
				items[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//删除图元矩阵中的指针 - ItemArray
void TemplateUI::deletePointersInItemArray(Ui::ItemArray &items)
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
void TemplateUI::initPointersInCvMatArray(Ui::CvMatArray &cvmats)
{
	if (cvmats.size() > 0) {
		deletePointersInCvMatArray(cvmats);//若执行过init函数，则先delete指针
	}
	else {
		cvmats.resize(detectConfig->nPhotographing);
		for (int iPhotographing = 0; iPhotographing < detectConfig->nPhotographing; iPhotographing++) { //行
			cvmats[iPhotographing].resize(detectConfig->nCamera);
			for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
				cvmats[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//删除样本图像向量中的指针 - CvMatArray
void TemplateUI::deletePointersInCvMatArray(Ui::CvMatArray &cvmats)
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
void TemplateUI::initPointersInQPixmapArray(Ui::QPixmapArray &qpixmaps)
{
	if (qpixmaps.size() > 0) {
		deletePointersInQPixmapArray(qpixmaps);//若执行过init函数，则先delete指针
	}
	else {
		qpixmaps.resize(detectConfig->nPhotographing);
		for (int iPhotographing = 0; iPhotographing < detectConfig->nPhotographing; iPhotographing++) { //行
			qpixmaps[iPhotographing].resize(detectConfig->nCamera);
			for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
				qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//删除样本图像向量中的指针 - QPixmapArray
void TemplateUI::deletePointersInQPixmapArray(Ui::QPixmapArray &qpixmaps)
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
void TemplateUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);
	}
}


/***************** 按键响应 *****************/

//开始提取新的PCB板
void TemplateUI::on_pushButton_start_clicked()
{
	if (detectParams->currentRow_extract == -1 && !templThread->isRunning()) {
		ui.label_status->setText(QString::fromLocal8Bit("开始运行"));
		ui.pushButton_start->setEnabled(false); //禁用开始按键
		ui.pushButton_return->setEnabled(false); //禁用返回按键

		resetTemplateUI();//重置模板提取子模块
		motionControler->resetControler(1); //运动结构复位
	}
}

//清空
//void TemplateUI::on_pushButton_clear_clicked()
//{
//	resetTemplateUI();
//	ui.label_status->setText(QString::fromLocal8Bit("缓存数据已清空"));
//}

//返回
void TemplateUI::on_pushButton_return_clicked()
{
	resetTemplateUI(); //重置模板提取界面，清空缓存数据
	emit showDetectMainUI(); //发送返回信号
}


/***************** 获取外部信号 ******************/

//暂时使用敲击键盘按键模拟外部信号
void TemplateUI::keyPressEvent(QKeyEvent *event)
{
	detectParams->sampleModelNum = "1"; //型号
	detectParams->sampleBatchNum = "1"; //批次号
	detectParams->sampleNum = "2"; //样本编号

	switch (event->key())
	{
	case Qt::Key_PageUp:
		qDebug() << "===== Key_PageUp";
		if (!templThread->isRunning()) {
			resetTemplateUI();//重置模板提取子模块
			motionControler->resetControler(2); //运动结构复位
		}
	case Qt::Key_PageDown: //换新的PCB板时按这个
		qDebug() << "===== Key_PageDown";
		break;
	case Qt::Key_Up:
		qDebug() << "===== Up";
		break;
	case Qt::Key_Down:
		qDebug() << "===== Down";
		if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1 && !templThread->isRunning())
			resetTemplateUI();//重置模板提取子模块

		//!imgConvertThread.isRunning()
		if (currentRow_show + 1 < detectConfig->nPhotographing && true) { //直接显示新的样本行
			currentRow_show += 1; //更新显示行号
			qDebug() << "currentRow_show  - " << currentRow_show;

			ui.label_status->setText(QString::fromLocal8Bit("正在拍摄第") +
				QString::number(currentRow_show + 1) +
				QString::fromLocal8Bit("行图像"));//更新状态
			qApp->processEvents();

			readSampleImages2(); //读图 - 相当于相机拍照		
		}
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << "===== Enter";
		break;
	default:
		break;
	}
}


//在绘图网格中显示下一行图像
void TemplateUI::nextRowOfSampleImages()
{
	if (currentRow_show + 1 < detectConfig->nPhotographing) { //直接显示新的样本行
		currentRow_show += 1; //更新显示行号
		eventCounter += 1; //更新事件计数器
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		//readSampleImages(); //读图
		double t2 = clock();
		showSampleImages(); //界面显示
		double t3 = clock();

		//qDebug() << "readSampleImages :" << (t2 - t1) << "ms";
		qDebug() << "showSampleImages :" << (t3 - t2) << "ms ( currentRow_show -" << currentRow_show << ")";

		//判断是否执行检测操作
		if (!templThread->isRunning() && eventCounter == 1) {
			extractTemplateImages(); //提取
		}
	}
	else if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1 && !templThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		resetTemplateUI();//重置模板提取子模块
		nextRowOfSampleImages(); //检测新的PCB样本图
	}
}

//在绘图网格中显示下一行图像
void TemplateUI::nextRowOfSampleImages2()
{
	if (currentRow_show + 1 < detectConfig->nPhotographing) { //直接显示新的样本行
		currentRow_show += 1; //更新显示行号
		eventCounter += 1; //更新事件计数器
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		readSampleImages2(); //读图
		double t2 = clock();
		showSampleImages(); //界面显示
		double t3 = clock();

		//qDebug() << "readSampleImages :" << (t2 - t1) << "ms";
		qDebug() << "showSampleImages :" << (t3 - t2) << "ms ( currentRow_show -" << currentRow_show << ")";

		//判断是否执行检测操作
		if (!templThread->isRunning() && eventCounter == 1) {
			extractTemplateImages(); //提取
		}
	}
	else if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1 && !templThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		resetTemplateUI();//重置模板提取子模块
		nextRowOfSampleImages2(); //检测新的PCB样本图
	}
}


/***************** 读图、显示与提取 *****************/

//读取相机组拍摄的一组分图（图像显示网格中的一行）
void TemplateUI::readSampleImages()
{
	//详见：on_takePhotos_finished_camera()
}

//读取相机组拍摄的一组分图 - 直接从硬盘上读图
void TemplateUI::readSampleImages2()
{
	clock_t t1 = clock();

	//获取对应目录的路径
	QString dirpath = detectConfig->SampleDirPath + "/" + detectParams->sampleModelNum + "/"
		+ detectParams->sampleBatchNum + "/" + detectParams->sampleNum;

	//读取目录下的样本图像
	QDir dir(dirpath);
	dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNummberOfSampleImage(); return; }

	if (detectParams->imageSize.width() < 0) { //小于0说明imageSize未初始化
		QImage img = QImage(fileList.at(0).absoluteFilePath());//读图
		detectParams->imageSize = img.size();
		templThread->initTemplFunc();
	}

	//读取并储存图像 - 从硬盘上读图 (用于临时调试)
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1; //0 1 ... nPhotographing-1
		int iCamera = idxs[1].toInt() - 1; //0 1 ... nCamera-1
		if (iPhotographing != currentRow_show) continue;
		if (iPhotographing < 0 || iPhotographing >= detectConfig->nPhotographing) continue;
		if (iCamera < 0 || iCamera >= detectConfig->nCamera) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //样本图的路径
		cv::Mat img = cv::imread(filepath.toStdString(), cv::IMREAD_COLOR);
		cvmatSamples[currentRow_show][iCamera] = new cv::Mat(img);
	}
	
	clock_t t2 = clock();
	qDebug() << "readSampleImages2: " << (t2 - t1) << "ms ( currentRow -" << currentRow_show << ")";

	//图像类型转换
	imgConvertThread.start();
}


//显示相机组拍摄的一组分图（图像显示网格中的一行）
void TemplateUI::showSampleImages()
{
	for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
		QPixmap scaledImg = (*qpixmapSamples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //定义图元
		item->setPos(itemGrid[currentRow_show][iCamera]); //图元的显示位置
		itemArray[currentRow_show][iCamera] = item; //存入图矩阵
	}

	//加载相机组新拍摄的一行图元
	for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//视图显示设置
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //设置垂直滚动条的位置
	ui.graphicsView->show();//显示
}


/******************** 字符识别 ********************/

//通过鼠标双击打开字符识别界面
void TemplateUI::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (currentRow_show == -1) return;//若没有显示对应的样本图则直接返回
	if (event->button() == Qt::RightButton) return;//忽略右键点击

	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	QPoint mousePosition(event->x(), event->y());
	
	//qDebug() << graphicsViewPos.x() << graphicsViewPos.y();
	//qDebug() << graphicsViewSize.width() << graphicsViewSize.height();
	//qDebug() << graphicsViewRect.contains(mousePosition);
	//qDebug() << mousePosition.x() << mousePosition.y();

	//todo 判断鼠标点击的是哪个分图
	if (!graphicsViewRect.contains(mousePosition)) return;
	QPoint relativePos = mousePosition - graphicsViewPos; //相对位置
	int gridColIdx = (int) ceil(relativePos.x() / gridSize.width());//点击位置在第几列
	int gridRowIdx = (int) ceil(relativePos.y() / gridSize.height());//点击位置在第几行
	//qDebug() << gridColIdx << gridRowIdx;
	
	if (true && gridRowIdx <= currentRow_show) {
		serialNumberUI.showSampleImage(gridRowIdx, gridColIdx);
		Ui::delay(3);//延迟
		serialNumberUI.showFullScreen();//显示序号识别界面
		Ui::delay(10);//延迟
		this->hide();
	}

}

//切换分图
void TemplateUI::on_switchImage_serialNumberUI()
{
	this->showFullScreen();
	Ui::delay(10);//延迟
	serialNumberUI.hide();
}

//从序号识别界面获得产品序号之后
void TemplateUI::on_recognizeFinished_serialNumberUI()
{
	//先将产品序号转换为 型号、批次号、样本编号

	//然后调用提取函数 开始提取模板

}


/******************** 运动控制 ********************/

//运动结构前进结束
void TemplateUI::on_moveForwardFinished_motion()
{
	//调用相机进行拍照
	if (currentRow_show + 1 < detectConfig->nPhotographing) {
		currentRow_show += 1; //更新显示行号

		ui.label_status->setText(QString::fromLocal8Bit("正在拍摄第") +
			QString::number(currentRow_show + 1) + 
			QString::fromLocal8Bit("行分图"));//更新状态
		qApp->processEvents();

		cameraControler->start(); //拍照
	}
}

//复位
void TemplateUI::on_resetControlerFinished_motion(int caller)
{
	switch (caller)
	{
	case 1: //TemplateUI::on_pushButton_start_clicked()
		Ui::delay(10);
		motionControler->moveForward(); //运动结构前进
		break;
	default:
		break;
	}
}

/******************** 相机控制 ********************/

//相机初始化结束
void TemplateUI::on_initCamerasFinished_camera(int)
{
}

//相机拍摄结束
void TemplateUI::on_takePhotosFinished_camera(int)
{
	//初始化若干变量
	if (detectParams->imageSize.width() <= 0) {
		cv::Size size = cvmatSamples[0][0]->size();
		detectParams->imageSize = QSize(size.width, size.height);
		templThread->initTemplFunc();
	}

	//调用图像类型转换线程
	imgConvertThread.start();
}


/******************** 图像转换线程 ********************/

//图像转换结束，在界面上显示图像，然后提取模板
void TemplateUI::on_convertFinished_convertThread()
{
	//更新事件计数器
	eventCounter += 1;

	//更新状态栏
	ui.label_status->setText(QString::fromLocal8Bit("第") +
		QString::number(currentRow_show + 1) +
		QString::fromLocal8Bit("行拍摄结束"));
	qApp->processEvents();

	//在界面上显示样本图
	clock_t t1 = clock();
	showSampleImages(); 
	clock_t t2 = clock();
	qDebug() << "showSampleImages: " << (t2 - t1) << "ms ( currentRow =" << currentRow_show << ")";

	//显示结束后之前驱动机械结构运动
	Ui::delay(10); //延迟
	if (currentRow_show + 1 < detectConfig->nPhotographing)
		motionControler->moveForward(); //运动结构前进
	else
		motionControler->resetControler(3); //当前PCB拍完则复位

	//判断是否执行检测操作
	if (eventCounter == 1 && !templThread->isRunning()) {
		extractTemplateImages(); //提取
	}
}


/******************** 模板提取线程 ********************/

//提取线程结束后
//void DetectUI::on_detectFinished_detectThread(bool qualified)
//{
//	//显示结果
//	ui.label_indicator->setPixmap((qualified) ? lightOffIcon : lightOnIcon); //切换指示灯
//	ui.label_result->setText((qualified) ? QString::fromLocal8Bit("合格") : QString::fromLocal8Bit("不合格"));
//}

//提取当前的一行样本图像
void TemplateUI::extractTemplateImages()
{
	//更新提取行号
	detectParams->currentRow_extract += 1;

	//更新状态栏
	ui.label_status->setText(QString::fromLocal8Bit("正在提取第") +
		QString::number(detectParams->currentRow_extract + 1) + 
		QString::fromLocal8Bit("行模板"));
	qApp->processEvents();

	//开启检测线程
	templThread->start();
}

//提取结束后更新状态
void TemplateUI::update_extractState_extractor(int state)
{
	//当提取完一行图像
	if ((TemplateExtractor::ExtractState)state == TemplateExtractor::Finished) { 
		eventCounter -= 1; //更新事件计数器

		//更新状态栏
		ui.label_status->setText(QString::fromLocal8Bit("第") +
			QString::number(detectParams->currentRow_extract + 1) +
			QString::fromLocal8Bit("行提取结束"));
		qApp->processEvents();

		//检查是否有未处理的事件
		while (templThread->isRunning()) Ui::delay(50); //等待线程结束
		if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1) { //当前PCB提取结束
			ui.pushButton_start->setEnabled(true); //启用开始按键
			ui.pushButton_return->setEnabled(true); //启用返回按键
		}
		else { //当前PCB未提取完
			if (eventCounter > 0) extractTemplateImages(); //提取下一行分图
		}
	}
}

