#include "SerialNumberUI.h"

using pcb::RuntimeParams;


SerialNumberUI::SerialNumberUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//成员变量的初始化
	maskRoiWidgetsIsVisible = true;
	NumberValidator = Q_NULLPTR;
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
	ocrRoi_tl.setX(-1);//roi左上角
	ocrRoi_tl.setY(-1);
	ocrRoi_br.setX(-1);//roi右下角
	ocrRoi_br.setY(-1);
	imageScalingRatio = 0;//图像缩放比例
	ocrHandle = Q_NULLPTR;
}

void SerialNumberUI::init()
{
	//多屏状态下选择在副屏全屏显示
	this->setGeometry(runtimeParams->ScreenRect);

	//设置掩膜区域相关控件的可见性与其他控件的相对位置
	this->initMaskRoiWidgets();

	//获取绘图控件QGraphicsView的位置
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//限制参数的输入范围
	delete NumberValidator;
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
	ui.lineEdit_maskRoi_tl_x->setValidator(NumberValidator); //掩膜
	ui.lineEdit_maskRoi_tl_y->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_x->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_y->setValidator(NumberValidator);
	ui.lineEdit_ocrRoi_tl_x->setValidator(NumberValidator); //OCR
	ui.lineEdit_ocrRoi_tl_y->setValidator(NumberValidator);
	ui.lineEdit_ocrRoi_br_x->setValidator(NumberValidator);
	ui.lineEdit_ocrRoi_br_y->setValidator(NumberValidator);
	ui.lineEdit_serialNum->setValidator(NumberValidator); //序号

	//初始化CheckBox
	this->initCheckBoxGroup();

	//设置检测界面的聚焦策略
	//this->setFocusPolicy(Qt::ClickFocus);

	//设置鼠标
	this->setMouseTracking(true);
	this->setCursor(Qt::ArrowCursor);

	//禁用滚动条
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//初始化OCR模块
	if (ocrHandle == Q_NULLPTR) { //防止多次初始化
		ocrHandle = TessBaseAPICreate(); //初始化
		if (TessBaseAPIInit3(ocrHandle, NULL, "eng") != 0) {
			showMessageBox(this, InitFailed); return;
			//die("Error initialising tesseract\n");
		}
	}

	//设置掩膜区域的坐标初始值
	runtimeParams->maskRoi_tl.setX(-1);
	runtimeParams->maskRoi_tl.setY(-1);
	runtimeParams->maskRoi_br.setX(-1);
	runtimeParams->maskRoi_br.setY(-1);
}

SerialNumberUI::~SerialNumberUI()
{
	qDebug() << "~SerialNumberUI";
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
	deleteImageItem();
}



/*************** 界面的初始化、设置、更新 **************/

//设置掩膜区域相关控件的可见性与其他控件的相对位置
void SerialNumberUI::initMaskRoiWidgets()
{
	if (maskRoiWidgetsIsVisible) return;

	//将掩膜区域坐标相关的控件设为不可见
	ui.label_maskRoi_title->setVisible(false);
	ui.checkBox_maskRoi_tl->setVisible(false);
	ui.label_maskRoi_tl->setVisible(false);
	ui.label_maskRoi_tl_x->setVisible(false);
	ui.label_maskRoi_tl_y->setVisible(false);
	ui.lineEdit_maskRoi_tl_x->setVisible(false);
	ui.lineEdit_maskRoi_tl_y->setVisible(false);

	ui.checkBox_maskRoi_br->setVisible(false);
	ui.label_maskRoi_br->setVisible(false);
	ui.label_maskRoi_br_x->setVisible(false);
	ui.label_maskRoi_br_y->setVisible(false);
	ui.lineEdit_maskRoi_br_x->setVisible(false);
	ui.lineEdit_maskRoi_br_y->setVisible(false);
	ui.pushButton_getMaskRoi->setVisible(false);
	ui.line_1->setVisible(false);

	//将右侧剩余的控件上移
	int distanceOfMovingUp = (this->size().height() > 800)? 210 : 100;
	QPoint direction = QPoint(0, -distanceOfMovingUp);

	ui.label_ocrRoi_title->move(ui.label_ocrRoi_title->pos() + direction);
	ui.checkBox_ocrRoi_tl->move(ui.checkBox_ocrRoi_tl->pos() + direction);
	ui.label_ocrRoi_tl->move(ui.label_ocrRoi_tl->pos() + direction);
	ui.label_ocrRoi_tl_x->move(ui.label_ocrRoi_tl_x->pos() + direction);
	ui.label_ocrRoi_tl_y->move(ui.label_ocrRoi_tl_y->pos() + direction);
	ui.lineEdit_ocrRoi_tl_x->move(ui.lineEdit_ocrRoi_tl_x->pos() + direction);
	ui.lineEdit_ocrRoi_tl_y->move(ui.lineEdit_ocrRoi_tl_y->pos() + direction);

	ui.checkBox_ocrRoi_br->move(ui.checkBox_ocrRoi_br->pos() + direction);
	ui.label_ocrRoi_br->move(ui.label_ocrRoi_br->pos() + direction);
	ui.label_ocrRoi_br_x->move(ui.label_ocrRoi_br_x->pos() + direction);
	ui.label_ocrRoi_br_y->move(ui.label_ocrRoi_br_y->pos() + direction);
	ui.lineEdit_ocrRoi_br_x->move(ui.lineEdit_ocrRoi_br_x->pos() + direction);
	ui.lineEdit_ocrRoi_br_y->move(ui.lineEdit_ocrRoi_br_y->pos() + direction);

	ui.pushButton_getOcrRoi->move(ui.pushButton_getOcrRoi->pos() + direction);
	ui.pushButton_recognize->move(ui.pushButton_recognize->pos() + direction);

	ui.line_2->move(ui.line_2->pos() + direction);
	ui.label_serialNum_title->move(ui.label_serialNum_title->pos() + direction);
	ui.lineEdit_serialNum->move(ui.lineEdit_serialNum->pos() + direction);
	ui.pushButton_confirm->move(ui.pushButton_confirm->pos() + direction);

	ui.line_3->move(ui.line_3->pos() + direction);
	ui.pushButton_return->move(ui.pushButton_return->pos() + direction);
}

