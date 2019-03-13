#include "TemplateUI.h"

using Ui::DetectConfig;
using Ui::DetectParams;


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

	//模板提取线程
	templExtractor = new TemplateExtractor;
	connect(templExtractor, SIGNAL(sig_extractState_extractor(int)), this, SLOT(update_extractState_extractor(int)));

	templThread = new TemplateThread();
	templThread->setTemplateExtractor(templExtractor);
}

TemplateUI::~TemplateUI()
{
	deletePointersInItemArray(); //删除图元矩阵中的指针
	deletePointersInSampleImages(); //移除场景中已经加载的图元
	delete templThread;
	delete templExtractor;
}


/****************** 配置 *********************/

//设置参数结构体的指针
void TemplateUI::setDetectConfig(DetectConfig *ptr) { config = ptr; }

void TemplateUI::setDetectParams(DetectParams *ptr) { params = ptr; }


/***************** 按键响应 *****************/

//提取
void TemplateUI::on_pushButton_getTempl_clicked()
{
	ui.label_status->setText(QString::fromLocal8Bit("正在提取模板"));

	//将分图保存到template文件夹中
	// ...
	Ui::delay(1000);

	ui.label_status->setText(QString::fromLocal8Bit("模板提取结束"));
}

//清空
void TemplateUI::on_pushButton_clear_clicked()
{
	removeItemsFromGraphicsScene(); //移除场景中已经加载的图元
	deletePointersInItemArray();//删除图元矩阵中的指针
	currentRow_show = -1; //显示行号的复位
	params->currentRow_extract = -1; //提取行号的复位
	ui.graphicsView->centerOn(0, 0); //垂直滑条复位
	ui.label_status->setText(QString::fromLocal8Bit("缓存数据已清空"));
}

//返回
void TemplateUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}


/****************** 图像显示 *******************/

//对绘图控件GraphicsView的初始化设置
void TemplateUI::initGraphicsView()
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
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //行
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //列
			QPointF pos(itemSpacing, itemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		itemGrid.append(posList);
	}

	//itemArray的初始化
	itemArray.resize(nPhotographing); //设置大小
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //行
		itemArray[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //列
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//sampleImages的初始化
	samples.resize(nPhotographing);
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		samples[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//初始化若干用于监视程序运行状态的变量
	currentRow_show = -1; //显示行号
	params->currentRow_extract = -1; //检测行号
	eventCounter = 0; //事件计数器

	//配置提取线程
	templExtractor->setDetectParams(params);
	templExtractor->setDetectConfig(config);
	templThread->setDetectParams(params);
	templThread->setDetectConfig(config);
	templThread->setSampleImages(&samples);

	//视图控件的设置
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //设置聚焦策略
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用水平滚动条
	ui.graphicsView->setScene(&scene); //在视图中添加场景
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //设置垂直滑动条的位置
}


//读取相机组拍摄的一组分图（图像显示网格中的一行）
void TemplateUI::readSampleImages()
{
	//获取对应目录的路径
	QString dirpath = SampleDirPath + "/" + params->sampleModelNum + "/" + params->sampleBatchNum + "/" + params->sampleNum;

	//读取目录下的样本图像
	QDir dir(dirpath);
	dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNummberOfSampleImage(); return; }

	if (params->imageSize.width() < 0) {
		QImage img = QImage(fileList.at(0).absoluteFilePath());//读图
		params->imageSize = img.size();
		templThread->initTemplFunc();
	}

	//读取并储存图像
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1; //0 1 ... nPhotographing-1
		int iCamera = idxs[1].toInt() - 1; //0 1 ... nCamera-1
		if (iPhotographing != currentRow_show) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //样本图的路径
		QImage *img = new QImage(filepath); //读图
		samples[currentRow_show][iCamera] = img;
	}
}


//显示相机组拍摄的一组分图（图像显示网格中的一行）
void TemplateUI::showSampleImages()
{
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		QImage scaledImg = (*samples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(scaledImg)); //定义图元
		item->setPos(itemGrid[currentRow_show][iCamera]); //图元的显示位置
		itemArray[currentRow_show][iCamera] = item; //存入图矩阵
	}

	//加载相机组新拍摄的一行图元
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//视图显示设置
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //设置垂直滚动条的位置
	ui.graphicsView->show();//显示
}


//删除图元矩阵中的指针
void TemplateUI::deletePointersInItemArray()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete itemArray[iPhotographing][iCamera];
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//删除样本图像向量中的指针
void TemplateUI::deletePointersInSampleImages()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete samples[iPhotographing][iCamera];
			samples[iPhotographing][iCamera] = Q_NULLPTR;
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

/***************** 获取外部信号 ******************/

//暂时使用敲击键盘按键模拟外部信号
void TemplateUI::keyPressEvent(QKeyEvent *event)
{
	params->sampleModelNum = "1"; //型号
	params->sampleBatchNum = "1"; //批次号
	params->sampleNum = "2"; //样本编号

	switch (event->key())
	{
	case Qt::Key_Up:
		qDebug() << "Up";
		break;
	case Qt::Key_Down: //切换并显示上一行样本分图
		qDebug() << "Down";
		//if (!isStarted) return; //系统未启动则直接返回
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
void TemplateUI::nextRowOfSampleImages()
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
		if (!templThread->isRunning() && eventCounter == 1) {
			extractTemplateImages(); //提取
		}
	}
	else if (params->currentRow_extract == nPhotographing - 1 && !templThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		removeItemsFromGraphicsScene(); //移除场景中的所有图元
		deletePointersInItemArray(); //删除之前的图元矩阵中的指针
		deletePointersInSampleImages(); //删除样本图像向量中的指针
		ui.graphicsView->centerOn(0, 0); //垂直滑条复位
		currentRow_show = -1; //将显示行号复位
		params->currentRow_extract = -1; //将检测行号复位

		nextRowOfSampleImages(); //检测新的PCB样本图
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


//当前的一行样本图像
void TemplateUI::extractTemplateImages()
{
	params->currentRow_extract += 1;

	ui.label_status->setText(QString::fromLocal8Bit("正在提取第") +
		QString::number(params->currentRow_extract + 1) + QString::fromLocal8Bit("行模板"));//更新状态

	//开启检测线程
	templThread->start();
}

void TemplateUI::update_extractState_extractor(int state)
{
	if (state == 1) { //提取完一行图像

		eventCounter -= 1; //更新事件计数器
		ui.label_status->setText(QString::fromLocal8Bit("第") +
			QString::number(params->currentRow_extract + 1) +
			QString::fromLocal8Bit("行提取结束"));//更新状态栏
		qApp->processEvents();

		//检查是否有未处理的事件
		while (templThread->isRunning()) Ui::delay(100);
		if (eventCounter > 0) extractTemplateImages();
	}
}
