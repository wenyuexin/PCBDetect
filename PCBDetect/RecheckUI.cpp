#include "RecheckUI.h"

//using pcb::MessageBoxType;
//using pcb::FlawImageInfo;


//复查界面的构造函数
RecheckUI::RecheckUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//成员变量初始化
	userConfig = Q_NULLPTR; //检修系统配置信息
	runtimeParams = Q_NULLPTR; //运行参数
	detectResult = Q_NULLPTR; //检测结果
	IconFolder = "";

}

//复查界面析构函数
RecheckUI::~RecheckUI()
{
	qDebug() << "~PCBRecheck";
	//delete sysInitThread;
	//delete serialNumberUI;
	//delete exitQueryUI;
}

/********************* 界面初始化 *********************/

void RecheckUI::init()
{
	//选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//加载图标
	IconFolder = QDir::currentPath() + "/icons";
	QPixmap redIcon(IconFolder + "/red.png"); //red
	lightOnIcon = redIcon.scaled(ui.label_indicator1->size(), Qt::KeepAspectRatio);
	QPixmap greyIcon(IconFolder + "/grey.png"); //grey
	lightOffIcon = greyIcon.scaled(ui.label_indicator1->size(), Qt::KeepAspectRatio);

	this->reset();

	//文本框等的背景色
	//QPalette palette;
	//palette.setColor(QPalette::Background, QColor(250, 250, 250));
	//ui.label_flaw->setAutoFillBackground(true); //缺陷图
	//ui.label_flaw->setPalette(palette);
	//ui.label_xLoc->setAutoFillBackground(true); //缺陷位置x
	//ui.label_xLoc->setPalette(palette);
	//ui.label_yLoc->setAutoFillBackground(true); //缺陷位置y
	//ui.label_yLoc->setPalette(palette);
	//ui.label_modelType->setAutoFillBackground(true); //样本编号
	//ui.label_modelType->setPalette(palette);

	//成员变量初始化
	originalFullImageSize = QSize(-1, -1);
	defectNum = -1;
	defectIndex = -1; 

	//graphicsView的设置与图像显示
	ui.graphicsView_full->setFocusPolicy(Qt::NoFocus);
	ui.graphicsView_full->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用水平滚动条
	ui.graphicsView_full->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //禁用垂直滚动条

	//刷新整图上闪烁的箭头
	connect(&flickeringArrow, SIGNAL(refreshArrow_arrow()), this, SLOT(on_refreshArrow_arrow()));
}

void RecheckUI::reset()
{
	//清空静态文本框中的内容
	ui.label_defectIndex->setText("");
	ui.label_defectNum->setText("");
	ui.label_xLoc->setText("");
	ui.label_yLoc->setText("");
	ui.label_modelType->setText("");
	ui.label_detecttionDate->setText("");

	//设置当前正在显示的缺陷小图及其缺陷类型指示灯
	ui.label_indicator1->setPixmap(lightOffIcon); //断路
	ui.label_indicator2->setPixmap(lightOffIcon); //缺失
	ui.label_indicator3->setPixmap(lightOffIcon); //短路
	ui.label_indicator4->setPixmap(lightOffIcon); //凸起
}

/******************* 初始化线程 *******************/

////当用户参数无效
//void PCBRecheck::on_userConfigError_initThread()
//{
//	userConfig.showMessageBox(this); //弹窗提示
//	pcb::delay(10); //延时
//	this->exitRecheckSystem(); //退出系统
//}


//void PCBRecheck::on_outFolderHierarchyError_initThread()
//{
//	QString message = pcb::chinese("无待检修样本，请先进行检测!  ");
//
//	MyMessageBox messageBox;
//	messageBox.set(pcb::MessageBoxType::Information, message);
//	messageBox.doShow();//显示错误信息窗口
//
//	pcb::delay(10); //延时
//	this->exitRecheckSystem(); //退出系统
//}