//初始化CheckBox 使得同一时刻只有一个box可以选
void SerialNumberUI::initCheckBoxGroup()
{
	QPushButton button;
	checkBoxGroup.addButton(&button);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_tl, 1);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_br, 2);
	checkBoxGroup.addButton(ui.checkBox_ocrRoi_tl, 3);
	checkBoxGroup.addButton(ui.checkBox_ocrRoi_br, 4);
}

//切换界面控件的状态
void SerialNumberUI::setSerialNumberUIEnabled(bool enable)
{
	ui.lineEdit_maskRoi_tl_x->setEnabled(enable);
	ui.lineEdit_maskRoi_tl_y->setEnabled(enable);
	ui.lineEdit_maskRoi_br_x->setEnabled(enable);
	ui.lineEdit_maskRoi_br_y->setEnabled(enable);

	ui.lineEdit_ocrRoi_tl_x->setEnabled(enable);
	ui.lineEdit_ocrRoi_tl_y->setEnabled(enable);
	ui.lineEdit_ocrRoi_br_x->setEnabled(enable);
	ui.lineEdit_ocrRoi_br_y->setEnabled(enable);
	ui.lineEdit_serialNum->setEnabled(enable); //序号输入框
	this->setPushButtonsEnabled(enable); //按键设置
}

//按键设置
void SerialNumberUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_getMaskRoi->setEnabled(enable); //确认Mask区域
	ui.pushButton_getOcrRoi->setEnabled(enable); //确认COR区域
	ui.pushButton_recognize->setEnabled(enable); //识别
	ui.pushButton_confirm->setEnabled(enable); //确定并返回
	ui.pushButton_return->setEnabled(enable); //返回
}

//重置序号识别界面
void SerialNumberUI::reset()
{
	if (this == Q_NULLPTR) return;

	this->setCursor(Qt::ArrowCursor); //设置鼠标

	ui.checkBox_maskRoi_tl->setChecked(false);
	ui.checkBox_maskRoi_br->setChecked(false);
	ui.checkBox_ocrRoi_tl->setChecked(false);
	ui.checkBox_ocrRoi_br->setChecked(false);
	//ui.lineEdit_serialNum->setText("");

	//清空掩膜区域坐标
	ui.lineEdit_maskRoi_tl_x->setText("");
	ui.lineEdit_maskRoi_tl_y->setText("");
	ui.lineEdit_maskRoi_br_x->setText("");
	ui.lineEdit_maskRoi_br_y->setText("");

	//删除图元
	deleteImageItem();

	//重置产品序号等
	runtimeParams->resetSerialNum();

	//删除buffer文件夹中的roi图片
	QFile file(roiFilePath);
	if (file.exists()) QFile::remove(roiFilePath);
}



/*************** 获取掩膜区域的坐标 **************/

