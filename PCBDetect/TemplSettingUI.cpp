#include "TemplSettingUI.h"

using pcb::RuntimeParams;


TemplSettingUI::TemplSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//��Ա�����ĳ�ʼ��
	NumberValidator = Q_NULLPTR;
	SegThreshValidator = Q_NULLPTR;

	errorCode = ErrorCode::Default;
	adminConfig = Q_NULLPTR; //ϵͳ����
	runtimeParams = Q_NULLPTR; //����ʱ����ʱ����
	cvmatSamples = Q_NULLPTR; //���ڼ�������ͼ
	qpixmapSamples = Q_NULLPTR; //������ʾ������ͼ
	imageItem = Q_NULLPTR; //ͼԪ
	imageConverter = Q_NULLPTR; //ͼ��ת����
	gridRowIdx = -1; //��ǰ��ͼ������
	gridColIdx = -1; //��ǰ��ͼ������
	graphicsScenePos.setX(0);//�������Ͻ�����
	graphicsScenePos.setY(0);
	imageScalingRatio = 0;//ͼ�����ű���
}

TemplSettingUI::~TemplSettingUI()
{
	qDebug() << "~SerialNumberUI";
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
	delete SegThreshValidator;
	SegThreshValidator = Q_NULLPTR;
	delete imageConverter;
	imageConverter = Q_NULLPTR; //ͼ��ת����
	deleteImageItem(); //ɾ��ͼԪ
}


/*************** ����ĳ�ʼ�� **************/

//ģ�����ý���������ʼ��
void TemplSettingUI::init()
{
	//����״̬��ѡ���ڸ���ȫ����ʾ
	this->setGeometry(runtimeParams->ScreenRect);

	//���Ʋ��������뷶Χ
	delete NumberValidator;
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
	ui.lineEdit_maskRoi_tl_x->setValidator(NumberValidator); //��Ĥ
	ui.lineEdit_maskRoi_tl_y->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_x->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_y->setValidator(NumberValidator);

	delete SegThreshValidator;
	SegThreshValidator = new QRegExpValidator(QRegExp("^(\\d{1,2}|1\\d{2}|2[0-4]\\d{1}|25[0-5])$"));
	ui.lineEdit_segThresh->setValidator(SegThreshValidator); //��ֵ

	//��ͼ�ؼ�QGraphicsView�ĳ�ʼ��
	this->initGraphicsView();

	//ģ������������ؿؼ��ĳ�ʼ��
	this->initMaskRoiWidgets();

	//ͼ��ָ���ֵ��ؿؼ��ĳ�ʼ��
	this->initSegThreshWidgets();

	//��Ʒ����Ƿ��Ѿ�����
	productIdFlag = false; 

	//�������
	this->setMouseTracking(true);
	this->setCursor(Qt::ArrowCursor);
}

//ģ������������ؿؼ��ĳ�ʼ��
void TemplSettingUI::initMaskRoiWidgets()
{
	//ģ�����������Ƿ��Ѿ�����
	maskRoiFlag = false;

	//������Ĥ����������ʼֵ
	runtimeParams->maskRoi_tl.setX(-1);
	runtimeParams->maskRoi_tl.setY(-1);
	runtimeParams->maskRoi_br.setX(-1);
	runtimeParams->maskRoi_br.setY(-1);

	//��ʼ��CheckBox ʹ��ͬһʱ��ֻ��һ��box����ѡ
	QPushButton button;
	checkBoxGroup.addButton(&button);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_tl, 1);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_br, 2);
}

//ͼ��ָ���ֵ��ؿؼ��ĳ�ʼ��
void TemplSettingUI::initSegThreshWidgets()
{
	connect(ui.horizontalSlider_segThresh, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_segThresh_changed(int)));
	ui.horizontalSlider_segThresh->setMinimum(0); //���û�������Сֵ
	ui.horizontalSlider_segThresh->setMaximum(255); //���û��������ֵ

	this->resetSegThreshWidgets(); //����
	segThreshFlag = false; //ͼ��ָ���ֵ�Ƿ��Ѿ�����
}

