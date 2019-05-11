#include "SerialNumberUI.h"

using pcb::RuntimeParams;


SerialNumberUI::SerialNumberUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//成员变量的初始化
	errorCode = ErrorCode::Default;
	adminConfig = Q_NULLPTR; //系统参数
	runtimeParams = Q_NULLPTR; //运行时的临时参数
	cvmatSamples = Q_NULLPTR; //用于检测的样本图
	qpixmapSamples = Q_NULLPTR; //用于显示的样本图
	imageItem = Q_NULLPTR; 
	gridRowIdx = -1; //当前分图所在行
	gridColIdx = -1; //当前分图所在列
	graphicsScenePos.setX(0);//场景左上角坐标
	graphicsScenePos.setY(0);
	roiRect_tl.setX(0);//roi左上角
	roiRect_tl.setY(0);
	roiRect_br.setX(0);//roi右下角
	roiRect_br.setY(0);
	imageScalingRatio = 0;//图像缩放比例
	ocrHandle = Q_NULLPTR;
}

void SerialNumberUI::init()
{
	//多屏状态下选择在副屏全屏显示
	this->setGeometry(runtimeParams->screenRect);

	//获取绘图控件QGraphicsView的位置
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//界面初始化
	this->initSerialNumberUI();
}

SerialNumberUI::~SerialNumberUI()
{
	qDebug() << "~SerialNumberUI";
	deleteImageItem();
}


/*************** 界面的初始化、设置、更新 **************/

//界面初始化
void SerialNumberUI::initSerialNumberUI()
{
	//限制参数的输入范围
	QIntValidator intValidator;
	ui.lineEdit_roi_tl_x->setValidator(&intValidator);
	ui.lineEdit_roi_tl_y->setValidator(&intValidator);
	ui.lineEdit_roi_br_x->setValidator(&intValidator);
	ui.lineEdit_roi_br_y->setValidator(&intValidator);
	ui.lineEdit_serialNum->setValidator(&intValidator);

	//初始化CheckBox
	this->initCheckBoxGroup();

	//设置检测界面的聚焦策略
	//this->setFocusPolicy(Qt::ClickFocus);

	//鼠标跟踪
	this->setMouseTracking(true);

	//禁用滚动条
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//初始化OCR模块
	ocrHandle = TessBaseAPICreate();
	if (TessBaseAPIInit3(ocrHandle, NULL, "eng") != 0) {
		showMessageBox(this, InitFailed); return;
		//die("Error initialising tesseract\n");
	}
}

//初始化CheckBox 使得同一时刻只有一个box可以选
void SerialNumberUI::initCheckBoxGroup()
{
	QPushButton button;
	checkBoxGroup.addButton(&button);
	checkBoxGroup.addButton(ui.checkBox_roi_tl, 1);
	checkBoxGroup.addButton(ui.checkBox_roi_br, 2);
}

//切换界面控件的状态
void SerialNumberUI::setSerialNumberUIEnabled(bool enable)
{
	ui.lineEdit_roi_tl_x->setEnabled(enable);
	ui.lineEdit_roi_tl_y->setEnabled(enable);
	ui.lineEdit_roi_br_x->setEnabled(enable);
	ui.lineEdit_roi_br_y->setEnabled(enable);

	ui.pushButton_getROI->setEnabled(enable);
	ui.pushButton_recognize->setEnabled(enable);

	ui.lineEdit_serialNum->setEnabled(enable);
	ui.pushButton_confirm->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}

//重置序号识别界面
void SerialNumberUI::resetSerialNumberUI()
{
	ui.checkBox_roi_tl->setChecked(true);
	ui.checkBox_roi_tl->setChecked(false);
	ui.checkBox_roi_br->setChecked(false);
	ui.lineEdit_serialNum->setText("");

	//删除图元
	deleteImageItem();

	//重置产品序号等
	runtimeParams->resetSerialNum();

	//删除buffer文件夹中的roi图片
	QFile file(roiFilePath);
	if (file.exists()) QFile::remove(roiFilePath);
}

/******************* 按键响应 *******************/

