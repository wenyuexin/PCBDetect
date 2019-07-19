#include "SerialNumberUI.h"

using pcb::RuntimeParams;


SerialNumberUI::SerialNumberUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//��Ա�����ĳ�ʼ��
	maskRoiWidgetsIsVisible = true;
	NumberValidator = Q_NULLPTR;
	errorCode = ErrorCode::Default;
	adminConfig = Q_NULLPTR; //ϵͳ����
	runtimeParams = Q_NULLPTR; //����ʱ����ʱ����
	cvmatSamples = Q_NULLPTR; //���ڼ�������ͼ
	qpixmapSamples = Q_NULLPTR; //������ʾ������ͼ
	imageItem = Q_NULLPTR; 
	gridRowIdx = -1; //��ǰ��ͼ������
	gridColIdx = -1; //��ǰ��ͼ������
	graphicsScenePos.setX(0);//�������Ͻ�����
	graphicsScenePos.setY(0);
	ocrRoi_tl.setX(-1);//roi���Ͻ�
	ocrRoi_tl.setY(-1);
	ocrRoi_br.setX(-1);//roi���½�
	ocrRoi_br.setY(-1);
	imageScalingRatio = 0;//ͼ�����ű���
	ocrHandle = Q_NULLPTR;
}

void SerialNumberUI::init()
{
	//����״̬��ѡ���ڸ���ȫ����ʾ
	this->setGeometry(runtimeParams->ScreenRect);

	//������Ĥ������ؿؼ��Ŀɼ����������ؼ������λ��
	this->initMaskRoiWidgets();

	//��ȡ��ͼ�ؼ�QGraphicsView��λ��
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//���Ʋ��������뷶Χ
	delete NumberValidator;
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
	ui.lineEdit_maskRoi_tl_x->setValidator(NumberValidator); //��Ĥ
	ui.lineEdit_maskRoi_tl_y->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_x->setValidator(NumberValidator);
	ui.lineEdit_maskRoi_br_y->setValidator(NumberValidator);
	ui.lineEdit_ocrRoi_tl_x->setValidator(NumberValidator); //OCR
	ui.lineEdit_ocrRoi_tl_y->setValidator(NumberValidator);
	ui.lineEdit_ocrRoi_br_x->setValidator(NumberValidator);
	ui.lineEdit_ocrRoi_br_y->setValidator(NumberValidator);
	ui.lineEdit_serialNum->setValidator(NumberValidator); //���

	//��ʼ��CheckBox
	this->initCheckBoxGroup();

	//���ü�����ľ۽�����
	//this->setFocusPolicy(Qt::ClickFocus);

	//�������
	this->setMouseTracking(true);
	this->setCursor(Qt::ArrowCursor);

	//���ù�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//��ʼ��OCRģ��
	if (ocrHandle == Q_NULLPTR) { //��ֹ��γ�ʼ��
		ocrHandle = TessBaseAPICreate(); //��ʼ��
		if (TessBaseAPIInit3(ocrHandle, NULL, "eng") != 0) {
			showMessageBox(this, InitFailed); return;
			//die("Error initialising tesseract\n");
		}
	}

	//������Ĥ����������ʼֵ
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



/*************** ����ĳ�ʼ�������á����� **************/

//������Ĥ������ؿؼ��Ŀɼ����������ؼ������λ��
void SerialNumberUI::initMaskRoiWidgets()
{
	if (maskRoiWidgetsIsVisible) return;

	//����Ĥ����������صĿؼ���Ϊ���ɼ�
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

	//���Ҳ�ʣ��Ŀؼ�����
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

//��ʼ��CheckBox ʹ��ͬһʱ��ֻ��һ��box����ѡ
void SerialNumberUI::initCheckBoxGroup()
{
	QPushButton button;
	checkBoxGroup.addButton(&button);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_tl, 1);
	checkBoxGroup.addButton(ui.checkBox_maskRoi_br, 2);
	checkBoxGroup.addButton(ui.checkBox_ocrRoi_tl, 3);
	checkBoxGroup.addButton(ui.checkBox_ocrRoi_br, 4);
}

//�л�����ؼ���״̬
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
	ui.lineEdit_serialNum->setEnabled(enable); //��������
	this->setPushButtonsEnabled(enable); //��������
}