//系统初始化结束
//void PCBRecheck::on_sysInitFinished_initThread()
//{
//	//将输出目录下的文件夹层次传递给编号设置界面
//	serialNumberUI->setFolderHierarchy(&OutFolderHierarchy);
//
//	//显示PCB序号询问界面
//	this->showSerialNumberUI();
//}


/******************* 退出询问界面 *****************/

//显示退出询问界面
//void PCBRecheck::showExitQueryUI()
//{
//	this->setPushButtonsEnabled(false);
//	pcb::delay(5); //延时
//	exitQueryUI->show(); //弹出退出询问框
//	flickeringArrow.stopFlickering();
//}

//隐藏退出询问界面，并显示编号设置界面
//void PCBRecheck::do_showSerialNumberUI_exitUI()
//{
//	exitQueryUI->hide(); //隐藏退出询问界面
//	pcb::delay(5); //延时
//	
//	if (!serialNumberUI->getNextSerialNum()) { //获取同批次的下一个产品序号
//		recheckStatus = CurrentBatchRechecked; //提示该批次已经检修完
//		this->showMessageBox(MessageBoxType::Information, recheckStatus);
//	}
//	
//	showSerialNumberUI(); //显示编号设置界面
//	pcb::delay(10); //延时
//	this->hide(); //隐藏主界面
//}

//由退出界面返回主界面
//void PCBRecheck::do_showRecheckMainUI_exitUI()
//{
//	exitQueryUI->hide();
//	this->setPushButtonsEnabled(true);
//	flickeringArrow.startFlickering(500);
//}


/********* 初始显示：加载PCB大图、第1个缺陷小图等 ********/

//刷新计时器与小箭头
void RecheckUI::on_refreshArrow_arrow()
{
	flickeringArrow.update(-100, -100, 200, 200);
}

//更新界面上显示的信息
void RecheckUI::refresh()
{
	//logging(runtimeParams.serialNum);

	//更新界面中的PCB型号
	ui.label_modelType->setText(runtimeParams->productID.modelType);

	////检测结果所在的文件夹
	//QString flawImageFolderPath = userConfig.OutputDirPath + "/"
	//	+ runtimeParams.getRelativeFolderPath();
	////判断产品序号对应文件夹是否存在
	//if (!QFileInfo(flawImageFolderPath).isDir()) {
	//	recheckStatus = OpenFlawImageFolderFailed;
	//	this->showMessageBox(MessageBoxType::Warning, recheckStatus);
	//	this->showSerialNumberUI(); //显示PCB序号询问界面
	//	return;
	//}

	//加载并显示PCB大图
	//if (!loadFullImage()) return;
	this->showFullImage();

	////获取文件夹内的图片路径
	//getFlawImageInfo(flawImageFolderPath);

	//加载闪烁的箭头
	defectIndex = 0;
	this->initFlickeringArrow();
	
	//设置场景和显示视图
	ui.graphicsView_full->setScene(&fullImageScene); //设置场景
	ui.graphicsView_full->show(); //显示图像

	//加载并显示第1个缺陷小图
	this->showFlawImage(); //显示缺陷图

	recheckStatus = NoError;
}