//获取包含产品序号的ROI区域
void SerialNumberUI::on_pushButton_getROI_clicked()
{
	ui.checkBox_roi_tl->setChecked(false);
	ui.checkBox_roi_br->setChecked(false);
	this->setSerialNumberUIEnabled(false);

	//计算区域范围
	cv::Point tl(roiRect_tl.x(), roiRect_tl.y());
	cv::Point br(roiRect_br.x(), roiRect_br.y());
	cv::Rect roiRect = cv::Rect(tl, br);
	if (roiRect.width >= adminConfig->ImageSize_W || 
		roiRect.height >= adminConfig->ImageSize_H) 
	{
		showMessageBox(this, Invalid_RoiRect); return;
	}

	//保存区域图片
	cv::Mat roiImg = (*(*cvmatSamples)[gridRowIdx][gridColIdx])(roiRect);
	roiFilePath = runtimeParams->BufferDirPath + "/serialNumRoi.bmp";
	cv::imwrite(roiFilePath.toStdString(), roiImg);

	this->setSerialNumberUIEnabled(true);
}

//识别产品序号 - 此处调用OCR模块
void SerialNumberUI::on_pushButton_recognize_clicked()
{
	this->setSerialNumberUIEnabled(false);
	PIX *img = pixRead(roiFilePath.toStdString().c_str());
	char *text = NULL;

	//加载图像
	TessBaseAPISetImage2(ocrHandle, img);
	if (TessBaseAPIRecognize(ocrHandle, NULL) != 0) {
		showMessageBox(this, Invalid_RoiData); return;
		//die("Error in Tesseract recognition\n");
	}

	//识别产品序号
	if ((text = TessBaseAPIGetUTF8Text(ocrHandle)) == NULL) {
		showMessageBox(this, RecognizeFailed); return;
		//die("Error getting text\n");
	}

	QString serialNum = QString(text);
	serialNum = serialNum.remove(QRegExp("\\s")); //删除空白字符
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//删除首尾的非数字字符
	ui.lineEdit_serialNum->setText(serialNum); //显示识别的产品序号
	this->setSerialNumberUIEnabled(true);

	//检查产品序号是否有效
	runtimeParams->serialNum = serialNum;
	runtimeParams->resetErrorCode(RuntimeParams::Index_serialNum);//重置错误代码
	RuntimeParams::ErrorCode code = RuntimeParams::Unchecked;
	code = runtimeParams->checkValidity(RuntimeParams::Index_serialNum);
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); return;
	}
}

//确定按键
void SerialNumberUI::on_pushButton_confirm_clicked()
{
	QString serialNum = ui.lineEdit_serialNum->text(); //读取产品序号
	serialNum = serialNum.remove(QRegExp("\\s")); //删除空白字符
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//删除首尾的非数字字符
	runtimeParams->serialNum = serialNum;
	runtimeParams->resetErrorCode(RuntimeParams::Index_serialNum);//重置错误代码

	RuntimeParams::ErrorCode code = RuntimeParams::Unchecked;
	code = runtimeParams->parseSerialNum(); //解析产品序号
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); return;
	}
	//检查型号、批次号、编号是否有效
	code = runtimeParams->checkValidity(RuntimeParams::Index_All_SerialNum);
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); return;
	}

	//返回上一级界面，并执行下一步处理
	emit recognizeFinished_serialNumUI();
}

//返回
void SerialNumberUI::on_pushButton_return_clicked()
{
	emit showPreviousUI_serialNumUI();
}


/**************** 显示分图与鼠标框选 ****************/

//在graphicView中显示分图
void SerialNumberUI::showSampleImage(int row, int col)
{
	gridRowIdx = row; //当前分图所在行
	gridColIdx = col; //当前分图所在列

	QSize graphicsViewSize = graphicsViewRect.size();
	QPixmap *img = (*qpixmapSamples)[gridRowIdx][gridColIdx];
	QPixmap scaledImg = img->scaled(graphicsViewSize, Qt::KeepAspectRatio);
	imageScalingRatio = 1.0 * scaledImg.width() / img->width();

	//在场景中加载新的图元
	deleteImageItem(); //删除之前的图元
	imageItem = new QGraphicsPixmapItem(scaledImg); //定义新图元
	graphicsScene.addItem(imageItem);

	//计算场景左上角点的坐标
	QPointF scenePosToView = -(ui.graphicsView->mapToScene(0,0));
	graphicsScenePos = scenePosToView - QPointF(scaledImg.width()/2.0, scaledImg.height()/2.0);

	ui.graphicsView->show();//显示
}

//删除graphicsView中显示的的图像
void SerialNumberUI::deleteImageItem()
{
	//回收内存
	delete imageItem;
	imageItem = Q_NULLPTR;

	//从场景中移除
	QList<QGraphicsItem *> itemList = graphicsScene.items();
	for (int i = 0; i < itemList.size(); i++) {
		graphicsScene.removeItem(itemList[i]);
	}

	//重置场景相对于视图的位置
	graphicsScenePos.setX(0);
	graphicsScenePos.setY(0);
}