//��������
void SerialNumberUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_getMaskRoi->setEnabled(enable); //ȷ��Mask����
	ui.pushButton_getOcrRoi->setEnabled(enable); //ȷ��COR����
	ui.pushButton_recognize->setEnabled(enable); //ʶ��
	ui.pushButton_confirm->setEnabled(enable); //ȷ��������
	ui.pushButton_return->setEnabled(enable); //����
}

//�������ʶ�����
void SerialNumberUI::reset()
{
	if (this == Q_NULLPTR) return;

	this->setCursor(Qt::ArrowCursor); //�������

	ui.checkBox_maskRoi_tl->setChecked(false);
	ui.checkBox_maskRoi_br->setChecked(false);
	ui.checkBox_ocrRoi_tl->setChecked(false);
	ui.checkBox_ocrRoi_br->setChecked(false);
	//ui.lineEdit_serialNum->setText("");

	//�����Ĥ��������
	ui.lineEdit_maskRoi_tl_x->setText("");
	ui.lineEdit_maskRoi_tl_y->setText("");
	ui.lineEdit_maskRoi_br_x->setText("");
	ui.lineEdit_maskRoi_br_y->setText("");

	//ɾ��ͼԪ
	deleteImageItem();

	//���ò�Ʒ��ŵ�
	runtimeParams->resetSerialNum();

	//ɾ��buffer�ļ����е�roiͼƬ
	QFile file(roiFilePath);
	if (file.exists()) QFile::remove(roiFilePath);
}



/*************** ��ȡ��Ĥ��������� **************/

void SerialNumberUI::on_pushButton_getMaskRoi_clicked()
{
	this->setCursor(Qt::ArrowCursor); //���������ʽ
	if (runtimeParams->DeveloperMode) return;

	//ui.checkBox_maskRoi_tl->setChecked(false);
	//ui.checkBox_maskRoi_br->setChecked(false);
	this->setSerialNumberUIEnabled(false);

	int ImageSize_W = adminConfig->ImageSize_W;
	int ImageSize_H = adminConfig->ImageSize_H;
	int tl_x = 0, tl_y = 0, br_x = 0, br_y = 0;

	//��ȡ��Ĥ��������� - ���Ͻ�
	QString str_tl_x = ui.lineEdit_maskRoi_tl_x->text();
	if (str_tl_x == "") tl_x = -1;
	else tl_x = (int)intervalCensored(str_tl_x.toInt(), 0, ImageSize_W - 1);

	QString str_tl_y = ui.lineEdit_maskRoi_tl_y->text();
	if (str_tl_y == "") tl_y = -1;
	else tl_y = (int)intervalCensored(str_tl_y.toInt(), 0, ImageSize_H - 1);

	//��ȡ��Ĥ��������� - ���½�
	QString str_br_x = ui.lineEdit_maskRoi_br_x->text();
	if (str_br_x == "") br_x = -1;
	else br_x = (int)intervalCensored(str_br_x.toInt(), 0, ImageSize_W - 1);

	QString str_br_y = ui.lineEdit_maskRoi_br_y->text();
	if (str_br_y == "") br_y = -1;
	else br_y = (int)intervalCensored(str_br_y.toInt(), 0, ImageSize_H - 1);

	//����������������в�����
	runtimeParams->maskRoi_tl.setX(tl_x);
	runtimeParams->maskRoi_tl.setY(tl_y);
	runtimeParams->maskRoi_br.setX(br_x);
	runtimeParams->maskRoi_br.setY(br_y);

	emit getMaskRoiFinished_serialNumUI();

	this->setSerialNumberUIEnabled(true);
}



/******************* ������Ӧ *******************/

//��ȡ������Ʒ��ŵ�ROI����
void SerialNumberUI::on_pushButton_getOcrRoi_clicked()
{
	if (runtimeParams->DeveloperMode) return;

	ui.checkBox_ocrRoi_tl->setChecked(false);
	ui.checkBox_ocrRoi_br->setChecked(false);
	this->setSerialNumberUIEnabled(false);

	//��������Χ
	cv::Point tl(ocrRoi_tl.x(), ocrRoi_tl.y());
	cv::Point br(ocrRoi_br.x(), ocrRoi_br.y());
	cv::Rect roiRect = cv::Rect(tl, br);
	if (roiRect.width >= adminConfig->ImageSize_W || 
		roiRect.height >= adminConfig->ImageSize_H) 
	{
		showMessageBox(this, Invalid_RoiRect); 
		this->setSerialNumberUIEnabled(true); return;
	}

	//��������ͼƬ
	cv::Mat roiImg = (*(*cvmatSamples)[gridRowIdx][gridColIdx])(roiRect);
	roiFilePath = runtimeParams->BufferDirPath + "/serialNumRoi.bmp";
	cv::imwrite(roiFilePath.toStdString(), roiImg);

	this->setSerialNumberUIEnabled(true);
}