void SerialNumberUI::on_pushButton_getMaskRoi_clicked()
{
	this->setCursor(Qt::ArrowCursor); //设置鼠标样式
	if (runtimeParams->DeveloperMode) return;

	//ui.checkBox_maskRoi_tl->setChecked(false);
	//ui.checkBox_maskRoi_br->setChecked(false);
	this->setSerialNumberUIEnabled(false);

	int ImageSize_W = adminConfig->ImageSize_W;
	int ImageSize_H = adminConfig->ImageSize_H;
	int tl_x = 0, tl_y = 0, br_x = 0, br_y = 0;

	//获取掩膜区域的坐标 - 左上角
	QString str_tl_x = ui.lineEdit_maskRoi_tl_x->text();
	if (str_tl_x == "") tl_x = -1;
	else tl_x = (int)intervalCensored(str_tl_x.toInt(), 0, ImageSize_W - 1);

	QString str_tl_y = ui.lineEdit_maskRoi_tl_y->text();
	if (str_tl_y == "") tl_y = -1;
	else tl_y = (int)intervalCensored(str_tl_y.toInt(), 0, ImageSize_H - 1);

	//获取掩膜区域的坐标 - 右下角
	QString str_br_x = ui.lineEdit_maskRoi_br_x->text();
	if (str_br_x == "") br_x = -1;
	else br_x = (int)intervalCensored(str_br_x.toInt(), 0, ImageSize_W - 1);

	QString str_br_y = ui.lineEdit_maskRoi_br_y->text();
	if (str_br_y == "") br_y = -1;
	else br_y = (int)intervalCensored(str_br_y.toInt(), 0, ImageSize_H - 1);

	//将区域坐标存入运行参数类
	runtimeParams->maskRoi_tl.setX(tl_x);
	runtimeParams->maskRoi_tl.setY(tl_y);
	runtimeParams->maskRoi_br.setX(br_x);
	runtimeParams->maskRoi_br.setY(br_y);

	emit getMaskRoiFinished_serialNumUI();

	this->setSerialNumberUIEnabled(true);
}



/******************* 按键响应 *******************/

