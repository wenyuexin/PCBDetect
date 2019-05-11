#include "SerialNumberUI.h"

using pcb::RuntimeParams;


SerialNumberUI::SerialNumberUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//��Ա�����ĳ�ʼ��
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
	roiRect_tl.setX(0);//roi���Ͻ�
	roiRect_tl.setY(0);
	roiRect_br.setX(0);//roi���½�
	roiRect_br.setY(0);
	imageScalingRatio = 0;//ͼ�����ű���
	ocrHandle = Q_NULLPTR;
}

void SerialNumberUI::init()
{
	//����״̬��ѡ���ڸ���ȫ����ʾ
	this->setGeometry(runtimeParams->screenRect);

	//��ȡ��ͼ�ؼ�QGraphicsView��λ��
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//�����ʼ��
	this->initSerialNumberUI();
}

SerialNumberUI::~SerialNumberUI()
{
	qDebug() << "~SerialNumberUI";
	deleteImageItem();
}


/*************** ����ĳ�ʼ�������á����� **************/

//�����ʼ��
void SerialNumberUI::initSerialNumberUI()
{
	//���Ʋ��������뷶Χ
	QIntValidator intValidator;
	ui.lineEdit_roi_tl_x->setValidator(&intValidator);
	ui.lineEdit_roi_tl_y->setValidator(&intValidator);
	ui.lineEdit_roi_br_x->setValidator(&intValidator);
	ui.lineEdit_roi_br_y->setValidator(&intValidator);
	ui.lineEdit_serialNum->setValidator(&intValidator);

	//��ʼ��CheckBox
	this->initCheckBoxGroup();

	//���ü�����ľ۽�����
	//this->setFocusPolicy(Qt::ClickFocus);

	//������
	this->setMouseTracking(true);

	//���ù�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//��ʼ��OCRģ��
	ocrHandle = TessBaseAPICreate();
	if (TessBaseAPIInit3(ocrHandle, NULL, "eng") != 0) {
		showMessageBox(this, InitFailed); return;
		//die("Error initialising tesseract\n");
	}
}

//��ʼ��CheckBox ʹ��ͬһʱ��ֻ��һ��box����ѡ
void SerialNumberUI::initCheckBoxGroup()
{
	QPushButton button;
	checkBoxGroup.addButton(&button);
	checkBoxGroup.addButton(ui.checkBox_roi_tl, 1);
	checkBoxGroup.addButton(ui.checkBox_roi_br, 2);
}

//�л�����ؼ���״̬
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

//�������ʶ�����
void SerialNumberUI::resetSerialNumberUI()
{
	ui.checkBox_roi_tl->setChecked(true);
	ui.checkBox_roi_tl->setChecked(false);
	ui.checkBox_roi_br->setChecked(false);
	ui.lineEdit_serialNum->setText("");

	//ɾ��ͼԪ
	deleteImageItem();

	//���ò�Ʒ��ŵ�
	runtimeParams->resetSerialNum();

	//ɾ��buffer�ļ����е�roiͼƬ
	QFile file(roiFilePath);
	if (file.exists()) QFile::remove(roiFilePath);
}

/******************* ������Ӧ *******************/

//��ȡ������Ʒ��ŵ�ROI����
void SerialNumberUI::on_pushButton_getROI_clicked()
{
	ui.checkBox_roi_tl->setChecked(false);
	ui.checkBox_roi_br->setChecked(false);
	this->setSerialNumberUIEnabled(false);

	//��������Χ
	cv::Point tl(roiRect_tl.x(), roiRect_tl.y());
	cv::Point br(roiRect_br.x(), roiRect_br.y());
	cv::Rect roiRect = cv::Rect(tl, br);
	if (roiRect.width >= adminConfig->ImageSize_W || 
		roiRect.height >= adminConfig->ImageSize_H) 
	{
		showMessageBox(this, Invalid_RoiRect); return;
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
	this->setSerialNumberUIEnabled(false);
	PIX *img = pixRead(roiFilePath.toStdString().c_str());
	char *text = NULL;

	//����ͼ��
	TessBaseAPISetImage2(ocrHandle, img);
	if (TessBaseAPIRecognize(ocrHandle, NULL) != 0) {
		showMessageBox(this, Invalid_RoiData); return;
		//die("Error in Tesseract recognition\n");
	}

	//ʶ���Ʒ���
	if ((text = TessBaseAPIGetUTF8Text(ocrHandle)) == NULL) {
		showMessageBox(this, RecognizeFailed); return;
		//die("Error getting text\n");
	}

	QString serialNum = QString(text);
	serialNum = serialNum.remove(QRegExp("\\s")); //ɾ���հ��ַ�
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//ɾ����β�ķ������ַ�
	ui.lineEdit_serialNum->setText(serialNum); //��ʾʶ��Ĳ�Ʒ���
	this->setSerialNumberUIEnabled(true);

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
	QString serialNum = ui.lineEdit_serialNum->text(); //��ȡ��Ʒ���
	serialNum = serialNum.remove(QRegExp("\\s")); //ɾ���հ��ַ�
	serialNum = pcb::eraseNonDigitalCharInHeadAndTail(serialNum);//ɾ����β�ķ������ַ�
	runtimeParams->serialNum = serialNum;
	runtimeParams->resetErrorCode(RuntimeParams::Index_serialNum);//���ô������

	RuntimeParams::ErrorCode code = RuntimeParams::Unchecked;
	code = runtimeParams->parseSerialNum(); //������Ʒ���
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); return;
	}
	//����ͺš����κš�����Ƿ���Ч
	code = runtimeParams->checkValidity(RuntimeParams::Index_All_SerialNum);
	if (code != RuntimeParams::ValidValue) {
		runtimeParams->showMessageBox(this, code); return;
	}

	//������һ�����棬��ִ����һ������
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

	//���㳡�����Ͻǵ������
	QPointF scenePosToView = -(ui.graphicsView->mapToScene(0,0));
	graphicsScenePos = scenePosToView - QPointF(scaledImg.width()/2.0, scaledImg.height()/2.0);

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
	//������������ȡ���Ͻǵ�
	if (event->button() == Qt::LeftButton && ui.checkBox_roi_tl->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
		relativePos -= graphicsScenePos; //����ڳ���������
		roiRect_tl = relativePos / imageScalingRatio; //����ת��
		ui.lineEdit_roi_tl_x->setText(QString::number((int)roiRect_tl.x()));//���½���
		ui.lineEdit_roi_tl_y->setText(QString::number((int)roiRect_tl.y()));
		return;
	}
	
	//������������ȡ���½ǵ�
	if(ui.checkBox_roi_br->isChecked()) { 
		if (!graphicsViewRect.contains(event->pos())) return; //�����ж�
		QPointF relativePos = event->pos() - graphicsViewRect.topLeft();//�������ͼ������
		relativePos -= graphicsScenePos; //����ڳ���������
		roiRect_br = relativePos / imageScalingRatio; //����ת��
		ui.lineEdit_roi_br_x->setText(QString::number((int)roiRect_br.x()));//���½���
		ui.lineEdit_roi_br_y->setText(QString::number((int)roiRect_br.y()));
		return;
	}

	//ͨ���Ͼ��ο�ķ�ʽѡ������
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

// ����beginPoint , endPoint ��ȡ��ǰѡ�еľ��� - ��ʱû��
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