#include "DetectUI.h"
#include <time.h>

using Ui::DetectConfig;
using Ui::DetectParams;


DetectUI::DetectUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//设置检测界面的聚焦策略
	this->setFocusPolicy(Qt::ClickFocus);
	
	//设置系统启动状态
	isStarted = false;

	//加载并显示默认的指示灯图标
	IconFolder = QDir::currentPath() + "/Icons";
	QImage greyIcon = QImage(IconFolder + "/grey.png"); //grey
	QPixmap defaultIcon = QPixmap::fromImage(greyIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio));
	ui.label_indicator->setPixmap(defaultIcon); //加载
	
	//加载其他指示灯图标
	QImage redIcon = QImage(IconFolder + "/red.png"); //red
	lightOnIcon = QPixmap::fromImage(redIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio));
	QImage greenIcon = QImage(IconFolder + "/green.png"); //grey
	lightOffIcon = QPixmap::fromImage(greenIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio));

	//检测线程的信号连接
	detectCore = new DetectCore;
	connect(detectCore, SIGNAL(sig_detectState_detectCore(int)), this, SLOT(update_detectState_detectCore(int)));

	detectThread = new DetectThread;
	detectThread->setDetectCore(detectCore);
}


DetectUI::~DetectUI()
{
	deletePointersInItemArray(); //删除图元矩阵中的指针
	deletePointersInSampleImages(); //删除样本图像向量中的指针
	delete detectThread; //删除检测线程
	delete detectCore; //删除检测核心
}


/******************** 配置 **********************/

void DetectUI::setDetectConfig(DetectConfig *ptr) { config = ptr; }

void DetectUI::setDetectParams(DetectParams *ptr) { params = ptr; }


/****************** 按键响应 *******************/

//检测
void DetectUI::on_pushButton_start_clicked()
{
	isStarted = !isStarted; //状态翻转
	if (isStarted) {
		ui.pushButton_return->setEnabled(false); //禁用返回键
		ui.pushButton_start->setText(QString::fromLocal8Bit("暂停"));
		ui.label_status->setText(QString::fromLocal8Bit("系统已启动"));
	}
	else {
		ui.pushButton_return->setEnabled(true); //启用返回键
		ui.pushButton_start->setText(QString::fromLocal8Bit("开始"));
		ui.label_status->setText(QString::fromLocal8Bit("系统暂停中"));
	}
}

//返回
void DetectUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}


/**************** 图像的加载与显示 *****************/