//加载PCB整图
//bool RecheckUI::loadFullImage()
//{
//	QString flawImageDirPath = userConfig.OutputDirPath + "/"
//		+ runtimeParams->sampleModelNum + "/"
//		+ runtimeParams.sampleBatchNum + "/"
//		+ runtimeParams.sampleNum; //检测结果所在的文件夹
//
//	QDir dir(flawImageDirPath + "/fullImage/");
//	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//	QStringList filters("*" + userConfig.ImageFormat); //将特定格式的图片过滤出来 
//	dir.setNameFilters(filters);
//	QFileInfoList folder_list = dir.entryInfoList(); //获取文件列表
//
//	QString fullImagePath = "";
//	QStringList pcbInfoList;//整图的原图大小+缺陷总数
//	for (int i = 0; i < folder_list.size(); i++) {
//		if (folder_list.at(i).baseName().startsWith(fullImageNamePrefix)) {
//			fullImagePath = folder_list.at(i).absoluteFilePath();
//
//			//根据文件名获取PCB整图的原始尺寸和缺陷总数
//			pcbInfoList = folder_list.at(i).baseName().split("_");
//			if (pcbInfoList.size() == 4) {
//				originalFullImageSize.setWidth(pcbInfoList[1].toInt());
//				originalFullImageSize.setHeight(pcbInfoList[2].toInt());
//				defectNum = pcbInfoList[3].toInt();
//				break;
//			}
//			else {
//				//logging("InvalidFullImageName: pcbInfoList size(): " + QString::number(pcbInfoList.size()));
//				recheckStatus = InvalidFullImageName;
//				//this->showMessageBox(MessageBoxType::Warning, recheckStatus);
//				//serialNumberUI->show(); //显示PCB序号询问界面
//				return false;
//			}
//		}
//	}
//
//	//判断是否找到fullImage为前缀的ImageFormat格式的图
//	if (fullImagePath == "") {
//		//logging("FullImageNotFound");
//		recheckStatus = FullImageNotFound;
//		//this->showMessageBox(MessageBoxType::Warning, recheckStatus);
//		//serialNumberUI->show(); //显示PCB序号询问界面
//		return false;
//	}
//
//	//加载大图
//	QImage fullImg; //读图
//	if (!fullImg.load(fullImagePath)) {
//		//logging("LoadFullImageFailed: fullImagePath: " + fullImagePath);
//		recheckStatus = LoadFullImageFailed;
//		//this->showMessageBox(MessageBoxType::Warning, recheckStatus);
//		//serialNumberUI->show(); //显示PCB序号询问界面
//		return false;
//	}
//
//	scaledFactor = qMin(qreal(ui.graphicsView_full->height() - 2) / fullImg.size().height(),
//		qreal(ui.graphicsView_full->width() - 2) / fullImg.size().width());//整图的尺寸变换因子
//	fullImg = fullImg.scaled(fullImg.size()*scaledFactor, Qt::KeepAspectRatio); //缩放
//	fullImage = QPixmap::fromImage(fullImg); //转换
//	fullImageItemSize = fullImage.size(); //PCB大图的实际显示尺寸
//	return true;
//}

//加载并显示PCB大图
//从对应的output文件夹的fullImage子文件夹中读取整图
void RecheckUI::showFullImage()
{
	//显示缺陷总数
	if (defectNum >= 0) {
		ui.label_defectNum->setText(QString::number(defectNum));
	}

	//删除场景中之前加载的元素
	QList<QGraphicsItem *> itemList = fullImageScene.items();
	for (int i = 0; i < itemList.size(); i++) {
		fullImageScene.removeItem(itemList[i]);  //从scene中移除
	}

	//将PCB大图加载到场景中
	fullImageScene.addPixmap(fullImage); //将图像加载进场景中
	QRect sceneRect = QRect(QPoint(0, 0), fullImageItemSize); //场景范围
	fullImageScene.setSceneRect(sceneRect); //设置场景范围
}


// 获取当前编号对应的所有缺陷图片的数据缓存到内存中，
// 上下切换缺陷图时，只需从数组中读取数据即可
//void PCBRecheck::getFlawImageInfo(QString dirpath)
//{
//	QDir dir(dirpath);
//	dir.setSorting(QDir::Name | QDir::Time | QDir::Reversed);
//	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//
//	QStringList filters("*" + userConfig.ImageFormat); //将特定格式的图片过滤出来 
//	dir.setNameFilters(filters);
//
//	QFileInfoList folder_list = dir.entryInfoList();
//	flawImageInfoVec.clear();
//	flawImageInfoVec.resize(defectNum);
//	for (int i = 0; i < folder_list.size(); i++) {
//		FlawImageInfo flawImageInfo;
//		flawImageInfo.filePath = folder_list.at(i).absoluteFilePath();
//
//		QStringList list = folder_list.at(i).baseName().split("_");
//		int index = list.at(0).toInt() - 1;
//		if (index >= defectNum) continue;
//		flawImageInfo.flawIndex = QString::number(index); //缺陷编号
//		flawImageInfo.xPos = list.at(1); //x坐标
//		flawImageInfo.yPos = list.at(2); //y坐标
//		flawImageInfo.flawType = list.at(3); //缺陷类型
//		flawImageInfoVec[index] = flawImageInfo;
//	}
//}