//��ͼ�ؼ�QGraphicsView�ĳ�ʼ��
void TemplSettingUI::initGraphicsView()
{
	//ͼ��ˢ��
	imageConverter = new ImageConverter(); //ͼ��ת����
	lastImageRefreshTime = QTime::currentTime();

	//��ȡ��ͼ�ؼ�QGraphicsView��λ��
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//���ù�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


/*************** �������������� **************/

//�л�����ؼ���״̬
void TemplSettingUI::setTemplSettingUIEnabled(bool enable)
{
	ui.lineEdit_maskRoi_tl_x->setEnabled(enable);
	ui.lineEdit_maskRoi_tl_y->setEnabled(enable);
	ui.lineEdit_maskRoi_br_x->setEnabled(enable);
	ui.lineEdit_maskRoi_br_y->setEnabled(enable);
	ui.pushButton_maskRoi->setEnabled(enable); //ȷ��Mask����

	if (ui.checkBox_segThresh->isChecked()) { //ȷ�Ͽ��ѹ�ѡ
		ui.horizontalSlider_segThresh->setEnabled(enable); //��ֵ����
		ui.lineEdit_segThresh->setEnabled(enable); //��ֵ�����
		ui.pushButton_segThresh->setEnabled(enable); //����
	}
	
	ui.lineEdit_modelType->setEnabled(enable); //�ͺ������
	ui.pushButton_modelType->setEnabled(enable);

	ui.pushButton_confirm->setEnabled(enable); //ȷ��������
	ui.pushButton_return->setEnabled(enable); //����
}

//���ý���
void TemplSettingUI::reset()
{
	if (this == Q_NULLPTR) return;

	this->setCursor(Qt::ArrowCursor); //�������
	this->resetMaskRoiWidgets(); //����ģ������������صĿؼ�
	this->resetSegThreshWidgets(); //���÷ָ���ֵ��صĿؼ�

	ui.lineEdit_modelType->setText(""); //����ͺ�
	runtimeParams->productID.resetModelType(); //���ò�Ʒid
	productIdFlag = false; //��Ʒ����Ƿ��Ѿ�����
	deleteImageItem(); //ɾ����ͼ�ؼ��е�ͼԪ
}

//����ģ������������صĿؼ�
void TemplSettingUI::resetMaskRoiWidgets()
{
	//ȥ��ȷ�Ͽ��еĶԹ�
	ui.checkBox_maskRoi_tl->setChecked(false); 
	ui.checkBox_maskRoi_br->setChecked(false);

	//�����Ĥ��������
	ui.lineEdit_maskRoi_tl_x->setText("");
	ui.lineEdit_maskRoi_tl_y->setText("");
	ui.lineEdit_maskRoi_br_x->setText("");
	ui.lineEdit_maskRoi_br_y->setText("");

	//ģ�����������Ƿ��Ѿ�����
	maskRoiFlag = false; 
}

//����ͼ��ָ���ֵ��صĿؼ�
void TemplSettingUI::resetSegThreshWidgets() 
{
	ui.checkBox_segThresh->setChecked(false); //ȡ��ȷ�Ͽ��еĶԹ�
	ui.horizontalSlider_segThresh->setValue(0); //��������
	ui.horizontalSlider_segThresh->setEnabled(false); //������Ϊ������ק
	ui.lineEdit_segThresh->setText(""); //����ı����е�ֵ
	ui.lineEdit_segThresh->setEnabled(false); //�ı�����Ϊ���ɱ༭
	ui.pushButton_segThresh->setEnabled(false); //������Ϊ���ɵ��
	
	runtimeParams->UsingDefaultSegThresh = true; //����Ĭ����ֵ
	segThreshFlag = true; //ͼ��ָ���ֵ�Ƿ��Ѿ�����
}


/************** ��ȡ��Ĥ��������� **************/

void TemplSettingUI::on_pushButton_maskRoi_clicked()
{
	this->setCursor(Qt::ArrowCursor); //���������ʽ

	ui.checkBox_maskRoi_tl->setChecked(false);
	ui.checkBox_maskRoi_br->setChecked(false);
	//this->setTemplSettingUIEnabled(false);

	int ImageSize_W = adminConfig->ImageSize_W;
	int ImageSize_H = adminConfig->ImageSize_H;
	int tl_x = -2, tl_y = -2, br_x = -2, br_y = -2;

	//��ȡ��Ĥ��������� - ���Ͻ�
	QString str_tl_x = ui.lineEdit_maskRoi_tl_x->text(); //x
	if (str_tl_x == "") tl_x = -1;
	else tl_x = (int)intervalCensored(str_tl_x.toInt(), 0, ImageSize_W - 1);

	QString str_tl_y = ui.lineEdit_maskRoi_tl_y->text(); //y
	if (str_tl_y == "") tl_y = -1;
	else tl_y = (int)intervalCensored(str_tl_y.toInt(), 0, ImageSize_H - 1);

	//��ȡ��Ĥ��������� - ���½�
	QString str_br_x = ui.lineEdit_maskRoi_br_x->text(); //x
	if (str_br_x == "") br_x = -1;
	else br_x = (int)intervalCensored(str_br_x.toInt(), 0, ImageSize_W - 1);

	QString str_br_y = ui.lineEdit_maskRoi_br_y->text(); //y
	if (str_br_y == "") br_y = -1;
	else br_y = (int)intervalCensored(str_br_y.toInt(), 0, ImageSize_H - 1);

	//���½����ϵ�����ֵ
	if (tl_x >= 0) ui.lineEdit_maskRoi_tl_x->setText(QString::number(tl_x));
	if (tl_y >= 0) ui.lineEdit_maskRoi_tl_y->setText(QString::number(tl_y));
	if (br_x >= 0) ui.lineEdit_maskRoi_br_x->setText(QString::number(br_x));
	if (br_y >= 0) ui.lineEdit_maskRoi_br_y->setText(QString::number(br_y));

	//����������������в�����
	if (tl_x >= 0) runtimeParams->maskRoi_tl.setX(tl_x);
	if (tl_y >= 0) runtimeParams->maskRoi_tl.setY(tl_y);
	if (br_x >= 0) runtimeParams->maskRoi_br.setX(br_x);
	if (br_y >= 0) runtimeParams->maskRoi_br.setY(br_y);

	//ģ�����������Ƿ��Ѿ�����
	maskRoiFlag = (tl_x >= 0 && tl_y >= 0 && br_x >= 0 && br_y >= 0);
	if (maskRoiFlag) { emit segThreshIsSet_templSettingUI(); }
	//this->setTemplSettingUIEnabled(true);
}


/***************** ͼ��ָ���ֵ *****************/

//ȷ�Ͽ�Ĺ�ѡ״̬�����仯ʱ - ʩ����
void TemplSettingUI::on_checkBox_segThresh_clicked() 
{
	//����ѡ�󣬽��뽻��ʽ������ֵ��ģʽ
	if (ui.checkBox_segThresh->isChecked()) {
		ui.horizontalSlider_segThresh->setValue(128); //��������
		ui.horizontalSlider_segThresh->setEnabled(true); //������Ϊ������ק
		ui.lineEdit_segThresh->setText("128"); //����ı����е�ֵ
		ui.lineEdit_segThresh->setEnabled(true); //�ı�����Ϊ���ɱ༭
		ui.pushButton_segThresh->setEnabled(true); //������Ϊ���ɵ��
		this->showImageDividedByThresh(128); //��ʾ�ָ��Ľ��
	}
	else { //����Ĭ����ֵ���������Զ������ֵ
		this->resetSegThreshWidgets(); //����
		this->showSampleImage(gridRowIdx, gridColIdx); //��ʾ�ָ�ǰ�ĻҶ�ͼ
	}
}

//�����еĻ���λ�÷����仯ʱ
void TemplSettingUI::on_horizontalSlider_segThresh_changed(int value)
{
	if (!ui.checkBox_segThresh->isChecked()) return;
	ui.lineEdit_segThresh->setText(QString::number(value));

	//��̬���·ָ���
	QTime curTime = QTime::currentTime();
	if (curTime > lastImageRefreshTime.addMSecs(10)) { //��ֹ�����϶�����ʱ��������ʾͼ��
		if (imageConverter->isRunning()) return;
		this->showImageDividedByThresh(value); //��ʾ�ָ���
		lastImageRefreshTime = curTime; //����ʱ��
	}
}

//�����ֵȷ�ϰ���
void TemplSettingUI::on_pushButton_segThresh_clicked()
{
	int value = ui.lineEdit_segThresh->text().toInt();
	if (value < 0 || value > 255) return;

	runtimeParams->segThresh = value;
	segThreshFlag = true; //ͼ��ָ���ֵ�Ƿ��Ѿ�����
	runtimeParams->UsingDefaultSegThresh = false; //ʹ���ֶ����õ���ֵ
	this->showImageDividedByThresh(value); //��ʾ�ָ���
}

//��ʾ��ֵ�ָ���ͼ��
void TemplSettingUI::showImageDividedByThresh(int thresh)
{
	//��ȡͼ�� ��ֵ�ָ�
	cv::Mat segImage;
	cv::threshold(currentCvMatImage, segImage, thresh, 255, cv::THRESH_BINARY);

	//cv::Matת��ΪQPixmap
	QPixmap image;
	imageConverter->set(&segImage, &image, ImageConverter::CvMat2QPixmap);
	imageConverter->start();
	while (imageConverter->isRunning()) { pcb::delay(1); }

	//����ͼ��
	this->showSampleImage(image);
}


/***************** ��ȡ��Ʒ�ͺ� *****************/

void TemplSettingUI::on_pushButton_modelType_clicked()
{
	runtimeParams->productID.setModelType(ui.lineEdit_modelType->text()); //��ȡ��Ʒid
	productIdFlag = runtimeParams->productID.isModelTypeValid(); //��Ʒ����Ƿ��Ѿ�����
	if (productIdFlag) { emit modelTypeIsSet_templSettingUI(); }
}


/**************** ��ʾ��ͼ ****************/

//��graphicView����ʾ��ͼ
void TemplSettingUI::showSampleImage(int row, int col)
{
	gridRowIdx = row; //��ǰ��ͼ������
	gridColIdx = col; //��ǰ��ͼ������
	QPixmap *img = (*qpixmapSamples)[gridRowIdx][gridColIdx];
	QPixmap scaledImg = img->scaled(graphicsViewRect.size(), Qt::KeepAspectRatio);
	imageScalingRatio = 1.0 * scaledImg.width() / img->width();
	this->showSampleImage(scaledImg); //����ͼ��
	qApp->processEvents();

	currentCvMatImage = (*cvmatSamples)[gridRowIdx][gridColIdx]->clone(); //��ǰ������ʾ��ͼ
	cv::Size size = cv::Size(scaledImg.width(), scaledImg.height());
	cv::resize(currentCvMatImage, currentCvMatImage, size, 0, 0, cv::INTER_LINEAR);
}

//��graphicView����ʾͼ��
void TemplSettingUI::showSampleImage(const QPixmap &img)
{
	//�ڳ����м����µ�ͼԪ
	deleteImageItem(); //ɾ��֮ǰ��ͼԪ
	imageItem = new QGraphicsPixmapItem(img); //������ͼԪ
	graphicsScene.addItem(imageItem);

	//���㳡�����Ͻǵ������
	QSize graphicsViewSize = graphicsViewRect.size();
	QPointF scenePosToView(graphicsViewSize.width() / 2.0, graphicsViewSize.height() / 2.0);
	graphicsScenePos = scenePosToView - QPointF(img.width(), img.height()) / 2.0;

	ui.graphicsView->show(); //��ʾ
}

//ɾ��graphicsView����ʾ�ĵ�ͼ��
void TemplSettingUI::deleteImageItem()
{
	//�����ڴ�
	delete imageItem;
	imageItem = Q_NULLPTR;

	//�ӳ������Ƴ�
	QList<QGraphicsItem *> itemList = graphicsScene.items();
	for (int i = 0; i < itemList.size(); i++) {
		graphicsScene.removeItem(itemList[i]);
	}

	//���ó����������ͼ��λ��
	graphicsScenePos.setX(0);
	graphicsScenePos.setY(0);
}


/**************** ����¼� ****************/

//��갴���¼�
void TemplSettingUI::mousePressEvent(QMouseEvent *event)
{
	this->setCursor(Qt::ArrowCursor); //���������ʽ

	//������������ȡ���Ͻǵ� - ��Ĥ
	if (event->button() == Qt::LeftButton && ui.checkBox_maskRoi_tl->isChecked()) {
		if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
		relativePos -= graphicsScenePos; //����ڳ���������
		maskRoi_tl = relativePos / imageScalingRatio; //����ת��
		maskRoi_tl.setX(intervalCensored(maskRoi_tl.x(), 0, adminConfig->ImageSize_W - 1));
		maskRoi_tl.setY(intervalCensored(maskRoi_tl.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_maskRoi_tl_x->setText(QString::number((int)maskRoi_tl.x()));//���½���
		ui.lineEdit_maskRoi_tl_y->setText(QString::number((int)maskRoi_tl.y()));
		//this->on_pushButton_maskRoi_clicked(); //ȷ������
		return;
	}

	//������������ȡ���½ǵ� - ��Ĥ
	if (event->button() == Qt::LeftButton && ui.checkBox_maskRoi_br->isChecked()) {
		if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
		relativePos -= graphicsScenePos; //����ڳ���������
		maskRoi_br = relativePos / imageScalingRatio; //����ת��
		maskRoi_br.setX(intervalCensored(maskRoi_br.x(), 0, adminConfig->ImageSize_W - 1));
		maskRoi_br.setY(intervalCensored(maskRoi_br.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_maskRoi_br_x->setText(QString::number((int)maskRoi_br.x()));//���½���
		ui.lineEdit_maskRoi_br_y->setText(QString::number((int)maskRoi_br.y()));
		//this->on_pushButton_maskRoi_clicked(); //ȷ������
		return;
	}

	//������������ȡ���Ͻǵ� - OCR
	//if (event->button() == Qt::LeftButton && ui.checkBox_ocrRoi_tl->isChecked()) { 
	//	if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
	//	QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
	//	relativePos -= graphicsScenePos; //����ڳ���������
	//	ocrRoi_tl = relativePos / imageScalingRatio; //����ת��
	//	ocrRoi_tl.setX(intervalCensored(ocrRoi_tl.x(), 0, adminConfig->ImageSize_W - 1));
	//	ocrRoi_tl.setY(intervalCensored(ocrRoi_tl.y(), 0, adminConfig->ImageSize_H - 1));
	//	ui.lineEdit_ocrRoi_tl_x->setText(QString::number((int)ocrRoi_tl.x()));//���½���
	//	ui.lineEdit_ocrRoi_tl_y->setText(QString::number((int)ocrRoi_tl.y()));
	//	return;
	//}
	
	//������������ȡ���½ǵ� - OCR
	//if(ui.checkBox_ocrRoi_br->isChecked()) { 
	//	if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
	//	QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
	//	relativePos -= graphicsScenePos; //����ڳ���������
	//	ocrRoi_br = relativePos / imageScalingRatio; //����ת��
	//	ocrRoi_br.setX(intervalCensored(ocrRoi_br.x(), 0, adminConfig->ImageSize_W - 1));
	//	ocrRoi_br.setY(intervalCensored(ocrRoi_br.y(), 0, adminConfig->ImageSize_H - 1));
	//	ui.lineEdit_ocrRoi_br_x->setText(QString::number((int)ocrRoi_br.x()));//���½���
	//	ui.lineEdit_ocrRoi_br_y->setText(QString::number((int)ocrRoi_br.y()));
	//	return;
	//}

	//ͨ���Ͼ��ο�ķ�ʽѡ������ ���� δ���
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

//�����ֽ�������ض�
double TemplSettingUI::intervalCensored(double num, double minVal, double maxVal)
{
	double tmpVal = num;
	if (tmpVal < minVal) tmpVal = minVal;
	if (tmpVal > maxVal) tmpVal = maxVal;
	return tmpVal;
}


//����ƶ��¼� - ��ʱû��
void TemplSettingUI::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton && mousePress) {
		if (!this->mouseRelease) {
			this->mouseReleasePos = event->pos();//��������ͷŵ�λ��
			//paint();

			qDebug() << mouseReleasePos.x() << mouseReleasePos.y();
		}
	}
}

//����ͷ��¼� - ��ʱû��
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

// ��ʱû��
bool TemplSettingUI::isPressPosInGraphicViewRect(QPoint mousePressPos)
{
	QRect selectRect = getRect(mousePressPos, mouseReleasePos);
	return selectRect.contains(mousePressPos);
}

//����beginPoint��endPoint���㵱ǰѡ�еľ��� - ��ʱû��
QRect TemplSettingUI::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
	int x, y, width, height;
	width = qAbs(beginPoint.x() - endPoint.x());
	height = qAbs(beginPoint.y() - endPoint.y());
	x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
	y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

	// �������Ϊ��ʱ������ͼ����;
	// ��ѡȡ��ͼ����Ϊ��ʱĬ��Ϊ2;
	QRect selectedRect = QRect(x, y, width, height);
	if (selectedRect.width() == 0) selectedRect.setWidth(1);
	if (selectedRect.height() == 0) selectedRect.setHeight(1);

	return selectedRect;
}


/************** ȷ���뷵�� ***************/

//����ײ���ȷ������
void TemplSettingUI::on_pushButton_confirm_clicked()
{
	this->setTemplSettingUIEnabled(false); //���ð���

	this->on_pushButton_maskRoi_clicked();
	if (!runtimeParams->UsingDefaultSegThresh) this->on_pushButton_segThresh_clicked();
	this->on_pushButton_modelType_clicked();

	if (this->isReadyForExtract()) {
		emit settingFinished_templSettingUI(); //����һ�����淢�����ý������ź�
		on_pushButton_return_clicked(); //������һ�����棬��ִ����һ������
	}
	this->setTemplSettingUIEnabled(true); //��������
}

//����ײ��ķ��ذ���
void TemplSettingUI::on_pushButton_return_clicked()
{
	emit showExtractUI_templSettingUI();
}


/********************* ���� ********************/

//��������
bool TemplSettingUI::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == TemplSettingUI::NoError) return false;

	QString valueName;
	switch (tempCode)
	{
	case TemplSettingUI::Unchecked:
		valueName = pcb::chinese("\"״̬δ��֤\""); break;
	case TemplSettingUI::InitFailed:
		valueName = pcb::chinese("\"��ʼ��ʧ��\""); break;
	case TemplSettingUI::Invalid_RoiRect:
		valueName = pcb::chinese("\"�������\""); break;
	case TemplSettingUI::Invalid_RoiData:
		valueName = pcb::chinese("\"����������ݶ�ʧ\""); break;
	case TemplSettingUI::RecognizeFailed:
		valueName = pcb::chinese("\"���ʶ��ʧ��\""); break;
	case TemplSettingUI::Invalid_SerialNum:
		valueName = pcb::chinese("\"��Ʒ���\""); break;
	case TemplSettingUI::Default:
		valueName = pcb::chinese("\"δ֪����\""); break;
	default:
		break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("��Ʒ���ʶ����� \n") +
		pcb::chinese("������Դ��") + valueName + "!        \n" +
		"ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return false;
}