//ʶ���Ʒ��� - �˴�����OCRģ��
void SerialNumberUI::on_pushButton_recognize_clicked()
{
	if (runtimeParams->DeveloperMode) return;

	//���ò��ֿؼ�
	this->setSerialNumberUIEnabled(false);
	
	//����ͼ��
	PIX *img = pixRead(roiFilePath.toStdString().c_str());
	char *text = NULL;

	//�ַ�ʶ��
	TessBaseAPISetImage2(ocrHandle, img);
	if (TessBaseAPIRecognize(ocrHandle, NULL) != 0) {
		showMessageBox(this, Invalid_RoiData); 
		this->setSerialNumberUIEnabled(true); return;
	}

	//���ת��
	if ((text = TessBaseAPIGetUTF8Text(ocrHandle)) == NULL) {
		showMessageBox(this, RecognizeFailed); 
		this->setSerialNumberUIEnabled(true); return;
	}

	//���Ԥ����
	QString serialNum = QString(text);
	serialNum = serialNum.remove(QRegExp("\\s")); //ɾ���հ��ַ�
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//ɾ����β�ķ������ַ�
	ui.lineEdit_serialNum->setText(serialNum); //��ʾʶ��Ĳ�Ʒ���
	this->setSerialNumberUIEnabled(true); //���ò��ֿؼ�

	//����Ʒ����Ƿ���Ч
	runtimeParams->serialNum = serialNum;
	runtimeParams->resetErrorCode(RuntimeParams::Index_serialNum);//���ô������
	RuntimeParams::ErrorCode code = RuntimeParams::Unchecked;
	code = runtimeParams->checkValidity(RuntimeParams::Index_serialNum);
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); return;
	}
}

//ȷ������
void SerialNumberUI::on_pushButton_confirm_clicked()
{
	this->setPushButtonsEnabled(false); //���ð���

	QString serialNum = ui.lineEdit_serialNum->text(); //��ȡ��Ʒ���
	serialNum = serialNum.remove(QRegExp("\\s")); //ɾ���հ��ַ�
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//ɾ����β�ķ������ַ�
	runtimeParams->serialNum = serialNum;
	runtimeParams->resetErrorCode(RuntimeParams::Index_serialNum);//���ô������

	RuntimeParams::ErrorCode code = RuntimeParams::Unchecked;
	code = runtimeParams->parseSerialNum(); //������Ʒ���
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); 
		this->setPushButtonsEnabled(true); return;
	}
	//����ͺš����κš�����Ƿ���Ч
	code = runtimeParams->checkValidity(RuntimeParams::Index_All_SerialNum);
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); 
		this->setPushButtonsEnabled(true); return;
	}

	//������һ�����棬��ִ����һ������
	emit on_pushButton_return_clicked();

	this->setPushButtonsEnabled(true); //��������

	//����һ�����淢��ʶ��������ź�
	emit recognizeFinished_serialNumUI();
}

//����
void SerialNumberUI::on_pushButton_return_clicked()
{
	emit showPreviousUI_serialNumUI();
}


/**************** ��ʾ��ͼ������ѡ ****************/

//��graphicView����ʾ��ͼ
void SerialNumberUI::showSampleImage(int row, int col)
{
	if (runtimeParams->DeveloperMode) return;

	gridRowIdx = row; //��ǰ��ͼ������
	gridColIdx = col; //��ǰ��ͼ������

	QSize graphicsViewSize = graphicsViewRect.size();
	QPixmap *img = (*qpixmapSamples)[gridRowIdx][gridColIdx];
	QPixmap scaledImg = img->scaled(graphicsViewSize, Qt::KeepAspectRatio);
	imageScalingRatio = 1.0 * scaledImg.width() / img->width();

	//�ڳ����м����µ�ͼԪ
	deleteImageItem(); //ɾ��֮ǰ��ͼԪ
	imageItem = new QGraphicsPixmapItem(scaledImg); //������ͼԪ
	graphicsScene.addItem(imageItem);
	pcb::delay(10);

	//���㳡�����Ͻǵ������
	//QPointF scenePosToView = -(ui.graphicsView->mapToScene(0,0));
	QPointF scenePosToView(graphicsViewSize.width() / 2.0, graphicsViewSize.height() / 2.0);
	graphicsScenePos = scenePosToView - QPointF(scaledImg.width(), scaledImg.height()) / 2.0;

	ui.graphicsView->show();//��ʾ
}