//加载初始的闪烁箭头
void RecheckUI::initFlickeringArrow()
{
	if (defectNum <= 0) return;
	flickeringArrow.startFlickering(500); //开始闪烁
	setFlickeringArrowPos(); //更新闪烁箭头的位置
	flickeringArrow.setFullImageSize(&fullImageItemSize);
	fullImageScene.addItem(&flickeringArrow); //将箭头加载进场景中
}

//更新闪烁箭头的位置
void RecheckUI::setFlickeringArrowPos()
{
	//qreal xLoc = flawImageInfoVec[defectIndex].xPos.toDouble();
	//xLoc *= (1.0*fullImageItemSize.width()/originalFullImageSize.width());
	//qreal yLoc = flawImageInfoVec[defectIndex].yPos.toDouble();
	//yLoc *= (1.0*fullImageItemSize.height()/originalFullImageSize.height());
	//flickeringArrow.setPos(xLoc, yLoc); //设置箭头的位置

	qreal xLoc = 0;
	qreal yLoc = 0;
	flickeringArrow.setPos(xLoc, yLoc); //设置箭头的位置
}



/***************** 切换缺陷小图 ****************/

//敲击小键盘的 + -
void RecheckUI::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) 
	{	
	case Qt::Key_Plus: //切换并显示下一个缺陷
		qDebug() << "Key_Plus";
		showNextFlawImage(); 
		break;
	case Qt::Key_Minus: //切换并显示上一个缺陷
		qDebug() << "Key_Minus";
		showLastFlawImage(); 
		break;
	case Qt::Key_Asterisk: //直接显示退出询问界面
		qDebug() << "Key_Asterisk";
		flickeringArrow.stopFlickering();
		//showExitQueryUI();
		break;
	default:
		break;
	}
}

//点击加号按键 - 切换并显示下一个缺陷
void RecheckUI::on_pushButton_plus2_clicked()
{
	qDebug() << "Button_Plus";
	showNextFlawImage();
}

//点击减号按键 - 切换并显示上一个缺陷
void RecheckUI::on_pushButton_minus2_clicked()
{
	qDebug() << "Button_Minus";
	showLastFlawImage();
}


//切换并显示上一个缺陷图
void RecheckUI::showLastFlawImage()
{
	//获取 当前的 index 判断是否 到达边界
	defectIndex -= 1;
	if (defectIndex < 0) {  //边界
		defectIndex += 1;
		qDebug() << "this is the first one";
		//showExitQueryUI(); //显示退出询问框
	}
	else {
		showFlawImage();
	}
}

//切换并显示下一个缺陷图
void RecheckUI::showNextFlawImage()
{
	////获取 当前的 index 判断是否 到达边界
	//defectIndex += 1;
	//if (defectIndex > flawImageInfoVec.size() - 1) {  //边界
	//	defectIndex -= 1;
	//	qDebug() << "this is the last one";
	//	showExitQueryUI(); //显示退出询问框
	//}
	//else {
	//	showFlawImage();
	//}
}

