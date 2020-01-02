#include "TemplSettingUI.h"

using pcb::RuntimeParams;


TemplSettingUI::TemplSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//成员变量的初始化
	NumberValidator = Q_NULLPTR;
	SegThreshValidator = Q_NULLPTR;

	errorCode = ErrorCode::Default;
	adminConfig = Q_NULLPTR; //系统参数
	runtimeParams = Q_NULLPTR; //运行时的临时参数
	cvmatSamples = Q_NULLPTR; //用于检测的样本图
	qpixmapSamples = Q_NULLPTR; //用于显示的样本图
	imageItem = Q_NULLPTR; //图元
	imageConverter = Q_NULLPTR; //图像转换器
	gridRowIdx = -1; //当前分图所在行
	gridColIdx = -1; //当前分图所在列
	graphicsScenePos.setX(0);//场景左上角坐标
	graphicsScenePos.setY(0);
	imageScalingRatio = 0;//图像缩放比例
}

TemplSettingUI::~TemplSettingUI()
{
	qDebug() << "~SerialNumberUI";
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
	delete SegThreshValidator;
	SegThreshValidator = Q_NULLPTR;
	delete imageConverter;
	imageConverter = Q_NULLPTR; //图像转换器
	deleteImageItem(); //删除图元
}


/*************** 界面的初始化 **************/

//模板设置界面的整体初始化
void TemplSettingUI::init()
{
	//多屏状态下选择在副屏全屏显示
	this->setGeometry(runtimeParams->ScreenRect);

	//限制参数的输入范围
	delete NumberValidator;
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
	ui.lineEdit_maskRoi_tl_x->setValidator(NumberValidator); //掩膜
	ui.lineEdit_maskRoi_tl_y->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_x->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_y->setValidator(NumberValidator);

	delete SegThreshValidator;
	SegThreshValidator = new QRegExpValidator(QRegExp("^(\\d{1,2}|1\\d{2}|2[0-4]\\d{1}|25[0-5])$"));
	ui.lineEdit_segThresh->setValidator(SegThreshValidator); //阈值

	//绘图控件QGraphicsView的初始化
	this->initGraphicsView();

	//模板区域坐标相关控件的初始化
	this->initMaskRoiWidgets();

	//图像分割阈值相关控件的初始化
	this->initSegThreshWidgets();

	//产品序号是否已经设置
	productIdFlag = false; 

	//设置鼠标
	this->setMouseTracking(true);
	this->setCursor(Qt::ArrowCursor);
}

//模板区域坐标相关控件的初始化
void TemplSettingUI::initMaskRoiWidgets()
{
	//模板区域坐标是否已经设置
	maskRoiFlag = false;

	//设置掩膜区域的坐标初始值
	runtimeParams->maskRoi_tl.setX(-1);
	runtimeParams->maskRoi_tl.setY(-1);
	runtimeParams->maskRoi_br.setX(-1);
	runtimeParams->maskRoi_br.setY(-1);

	//初始化CheckBox 使得同一时刻只有一个box可以选
	QPushButton button;
	checkBoxGroup.addButton(&button);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_tl, 1);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_br, 2);
}

//图像分割阈值相关控件的初始化
void TemplSettingUI::initSegThreshWidgets()
{
	connect(ui.horizontalSlider_segThresh, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_segThresh_changed(int)));
	ui.horizontalSlider_segThresh->setMinimum(0); //设置滑条的最小值
	ui.horizontalSlider_segThresh->setMaximum(255); //设置滑条的最大值

	this->resetSegThreshWidgets(); //重置
	segThreshFlag = false; //图像分割阈值是否已经设置
}