//对绘图控件GraphicsView的初始化设置
void DetectUI::initGraphicsView()
{
	//基本参数
	itemSpacing = 3; //图元间距
	nCamera = config->nCamera; //相机个数
	nPhotographing = config->nPhotographing; //拍摄次数
	SampleDirPath = config->SampleDirPath; //sample文件夹的路径 
	//QSize imageSize = config->imageSize; //原图尺寸

	//计算总间距
	QSize totalSpacing; //总间距
	totalSpacing.setWidth(itemSpacing * (nCamera + 1)); //间距总宽度
	totalSpacing.setHeight(itemSpacing * (nPhotographing + 1)); //间距总高度

	//计算图元尺寸
	QSize viewSize = ui.graphicsView->size(); //视图尺寸
	itemSize.setWidth(int((viewSize.width() - totalSpacing.width()) / nCamera)); //图元宽度
	//qreal itemAspectRatio = qreal(imageSize.width()) / imageSize.height(); //宽高比
	qreal itemAspectRatio = config->imageAspectRatio; //宽高比
	itemSize.setHeight(int(itemSize.width() / itemAspectRatio)); //图元高度

	//计算场景尺寸
	sceneSize = totalSpacing; 
	sceneSize += QSize(itemSize.width()*nCamera, itemSize.height()*nPhotographing);
	scene.setSceneRect(0, 0, sceneSize.width(), sceneSize.height());

	//生成绘图网点
	QSize spacingBlock = QSize(itemSpacing, itemSpacing);
	QSize gridSize = itemSize + spacingBlock;
	for (int iPhotographing=0; iPhotographing<nPhotographing; iPhotographing++) { //行
		QList<QPointF> posList;
		for (int iCamera=0; iCamera<nCamera; iCamera++) { //列
			QPointF pos(itemSpacing, itemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		itemGrid.append(posList);
	}

	//itemArray的初始化
	itemArray.resize(nPhotographing); //设置大小
	for (int iPhotographing=0; iPhotographing<nPhotographing; iPhotographing++) { //行
		itemArray[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //列
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//sampleImages的初始化
	samples.resize(nPhotographing);
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //行
		samples[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//初始化若干用于监视程序运行状态的变量
	currentRow_show = -1; //显示行号
	params->currentRow_detect = -1; //检测行号
	eventCounter = 0; //事件计数器

	//初始化绘图控件
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //设置聚焦策略
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用水平滚动条
	ui.graphicsView->setScene(&scene); //在视图中添加场景
	ui.graphicsView->centerOn(sceneSize.width()/2, 0); //设置垂直滑动条的位置

	//配置detectThread和detectCore
	detectCore->setDetectConfig(config);
	detectCore->setDetectParams(params);
	detectCore->setDetectResult(&detectResult); //设置存储检测结果的结构体
	detectThread->setSampleImages(&samples);
	detectThread->setDetectParams(params);
}


//读取相机组拍摄的一组分图（图像显示网格中的一行）
void DetectUI::readSampleImages()
{
	double t1 = clock();

	//获取对应目录的路径
	QString dirpath = SampleDirPath + "/" + params->sampleModelNum + "/" + params->sampleBatchNum + "/" + params->sampleNum;

	//读取目录下的样本图像
	QDir dir(dirpath);
	//dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNummberOfSampleImage(); return; }

	if (params->imageSize.height() <= 0) {
		QImage img = QImage(fileList.at(0).absoluteFilePath());//读图
		params->imageSize = img.size();
	}

	//读取并将图像存到itemArray和sampleImages中
	double loadtime = 0;
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1;
		int iCamera = idxs[1].toInt() - 1;
		if (iPhotographing != currentRow_show) continue;

		/*
		int num = fileList.at(i).baseName().toInt(); //图片的编号
		int iCamera = (num - 1) % nCamera; //0 1 ... nCamera-1
		int iPhotographing = (num - 1) / nCamera; //0 1 ... nPhotographing-1
		if (iPhotographing != currentRow) continue;
		*/

		QString filepath = fileList.at(i).absoluteFilePath(); //样本图的路径
		double t11 = clock();
		QImage *img = new QImage(filepath);//读图
		double t22 = clock();
		loadtime += t22 - t11;

		samples[currentRow_show][iCamera] = img;
	}
	double t2 = clock();
	//qDebug() << "readSampleImages : " << (t2 - t1) << "ms   ( loadtime :" << loadtime << "ms )";
	qDebug() << "readSampleImages : " << (t2 - t1) << "ms  ( currentRow_show -" << currentRow_show << ")";
}


//显示相机组拍摄的一组分图（图像显示网格中的一行）
void DetectUI::showSampleImages()
{
	//定义图元
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		QImage imgScaled = (*samples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(imgScaled)); //定义图元
		item->setPos(itemGrid[currentRow_show][iCamera]); //图元的显示位置
		itemArray[currentRow_show][iCamera] = item; //存入图矩阵
	}

	//加载相机组新拍摄的一行图元
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//场景显示
	ui.graphicsView->show(); //显示图像

	//设置垂直滚动条的位置
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height()/2;
	ui.graphicsView->centerOn(sceneSize.width()/2, y_SliderPos);
}


//删除图元矩阵中的指针
void DetectUI::deletePointersInItemArray()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete itemArray[iPhotographing][iCamera];
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//删除样本图像向量中的指针
void DetectUI::deletePointersInSampleImages()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { 
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete samples[iPhotographing][iCamera];
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//移除场景中的所有图元
void DetectUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);  
	}
}


/***************** 获取外部信号 ******************/

//暂时使用敲击键盘按键模拟外部信号
void DetectUI::keyPressEvent(QKeyEvent *event)
{
	params->sampleModelNum = "1"; //型号
	params->sampleBatchNum = "1"; //批次号
	params->sampleNum = "3"; //样本编号

	switch (event->key())
	{
	case Qt::Key_Up:
		qDebug() << "Up";
		break;
	case Qt::Key_Down: //切换并显示上一行样本分图
		qDebug() << "Down";
		if (!isStarted) return; //系统未启动则直接返回
		nextRowOfSampleImages(); 
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << "Enter";
		break;
	default:
		break;
	}
}


//在绘图网格中显示下一行图像
void DetectUI::nextRowOfSampleImages()
{
	if (currentRow_show + 1 < nPhotographing) { //直接显示新的样本行
		currentRow_show += 1; //更新显示行号
		eventCounter += 1; //更新事件计数器
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		readSampleImages(); //读图
		double t2 = clock();
		showSampleImages(); //界面显示
		double t3 = clock();

		//qDebug() << "readSampleImages :" << (t2 - t1) << "ms";
		qDebug() << "showSampleImages :" << (t3 - t2) << "ms ( currentRow_show -" << currentRow_show << ")";

		//判断是否执行检测操作
		if (!detectThread->isRunning() && eventCounter == 1) {
			detectSampleImages(); //执行检测
		}
	}
	else if (params->currentRow_detect == nPhotographing - 1 && !detectThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;
		//params->sampleNum = QString::number(params->sampleNum.toInt() + 1); //编号自增

		removeItemsFromGraphicsScene(); //移除场景中的所有图元
		deletePointersInItemArray(); //删除之前的图元矩阵中的指针
		deletePointersInSampleImages(); //删除样本图像向量中的指针
		ui.graphicsView->centerOn(0, 0); //垂直滑条复位
		currentRow_show = -1; //将显示行号复位
		params->currentRow_detect = -1; //将检测行号复位

		nextRowOfSampleImages(); //检测新的PCB样本图
	}
}


/******************** 检测线程 ********************/

//检测线程结束后
void DetectUI::on_detectFinished_detectThread(bool qualified)
{
	//显示结果
	ui.label_indicator->setPixmap((qualified) ? lightOffIcon : lightOnIcon); //切换指示灯
	ui.label_result->setText((qualified) ? QString::fromLocal8Bit("合格") : QString::fromLocal8Bit("不合格"));
}

//检测当前的一行样本图像
void DetectUI::detectSampleImages()
{
	params->currentRow_detect += 1; //更新检测行号
	qDebug() << "currentRow_detect  - " << params->currentRow_detect;

	ui.label_status->setText(QString::fromLocal8Bit("正在检测第") +
		QString::number(params->currentRow_detect + 1) + QString::fromLocal8Bit("行图像"));//更新状态

	//开启检测线程
	detectThread->start();
}


/***************** 更新状态 ******************/

void DetectUI::update_detectState_detectCore(int state)
{
	if (state == 2) { //检测完一行图像currentRow_detect

		eventCounter -= 1; //更新事件计数器
		ui.label_status->setText(QString::fromLocal8Bit("第") +
			QString::number(params->currentRow_detect + 1) +
			QString::fromLocal8Bit("行检测结束"));//更新状态栏
		qApp->processEvents();

		//检查是否有未处理的事件
		while (detectThread->isRunning()) Ui::delay(100);
		if (eventCounter > 0) detectSampleImages();
	}
}