//将缺陷图加载并显示到对应的lebal控件中
void RecheckUI::showFlawImage()
{
	//if (defectNum <= 0) return;
	//QFileInfo flawImgInfo(flawImageInfoVec[defectIndex].filePath);
	//if (!flawImgInfo.isFile()) {
	//	recheckStatus = FlawImageNotFound;
	//	this->showMessageBox(MessageBoxType::Warning, recheckStatus);
	//	return;
	//}

	//QImage flawImg(flawImageInfoVec[defectIndex].filePath); //读图
	//flawImg = flawImg.scaled(ui.label_flaw->size(), Qt::KeepAspectRatio); //缩放
	//QPixmap flawImage(QPixmap::fromImage(flawImg)); //转换
	//ui.label_flaw->clear(); //清空
	//ui.label_flaw->setPixmap(flawImage); //显示图像

	////更新其他缺陷信息
	//ui.label_xLoc->setText(flawImageInfoVec[defectIndex].xPos); //更新缺陷的x坐标
	//ui.label_yLoc->setText(flawImageInfoVec[defectIndex].yPos); //更新缺陷的y坐标
	ui.label_defectIndex->setText(QString::number(defectIndex + 1)); //显示缺陷编号
	this->switchFlawIndicator(); //更新缺陷类型图标(修改指示灯亮灭状态)

	//更新PCB大图上的小箭头的位置
	this->setFlickeringArrowPos();
}

//更新缺陷类型的指示图标
void RecheckUI::switchFlawIndicator()
{
	//flawIndicatorStatus = pow(2, flawImageInfoVec[defectIndex].flawType.toInt() - 1);

	flawIndicatorStatus = 1;
	ui.label_indicator1->setPixmap(bool((flawIndicatorStatus & 0x1) >> 0) ? lightOnIcon : lightOffIcon); //A
	ui.label_indicator2->setPixmap(bool((flawIndicatorStatus & 0x2) >> 1) ? lightOnIcon : lightOffIcon); //B
	ui.label_indicator3->setPixmap(bool((flawIndicatorStatus & 0x4) >> 2) ? lightOnIcon : lightOffIcon); //C
	ui.label_indicator4->setPixmap(bool((flawIndicatorStatus & 0x8) >> 3) ? lightOnIcon : lightOffIcon); //D
}


/**************** 退出程序 *****************/

//点击返回按键
void RecheckUI::on_pushButton_return_clicked()
{
	this->reset(); //清空界面上的检测结果
	emit recheckFinished_recheckUI();
}


/******************** 其他 *********************/

//设置按键
void RecheckUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_plus2->setEnabled(enable);
	ui.pushButton_minus2->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}

//弹窗报错
//void PCBRecheck::showMessageBox(MessageBoxType boxType, RecheckStatus status)
//{
//	RecheckStatus tempStatus = (status == Default) ? recheckStatus : status;
//	if (tempStatus == NoError) return;
//
//	QString message = "";
//	switch (status)
//	{
//	case PCBRecheck::Unchecked:
//		message = pcb::chinese("系统状态未知!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::CurrentBatchRechecked:
//		message = pcb::chinese("该批次的所有样本已经复查完成!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::InvalidFullImageName:
//		message = pcb::chinese("PCB整图文件的文件名无效!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::FullImageNotFound:
//		message = pcb::chinese("没有找到PCB整图!  \n");
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::LoadFullImageFailed:
//		message = pcb::chinese("无法打开PCB整图!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::FlawImageNotFound:
//		message = pcb::chinese("没有找到PCB缺陷图!  \n");
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::LoadFlawImageFailed:
//		message = pcb::chinese("无法打开相应的缺陷图!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::OpenFlawImageFolderFailed:
//		message = pcb::chinese("路径定位失败，无法获取相应的检测结果!\n"); 
//		//message += QString("path: ./output" + runtimeParams.getRelativeFolderPath()) + "\n"; 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::Default:
//		break;
//	}
//
//	//显示窗口
//	//MyMessageBox messageBox;
//	//messageBox.set(boxType, message);
//	//messageBox.doShow();
//	//pcb::delay(10);//延时
//}

//添加日志
void RecheckUI::logging(QString msg)
{
	QString fileName = "./log.txt";
	QFile file(fileName);
	file.open(QIODevice::Append);
	file.close();
	if (file.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		QTextStream stream(&file);
		stream.seek(file.size());

		QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
		stream << time << " " << msg << "\n";
		file.close();
	}
}