//获取包含产品序号的ROI区域
void SerialNumberUI::on_pushButton_getOcrRoi_clicked()
{
	if (runtimeParams->DeveloperMode) return;

	ui.checkBox_ocrRoi_tl->setChecked(false);
	ui.checkBox_ocrRoi_br->setChecked(false);
	this->setSerialNumberUIEnabled(false);

	//计算区域范围
	cv::Point tl(ocrRoi_tl.x(), ocrRoi_tl.y());
	cv::Point br(ocrRoi_br.x(), ocrRoi_br.y());
	cv::Rect roiRect = cv::Rect(tl, br);
	if (roiRect.width >= adminConfig->ImageSize_W || 
		roiRect.height >= adminConfig->ImageSize_H) 
	{
		showMessageBox(this, Invalid_RoiRect); 
		this->setSerialNumberUIEnabled(true); return;
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
	if (runtimeParams->DeveloperMode) return;

	//禁用部分控件
	this->setSerialNumberUIEnabled(false);
	
	//加载图像
	PIX *img = pixRead(roiFilePath.toStdString().c_str());
	char *text = NULL;

	//字符识别
	TessBaseAPISetImage2(ocrHandle, img);
	if (TessBaseAPIRecognize(ocrHandle, NULL) != 0) {
		showMessageBox(this, Invalid_RoiData); 
		this->setSerialNumberUIEnabled(true); return;
	}

	//序号转换
	if ((text = TessBaseAPIGetUTF8Text(ocrHandle)) == NULL) {
		showMessageBox(this, RecognizeFailed); 
		this->setSerialNumberUIEnabled(true); return;
	}

	//序号预处理
	QString serialNum = QString(text);
	serialNum = serialNum.remove(QRegExp("\\s")); //删除空白字符
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//删除首尾的非数字字符
	ui.lineEdit_serialNum->setText(serialNum); //显示识别的产品序号
	this->setSerialNumberUIEnabled(true); //启用部分控件

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
	this->setPushButtonsEnabled(false); //禁用按键

	QString serialNum = ui.lineEdit_serialNum->text(); //读取产品序号
	serialNum = serialNum.remove(QRegExp("\\s")); //删除空白字符
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//删除首尾的非数字字符
	runtimeParams->serialNum = serialNum;
	runtimeParams->resetErrorCode(RuntimeParams::Index_serialNum);//重置错误代码

	RuntimeParams::ErrorCode code = RuntimeParams::Unchecked;
	code = runtimeParams->parseSerialNum(); //解析产品序号
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); 
		this->setPushButtonsEnabled(true); return;
	}
	//检查型号、批次号、编号是否有效
	code = runtimeParams->checkValidity(RuntimeParams::Index_All_SerialNum);
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); 
		this->setPushButtonsEnabled(true); return;
	}

	//返回上一级界面，并执行下一步处理
	emit on_pushButton_return_clicked();

	this->setPushButtonsEnabled(true); //开启按键

	//向上一级界面发送识别结束的信号
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
	if (runtimeParams->DeveloperMode) return;

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
	pcb::delay(10);

	//计算场景左上角点的坐标
	//QPointF scenePosToView = -(ui.graphicsView->mapToScene(0,0));
	QPointF scenePosToView(graphicsViewSize.width() / 2.0, graphicsViewSize.height() / 2.0);
	graphicsScenePos = scenePosToView - QPointF(scaledImg.width(), scaledImg.height()) / 2.0;

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
	if (runtimeParams->DeveloperMode) return;

	//鼠标左键单击获取左上角点 - 掩膜
	if (event->button() == Qt::LeftButton && ui.checkBox_maskRoi_tl->isChecked()) {
		if (!graphicsViewRect.contains(event->pos())) return; //区域判断
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
		relativePos -= graphicsScenePos; //相对于场景的坐标
		maskRoi_tl = relativePos / imageScalingRatio; //坐标转换
		maskRoi_tl.setX(intervalCensored(maskRoi_tl.x(), 0, adminConfig->ImageSize_W - 1));
		maskRoi_tl.setY(intervalCensored(maskRoi_tl.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_maskRoi_tl_x->setText(QString::number((int)maskRoi_tl.x()));//更新界面
		ui.lineEdit_maskRoi_tl_y->setText(QString::number((int)maskRoi_tl.y()));
		this->on_pushButton_getMaskRoi_clicked(); //确认区域
		return;
	}

	//鼠标左键单击获取右下角点 - 掩膜
	if (event->button() == Qt::LeftButton && ui.checkBox_maskRoi_br->isChecked()) {
		if (!graphicsViewRect.contains(event->pos())) return; //区域判断
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
		relativePos -= graphicsScenePos; //相对于场景的坐标
		maskRoi_br = relativePos / imageScalingRatio; //坐标转换
		maskRoi_br.setX(intervalCensored(maskRoi_br.x(), 0, adminConfig->ImageSize_W - 1));
		maskRoi_br.setY(intervalCensored(maskRoi_br.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_maskRoi_br_x->setText(QString::number((int)maskRoi_br.x()));//更新界面
		ui.lineEdit_maskRoi_br_y->setText(QString::number((int)maskRoi_br.y()));
		this->on_pushButton_getMaskRoi_clicked(); //确认区域
		return;
	}

	//鼠标左键单击获取左上角点 - OCR
	if (event->button() == Qt::LeftButton && ui.checkBox_ocrRoi_tl->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //区域判断
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
		relativePos -= graphicsScenePos; //相对于场景的坐标
		ocrRoi_tl = relativePos / imageScalingRatio; //坐标转换
		ocrRoi_tl.setX(intervalCensored(ocrRoi_tl.x(), 0, adminConfig->ImageSize_W - 1));
		ocrRoi_tl.setY(intervalCensored(ocrRoi_tl.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_ocrRoi_tl_x->setText(QString::number((int)ocrRoi_tl.x()));//更新界面
		ui.lineEdit_ocrRoi_tl_y->setText(QString::number((int)ocrRoi_tl.y()));
		return;
	}
	
	//鼠标左键单击获取右下角点 - OCR
	if(ui.checkBox_ocrRoi_br->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //区域判断
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
		relativePos -= graphicsScenePos; //相对于场景的坐标
		ocrRoi_br = relativePos / imageScalingRatio; //坐标转换
		ocrRoi_br.setX(intervalCensored(ocrRoi_br.x(), 0, adminConfig->ImageSize_W - 1));
		ocrRoi_br.setY(intervalCensored(ocrRoi_br.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_ocrRoi_br_x->setText(QString::number((int)ocrRoi_br.x()));//更新界面
		ui.lineEdit_ocrRoi_br_y->setText(QString::number((int)ocrRoi_br.y()));
		return;
	}

	//通过拖矩形框的方式选择区域 —— 未完成
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

//对数字进行区间截断
double SerialNumberUI::intervalCensored(double num, double minVal, double maxVal)
{
	double tmpVal = num;
	if (tmpVal < minVal) tmpVal = minVal;
	if (tmpVal > maxVal) tmpVal = maxVal;
	return tmpVal;
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

//根据beginPoint和endPoint计算当前选中的矩形 - 暂时没用
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