//ɾ��graphicsView����ʾ�ĵ�ͼ��
void SerialNumberUI::deleteImageItem()
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

//��갴���¼�
void SerialNumberUI::mousePressEvent(QMouseEvent *event)
{
	if (runtimeParams->DeveloperMode) return;

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
		this->on_pushButton_getMaskRoi_clicked(); //ȷ������
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
		this->on_pushButton_getMaskRoi_clicked(); //ȷ������
		return;
	}

	//������������ȡ���Ͻǵ� - OCR
	if (event->button() == Qt::LeftButton && ui.checkBox_ocrRoi_tl->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
		relativePos -= graphicsScenePos; //����ڳ���������
		ocrRoi_tl = relativePos / imageScalingRatio; //����ת��
		ocrRoi_tl.setX(intervalCensored(ocrRoi_tl.x(), 0, adminConfig->ImageSize_W - 1));
		ocrRoi_tl.setY(intervalCensored(ocrRoi_tl.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_ocrRoi_tl_x->setText(QString::number((int)ocrRoi_tl.x()));//���½���
		ui.lineEdit_ocrRoi_tl_y->setText(QString::number((int)ocrRoi_tl.y()));
		return;
	}
	
	//������������ȡ���½ǵ� - OCR
	if(ui.checkBox_ocrRoi_br->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
		relativePos -= graphicsScenePos; //����ڳ���������
		ocrRoi_br = relativePos / imageScalingRatio; //����ת��
		ocrRoi_br.setX(intervalCensored(ocrRoi_br.x(), 0, adminConfig->ImageSize_W - 1));
		ocrRoi_br.setY(intervalCensored(ocrRoi_br.y(), 0, adminConfig->ImageSize_H - 1));
		ui.lineEdit_ocrRoi_br_x->setText(QString::number((int)ocrRoi_br.x()));//���½���
		ui.lineEdit_ocrRoi_br_y->setText(QString::number((int)ocrRoi_br.y()));
		return;
	}

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
double SerialNumberUI::intervalCensored(double num, double minVal, double maxVal)
{
	double tmpVal = num;
	if (tmpVal < minVal) tmpVal = minVal;
	if (tmpVal > maxVal) tmpVal = maxVal;
	return tmpVal;
}


//����ƶ��¼� - ��ʱû��
void SerialNumberUI::mouseMoveEvent(QMouseEvent *event)
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

// ��ʱû��
bool SerialNumberUI::isPressPosInGraphicViewRect(QPoint mousePressPos)
{
	QRect selectRect = getRect(mousePressPos, mouseReleasePos);
	return selectRect.contains(mousePressPos);
}

//����beginPoint��endPoint���㵱ǰѡ�еľ��� - ��ʱû��
QRect SerialNumberUI::getRect(const QPoint &beginPoint, const QPoint &endPoint)
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


/********************* ���� ********************/

//��������
bool SerialNumberUI::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == SerialNumberUI::NoError) return false;

	QString valueName;
	switch (tempCode)
	{
	case SerialNumberUI::Unchecked:
		valueName = pcb::chinese("\"״̬δ��֤\""); break;
	case SerialNumberUI::InitFailed:
		valueName = pcb::chinese("\"��ʼ��ʧ��\""); break;
	case SerialNumberUI::Invalid_RoiRect:
		valueName = pcb::chinese("\"�������\""); break;
	case SerialNumberUI::Invalid_RoiData:
		valueName = pcb::chinese("\"����������ݶ�ʧ\""); break;
	case SerialNumberUI::RecognizeFailed:
		valueName = pcb::chinese("\"���ʶ��ʧ��\""); break;
	case SerialNumberUI::Invalid_SerialNum:
		valueName = pcb::chinese("\"��Ʒ���\""); break;
	case SerialNumberUI::Default:
		valueName = pcb::chinese("\"δ֪����\""); break;
	default:
		break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("��Ʒ���ʶ����� \n") +
		pcb::chinese("������Դ��") + valueName + "!        \n" +
		"SerialNumber: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return false;
}