//绘图控件QGraphicsView的初始化
void TemplSettingUI::initGraphicsView()
{
	//图像刷新
	imageConverter = new ImageConverter(); //图像转换器
	lastImageRefreshTime = QTime::currentTime();

	//获取绘图控件QGraphicsView的位置
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//禁用滚动条
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


/*************** 界面的设置与更新 **************/

//切换界面控件的状态
void TemplSettingUI::setTemplSettingUIEnabled(bool enable)
{
	ui.lineEdit_maskRoi_tl_x->setEnabled(enable);
	ui.lineEdit_maskRoi_tl_y->setEnabled(enable);
	ui.lineEdit_maskRoi_br_x->setEnabled(enable);
	ui.lineEdit_maskRoi_br_y->setEnabled(enable);
	ui.pushButton_maskRoi->setEnabled(enable); //确认Mask区域

	if (ui.checkBox_segThresh->isChecked()) { //确认框已勾选
		ui.horizontalSlider_segThresh->setEnabled(enable); //阈值滑条
		ui.lineEdit_segThresh->setEnabled(enable); //阈值输入框
		ui.pushButton_segThresh->setEnabled(enable); //按键
	}
	
	ui.lineEdit_modelType->setEnabled(enable); //型号输入框
	ui.pushButton_modelType->setEnabled(enable);

	ui.pushButton_confirm->setEnabled(enable); //确定并返回
	ui.pushButton_return->setEnabled(enable); //返回
}

//重置界面
void TemplSettingUI::reset()
{
	if (this == Q_NULLPTR) return;

	this->setCursor(Qt::ArrowCursor); //设置鼠标
	this->resetMaskRoiWidgets(); //重置模板区域坐标相关的控件
	this->resetSegThreshWidgets(); //重置分割阈值相关的控件

	ui.lineEdit_modelType->setText(""); //清空型号
	runtimeParams->productID.resetModelType(); //重置产品id
	productIdFlag = false; //产品序号是否已经设置
	deleteImageItem(); //删除绘图控件中的图元
}

//重置模板区域坐标相关的控件
void TemplSettingUI::resetMaskRoiWidgets()
{
	//去除确认框中的对勾
	ui.checkBox_maskRoi_tl->setChecked(false); 
	ui.checkBox_maskRoi_br->setChecked(false);

	//清空掩膜区域坐标
	ui.lineEdit_maskRoi_tl_x->setText("");
	ui.lineEdit_maskRoi_tl_y->setText("");
	ui.lineEdit_maskRoi_br_x->setText("");
	ui.lineEdit_maskRoi_br_y->setText("");

	//模板区域坐标是否已经设置
	maskRoiFlag = false; 
}

//重置图像分割阈值相关的控件
void TemplSettingUI::resetSegThreshWidgets() 
{
	ui.checkBox_segThresh->setChecked(false); //取消确认框中的对勾
	ui.horizontalSlider_segThresh->setValue(0); //滑条归零
	ui.horizontalSlider_segThresh->setEnabled(false); //滑条设为不可拖拽
	ui.lineEdit_segThresh->setText(""); //清空文本框中的值
	ui.lineEdit_segThresh->setEnabled(false); //文本框设为不可编辑
	ui.pushButton_segThresh->setEnabled(false); //按键设为不可点击
	
	runtimeParams->UsingDefaultSegThresh = true; //采用默认阈值
	segThreshFlag = true; //图像分割阈值是否已经设置
}


/************** 获取掩膜区域的坐标 **************/

void TemplSettingUI::on_pushButton_maskRoi_clicked()
{
	this->setCursor(Qt::ArrowCursor); //设置鼠标样式

	ui.checkBox_maskRoi_tl->setChecked(false);
	ui.checkBox_maskRoi_br->setChecked(false);
	//this->setTemplSettingUIEnabled(false);

	int ImageSize_W = adminConfig->ImageSize_W;
	int ImageSize_H = adminConfig->ImageSize_H;
	int tl_x = -2, tl_y = -2, br_x = -2, br_y = -2;

	//获取掩膜区域的坐标 - 左上角
	QString str_tl_x = ui.lineEdit_maskRoi_tl_x->text(); //x
	if (str_tl_x == "") tl_x = -1;
	else tl_x = (int)intervalCensored(str_tl_x.toInt(), 0, ImageSize_W - 1);

	QString str_tl_y = ui.lineEdit_maskRoi_tl_y->text(); //y
	if (str_tl_y == "") tl_y = -1;
	else tl_y = (int)intervalCensored(str_tl_y.toInt(), 0, ImageSize_H - 1);

	//获取掩膜区域的坐标 - 右下角
	QString str_br_x = ui.lineEdit_maskRoi_br_x->text(); //x
	if (str_br_x == "") br_x = -1;
	else br_x = (int)intervalCensored(str_br_x.toInt(), 0, ImageSize_W - 1);

	QString str_br_y = ui.lineEdit_maskRoi_br_y->text(); //y
	if (str_br_y == "") br_y = -1;
	else br_y = (int)intervalCensored(str_br_y.toInt(), 0, ImageSize_H - 1);

	//更新界面上的坐标值
	if (tl_x >= 0) ui.lineEdit_maskRoi_tl_x->setText(QString::number(tl_x));
	if (tl_y >= 0) ui.lineEdit_maskRoi_tl_y->setText(QString::number(tl_y));
	if (br_x >= 0) ui.lineEdit_maskRoi_br_x->setText(QString::number(br_x));
	if (br_y >= 0) ui.lineEdit_maskRoi_br_y->setText(QString::number(br_y));

	//将区域坐标存入运行参数类
	if (tl_x >= 0) runtimeParams->maskRoi_tl.setX(tl_x);
	if (tl_y >= 0) runtimeParams->maskRoi_tl.setY(tl_y);
	if (br_x >= 0) runtimeParams->maskRoi_br.setX(br_x);
	if (br_y >= 0) runtimeParams->maskRoi_br.setY(br_y);

	//模板区域坐标是否已经设置
	maskRoiFlag = (tl_x >= 0 && tl_y >= 0 && br_x >= 0 && br_y >= 0);
	if (maskRoiFlag) { emit segThreshIsSet_templSettingUI(); }
	//this->setTemplSettingUIEnabled(true);
}


/***************** 图像分割阈值 *****************/

//确认框的勾选状态发生变化时 - 施工中
void TemplSettingUI::on_checkBox_segThresh_clicked() 
{
	//被勾选后，进入交互式调整阈值的模式
	if (ui.checkBox_segThresh->isChecked()) {
		ui.horizontalSlider_segThresh->setValue(128); //滑条归零
		ui.horizontalSlider_segThresh->setEnabled(true); //滑条设为不可拖拽
		ui.lineEdit_segThresh->setText("128"); //清空文本框中的值
		ui.lineEdit_segThresh->setEnabled(true); //文本框设为不可编辑
		ui.pushButton_segThresh->setEnabled(true); //按键设为不可点击
		this->showImageDividedByThresh(128); //显示分割后的结果
	}
	else { //采用默认阈值，即程序自动计算的值
		this->resetSegThreshWidgets(); //重置
		this->showSampleImage(gridRowIdx, gridColIdx); //显示分割前的灰度图
	}
}

//滑条中的滑块位置发生变化时
void TemplSettingUI::on_horizontalSlider_segThresh_changed(int value)
{
	if (!ui.checkBox_segThresh->isChecked()) return;
	ui.lineEdit_segThresh->setText(QString::number(value));

	//动态更新分割结果
	QTime curTime = QTime::currentTime();
	if (curTime > lastImageRefreshTime.addMSecs(10)) { //防止滑块拖动过快时来不及显示图像
		if (imageConverter->isRunning()) return;
		this->showImageDividedByThresh(value); //显示分割结果
		lastImageRefreshTime = curTime; //更新时间
	}
}

//点击阈值确认按键
void TemplSettingUI::on_pushButton_segThresh_clicked()
{
	int value = ui.lineEdit_segThresh->text().toInt();
	if (value < 0 || value > 255) return;

	runtimeParams->segThresh = value;
	segThreshFlag = true; //图像分割阈值是否已经设置
	runtimeParams->UsingDefaultSegThresh = false; //使用手动设置的阈值
	this->showImageDividedByThresh(value); //显示分割结果
}

//显示阈值分割后的图像
void TemplSettingUI::showImageDividedByThresh(int thresh)
{
	//获取图像 阈值分割
	cv::Mat segImage;
	cv::threshold(currentCvMatImage, segImage, thresh, 255, cv::THRESH_BINARY);

	//cv::Mat转换为QPixmap
	QPixmap image;
	imageConverter->set(&segImage, &image, ImageConverter::CvMat2QPixmap);
	imageConverter->start();
	while (imageConverter->isRunning()) { pcb::delay(1); }

	//绘制图像
	this->showSampleImage(image);
}


/***************** 获取产品型号 *****************/

void TemplSettingUI::on_pushButton_modelType_clicked()
{
	runtimeParams->productID.setModelType(ui.lineEdit_modelType->text()); //获取产品id
	productIdFlag = runtimeParams->productID.isModelTypeValid(); //产品序号是否已经设置
	if (productIdFlag) { emit modelTypeIsSet_templSettingUI(); }
}


/**************** 显示分图 ****************/

//在graphicView中显示分图
void TemplSettingUI::showSampleImage(int row, int col)
{
	gridRowIdx = row; //当前分图所在行
	gridColIdx = col; //当前分图所在列
	QPixmap *img = (*qpixmapSamples)[gridRowIdx][gridColIdx];
	QPixmap scaledImg = img->scaled(graphicsViewRect.size(), Qt::KeepAspectRatio);
	imageScalingRatio = 1.0 * scaledImg.width() / img->width();
	this->showSampleImage(scaledImg); //绘制图像
	qApp->processEvents();

	currentCvMatImage = (*cvmatSamples)[gridRowIdx][gridColIdx]->clone(); //当前正在显示的图
	cv::Size size = cv::Size(scaledImg.width(), scaledImg.height());
	cv::resize(currentCvMatImage, currentCvMatImage, size, 0, 0, cv::INTER_LINEAR);
}

//在graphicView中显示图像
void TemplSettingUI::showSampleImage(const QPixmap &img)
{
	//在场景中加载新的图元
	deleteImageItem(); //删除之前的图元
	imageItem = new QGraphicsPixmapItem(img); //定义新图元
	graphicsScene.addItem(imageItem);

	//计算场景左上角点的坐标
	QSize graphicsViewSize = graphicsViewRect.size();
	QPointF scenePosToView(graphicsViewSize.width() / 2.0, graphicsViewSize.height() / 2.0);
	graphicsScenePos = scenePosToView - QPointF(img.width(), img.height()) / 2.0;

	ui.graphicsView->show(); //显示
}

//删除graphicsView中显示的的图像
void TemplSettingUI::deleteImageItem()
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


/**************** 鼠标事件 ****************/

//鼠标按下事件
void TemplSettingUI::mousePressEvent(QMouseEvent *event)
{
	this->setCursor(Qt::ArrowCursor); //设置鼠标样式

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
		//this->on_pushButton_maskRoi_clicked(); //确认区域
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
		//this->on_pushButton_maskRoi_clicked(); //确认区域
		return;
	}

	//鼠标左键单击获取左上角点 - OCR
	//if (event->button() == Qt::LeftButton && ui.checkBox_ocrRoi_tl->isChecked()) { 
	//	if (!graphicsViewRect.contains(event->pos())) return; //区域判断
	//	QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
	//	relativePos -= graphicsScenePos; //相对于场景的坐标
	//	ocrRoi_tl = relativePos / imageScalingRatio; //坐标转换
	//	ocrRoi_tl.setX(intervalCensored(ocrRoi_tl.x(), 0, adminConfig->ImageSize_W - 1));
	//	ocrRoi_tl.setY(intervalCensored(ocrRoi_tl.y(), 0, adminConfig->ImageSize_H - 1));
	//	ui.lineEdit_ocrRoi_tl_x->setText(QString::number((int)ocrRoi_tl.x()));//更新界面
	//	ui.lineEdit_ocrRoi_tl_y->setText(QString::number((int)ocrRoi_tl.y()));
	//	return;
	//}
	
	//鼠标左键单击获取右下角点 - OCR
	//if(ui.checkBox_ocrRoi_br->isChecked()) { 
	//	if (!graphicsViewRect.contains(event->pos())) return; //区域判断
	//	QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//相对于视图的坐标
	//	relativePos -= graphicsScenePos; //相对于场景的坐标
	//	ocrRoi_br = relativePos / imageScalingRatio; //坐标转换
	//	ocrRoi_br.setX(intervalCensored(ocrRoi_br.x(), 0, adminConfig->ImageSize_W - 1));
	//	ocrRoi_br.setY(intervalCensored(ocrRoi_br.y(), 0, adminConfig->ImageSize_H - 1));
	//	ui.lineEdit_ocrRoi_br_x->setText(QString::number((int)ocrRoi_br.x()));//更新界面
	//	ui.lineEdit_ocrRoi_br_y->setText(QString::number((int)ocrRoi_br.y()));
	//	return;
	//}

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
double TemplSettingUI::intervalCensored(double num, double minVal, double maxVal)
{
	double tmpVal = num;
	if (tmpVal < minVal) tmpVal = minVal;
	if (tmpVal > maxVal) tmpVal = maxVal;
	return tmpVal;
}


//鼠标移动事件 - 暂时没用
void TemplSettingUI::mouseMoveEvent(QMouseEvent *event)
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
void TemplSettingUI::mouseReleaseEvent(QMouseEvent *event)
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
bool TemplSettingUI::isPressPosInGraphicViewRect(QPoint mousePressPos)
{
	QRect selectRect = getRect(mousePressPos, mouseReleasePos);
	return selectRect.contains(mousePressPos);
}

//根据beginPoint和endPoint计算当前选中的矩形 - 暂时没用
QRect TemplSettingUI::getRect(const QPoint &beginPoint, const QPoint &endPoint)
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


/************** 确定与返回 ***************/

//界面底部的确定按键
void TemplSettingUI::on_pushButton_confirm_clicked()
{
	this->setTemplSettingUIEnabled(false); //禁用按键

	this->on_pushButton_maskRoi_clicked();
	if (!runtimeParams->UsingDefaultSegThresh) this->on_pushButton_segThresh_clicked();
	this->on_pushButton_modelType_clicked();

	if (this->isReadyForExtract()) {
		emit settingFinished_templSettingUI(); //向上一级界面发送设置结束的信号
		on_pushButton_return_clicked(); //返回上一级界面，并执行下一步处理
	}
	this->setTemplSettingUIEnabled(true); //开启按键
}

//界面底部的返回按键
void TemplSettingUI::on_pushButton_return_clicked()
{
	emit showExtractUI_templSettingUI();
}


/********************* 其他 ********************/

//弹窗警告
bool TemplSettingUI::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == TemplSettingUI::NoError) return false;

	QString valueName;
	switch (tempCode)
	{
	case TemplSettingUI::Unchecked:
		valueName = pcb::chinese("\"状态未验证\""); break;
	case TemplSettingUI::InitFailed:
		valueName = pcb::chinese("\"初始化失败\""); break;
	case TemplSettingUI::Invalid_RoiRect:
		valueName = pcb::chinese("\"序号区域\""); break;
	case TemplSettingUI::Invalid_RoiData:
		valueName = pcb::chinese("\"序号区域数据丢失\""); break;
	case TemplSettingUI::RecognizeFailed:
		valueName = pcb::chinese("\"序号识别失败\""); break;
	case TemplSettingUI::Invalid_SerialNum:
		valueName = pcb::chinese("\"产品序号\""); break;
	case TemplSettingUI::Default:
		valueName = pcb::chinese("\"未知错误\""); break;
	default:
		break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("产品序号识别错误！ \n") +
		pcb::chinese("错误来源：") + valueName + "!        \n" +
		"ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return false;
}