//鼠标按下事件
void SerialNumberUI::mousePressEvent(QMouseEvent *event)
{
	//鼠标左键单击获取左上角点
	if (event->button() == Qt::LeftButton && ui.checkBox_roi_tl->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //区域判断
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
		relativePos -= graphicsScenePos; //相对于场景的坐标
		roiRect_tl = relativePos / imageScalingRatio; //坐标转换
		ui.lineEdit_roi_tl_x->setText(QString::number((int)roiRect_tl.x()));//更新界面
		ui.lineEdit_roi_tl_y->setText(QString::number((int)roiRect_tl.y()));
		return;
	}
	
	//鼠标左键单击获取右下角点
	if(ui.checkBox_roi_br->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //区域判断
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
		relativePos -= graphicsScenePos; //相对于场景的坐标
		roiRect_br = relativePos / imageScalingRatio; //坐标转换
		ui.lineEdit_roi_br_x->setText(QString::number((int)roiRect_br.x()));//更新界面
		ui.lineEdit_roi_br_y->setText(QString::number((int)roiRect_br.y()));
		return;
	}

	//通过拖矩形框的方式选择区域
	if (event->button() == Qt::LeftButton && captureStatus == InitCapture) {
		captureStatus = BeginCapture;
		mousePressPos = event->pos();

		qDebug() << mousePressPos.x() << mousePressPos.y();
	}
	else if (event->button() == Qt::LeftButton && graphicsViewRect.contains(event->pos())) {
		captureStatus = BeginMove;
		setCursor(Qt::SizeAllCursor);
		mouseReleasePos = event->pos();
	}
}

//鼠标移动事件 - 暂时没用
void SerialNumberUI::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton && mousePress) {
		if (!this->mouseRelease) {
			this->mouseReleasePos = event->pos();//更新鼠标释放的位置
			//paint();

			qDebug() << mouseReleasePos.x() << mouseReleasePos.y();
		}
	}
}

//鼠标释放事件 - 暂时没用
void SerialNumberUI::mouseReleaseEvent(QMouseEvent *event)
{
	Qt::MouseButtons b = event->buttons();
	if (event->buttons() == Qt::LeftButton) {
		if (this->mouseRelease) {
			this->mouseReleasePos = event->pos();
			this->mousePress = false;
			this->mouseRelease = true;

			qDebug() << mouseReleasePos.x() << mouseReleasePos.y();
			//paint();
		}
	}
}

// 暂时没用
bool SerialNumberUI::isPressPosInGraphicViewRect(QPoint mousePressPos)
{
	QRect selectRect = getRect(mousePressPos, mouseReleasePos);
	return selectRect.contains(mousePressPos);
}

// 根据beginPoint , endPoint 获取当前选中的矩形 - 暂时没用
QRect SerialNumberUI::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
	int x, y, width, height;
	width = qAbs(beginPoint.x() - endPoint.x());
	height = qAbs(beginPoint.y() - endPoint.y());
	x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
	y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

	// 避免宽或高为零时拷贝截图有误;
	// 当选取截图宽或高为零时默认为2;
	QRect selectedRect = QRect(x, y, width, height);
	if (selectedRect.width() == 0) selectedRect.setWidth(1);
	if (selectedRect.height() == 0) selectedRect.setHeight(1);

	return selectedRect;
}


/********************* 其他 ********************/

//弹窗警告
bool SerialNumberUI::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == SerialNumberUI::NoError) return false;

	QString valueName;
	switch (tempCode)
	{
	case SerialNumberUI::Unchecked:
		valueName = pcb::chinese("\"状态未验证\""); break;
	case SerialNumberUI::InitFailed:
		valueName = pcb::chinese("\"初始化失败\""); break;
	case SerialNumberUI::Invalid_RoiRect:
		valueName = pcb::chinese("\"序号区域\""); break;
	case SerialNumberUI::Invalid_RoiData:
		valueName = pcb::chinese("\"序号区域数据丢失\""); break;
	case SerialNumberUI::RecognizeFailed:
		valueName = pcb::chinese("\"序号识别失败\""); break;
	case SerialNumberUI::Invalid_SerialNum:
		valueName = pcb::chinese("\"产品序号\""); break;
	case SerialNumberUI::Default:
		valueName = pcb::chinese("\"未知错误\""); break;
	default:
		break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("产品序号识别错误！ \n") +
		pcb::chinese("错误来源：") + valueName + "!        \n" +
		"SerialNumber: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return false;
}