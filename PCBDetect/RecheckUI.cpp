#include "RecheckUI.h"
#include "ImgConvertThread.h"

//using pcb::MessageBoxType;
//using pcb::FlawImageInfo;


//�������Ĺ��캯��
RecheckUI::RecheckUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//��Ա������ʼ��
	userConfig = Q_NULLPTR; //����ϵͳ������Ϣ
	runtimeParams = Q_NULLPTR; //���в���
	detectResult = Q_NULLPTR; //�����
	IconFolder = "";

}

//���������������
RecheckUI::~RecheckUI()
{
	qDebug() << "~PCBRecheck";
}

/********************* �����ʼ����ˢ�� *********************/

void RecheckUI::init()
{
	//ѡ�����������Ǹ�������ʾ
	this->setGeometry(runtimeParams->ScreenRect);

	//����ͼ��
	IconFolder = QDir::currentPath() + "/icons";
	QPixmap redIcon(IconFolder + "/red.png"); //red
	lightOnIcon = redIcon.scaled(ui.label_indicator1->size(), Qt::KeepAspectRatio);
	QPixmap greyIcon(IconFolder + "/grey.png"); //grey
	lightOffIcon = greyIcon.scaled(ui.label_indicator1->size(), Qt::KeepAspectRatio);

	this->reset();

	//��Ա������ʼ��
	originalFullImageSize = QSize(-1, -1);
	defectNum = -1;
	defectIndex = -1; 

	//graphicsView��������ͼ����ʾ
	ui.graphicsView_full->setFocusPolicy(Qt::NoFocus);
	ui.graphicsView_full->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView_full->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //���ô�ֱ������

	//ˢ����ͼ����˸�ļ�ͷ
	connect(&flickeringArrow, SIGNAL(refreshArrow_arrow()), this, SLOT(on_refreshArrow_arrow()));
}

//������� ���ý���
void RecheckUI::reset()
{
	ui.label_detectDate->setText("");
	ui.label_defectNum->setText("");

	this->deleteItemsFromGraphicScene(); //ɾ��������֮ǰ���ص�Ԫ��
	this->deleteFlawInfos(); //ɾ��ȱ����Ϣ
}

//ˢ�½�������ʾ����Ϣ
void RecheckUI::refresh()
{
	//����ȱ��Сͼ
	this->loadFlawInfos();

	//ˢ�¾�̬�ı���
	ui.label_modelType->setText(runtimeParams->productID.modelType); //�ͺ�
	ui.label_detectDate->setText(detectResult->detectDate.toString("yyyy.MM.dd")); //�������
	if (defectNum >= 0) ui.label_defectNum->setText(QString::number(defectNum)); //ȱ������


	//ˢ��GraphicView
	defectIndex = 0;
	this->deleteItemsFromGraphicScene(); //ɾ��������֮ǰ���ص�Ԫ��
	this->loadFullImage(); //���ز���ʾPCB��ͼ
	this->initFlickeringArrow(); //������˸�ļ�ͷ
	ui.graphicsView_full->setScene(&fullImageScene); //���ó���
	ui.graphicsView_full->show(); //��ʾͼ��

	//��ʾȱ��ͼ
	while (imgConvertThread.isRunning()) { pcb::delay(2); } //�ȴ�ת������
	this->showFlawImage(); //��ʾȱ��ͼ

	recheckStatus = NoError;
}


/************** ���ء�ɾ��GraphicView�е�ͼ����Դ *************/

//��ȡPCB��ͼ�������ص�������
void RecheckUI::loadFullImage()
{
	ImageConverter imageConverter; //ͼ��ת����
	QPixmap fullImage;
	imageConverter.set(&detectResult->fullImage, &fullImage, ImageConverter::CvMat2QPixmap);
	imageConverter.start();

	//��PCB��ͼ���ص�������
	while (imageConverter.isRunning()) { pcb::delay(2); }
	fullImageScene.addPixmap(fullImage); //��ͼ����ؽ�������
	QRect sceneRect = QRect(QPoint(0, 0), fullImageItemSize); //������Χ
	fullImageScene.setSceneRect(sceneRect); //���ó�����Χ
}

//���س�ʼ����˸��ͷ
void RecheckUI::initFlickeringArrow()
{
	if (defectNum <= 0) return;
	flickeringArrow.startFlickering(500); //��ʼ��˸
	setFlickeringArrowPos(); //������˸��ͷ��λ��
	flickeringArrow.setFullImageSize(&fullImageItemSize);
	fullImageScene.addItem(&flickeringArrow); //����ͷ���ؽ�������
}

//ˢ�¼�ʱ����С��ͷ
void RecheckUI::on_refreshArrow_arrow()
{
	flickeringArrow.update(-100, -100, 200, 200);
}

//������˸��ͷ��λ��
void RecheckUI::setFlickeringArrowPos()
{
	qreal xLoc = (detectResult->defectInfos)[defectIndex].xPos;
	xLoc *= (1.0*fullImageItemSize.width()/originalFullImageSize.width());
	qreal yLoc = (detectResult->defectInfos)[defectIndex].yPos;
	yLoc *= (1.0*fullImageItemSize.height()/originalFullImageSize.height());
	flickeringArrow.setPos(xLoc, yLoc); //���ü�ͷ��λ��
}

//ɾ��GraphicScene�е�ͼԪ
void RecheckUI::deleteItemsFromGraphicScene()
{
	QList<QGraphicsItem *> itemList = fullImageScene.items();
	for (int i = 0; i < itemList.size(); i++) {
		fullImageScene.removeItem(itemList[i]);  //��scene���Ƴ�
	}
}


/************** ���ء�ɾ��ȱ��Сͼ *************/

//����ȱ��
void RecheckUI::loadFlawInfos()
{
	defectNum = detectResult->defectInfos.size();

	pcb::CvMatVector cvmatImages;
	cvmatImages.resize(defectNum);
	for (int i = 0; i < defectNum; i++) {
		cv::Mat img = detectResult->defectInfos[i].defectImage;
		cvmatImages[i] = &img;
	}

	//defectImages.resize(defectNum);
	//for (int i = 0; i < defectNum; i++) {
	//	QPixmap *img = new QPixmap();
	//	defectImages[i] = img;
	//}

	imgConvertThread.setCvMats(&cvmatImages);
	imgConvertThread.setQPixmaps(&defectImages);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);
	imgConvertThread.start();
}

//ɾ��ȱ��ͼ
void RecheckUI::deleteFlawInfos() 
{
	//��վ�̬�ı����е�����
	ui.label_defectIndex->setText("");
	ui.label_xLoc->setText("");
	ui.label_yLoc->setText("");
	ui.label_modelType->setText("");

	//���õ�ǰ������ʾ��ȱ��Сͼ����ȱ������ָʾ��
	ui.label_indicator1->setPixmap(lightOffIcon); //��·
	ui.label_indicator2->setPixmap(lightOffIcon); //ȱʧ
	ui.label_indicator3->setPixmap(lightOffIcon); //��·
	ui.label_indicator4->setPixmap(lightOffIcon); //͹��

	//���������ʾ��ȱ��ͼ
	ui.label_flaw->clear(); 
	
	//ɾ��������ʾ��ȱ��Сͼ
	for (int i = 0; i < defectImages.size(); i++) {
		delete defectImages[i];
		defectImages[i] = Q_NULLPTR;
	}
}

/***************** �л�ȱ��Сͼ ****************/

//�û�С���̵� + -
void RecheckUI::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) 
	{	
	case Qt::Key_Space:
	case Qt::Key_Plus: 
	case Qt::Key_Up:
		qDebug() << "========== Key_Plus";
		showNextFlawImage(); //�л�����ʾ��һ��ȱ��
		break;
	case Qt::Key_Minus: 
	case Qt::Key_Down:
		qDebug() << "========== Key_Minus";
		showLastFlawImage(); //�л�����ʾ��һ��ȱ��
		break;
	default:
		break;
	}
}

//����ӺŰ��� - �л�����ʾ��һ��ȱ��
void RecheckUI::on_pushButton_plus2_clicked()
{
	qDebug() << "Button_Plus";
	showNextFlawImage();
}

//������Ű��� - �л�����ʾ��һ��ȱ��
void RecheckUI::on_pushButton_minus2_clicked()
{
	qDebug() << "Button_Minus";
	showLastFlawImage();
}


//�л�����ʾ��һ��ȱ��ͼ
void RecheckUI::showLastFlawImage()
{
	//��ȡ ��ǰ�� index �ж��Ƿ� ����߽�
	defectIndex -= 1;
	if (defectIndex < 0) {  //�߽�
		defectIndex += 1;
		qDebug() << "this is the first one";
	}
	else {
		showFlawImage();
	}
}

//�л�����ʾ��һ��ȱ��ͼ
void RecheckUI::showNextFlawImage()
{
	//��ȡ ��ǰ�� index �ж��Ƿ� ����߽�
	defectIndex += 1;
	if (defectIndex > defectNum - 1) {  //�߽�
		defectIndex -= 1;
		qDebug() << "this is the last one";
	}
	else {
		showFlawImage();
	}
}

//��ȱ��ͼ���ز���ʾ����Ӧ��lebal�ؼ���
void RecheckUI::showFlawImage()
{
	if (defectNum <= 0) return;
	pcb::DefectInfo flawInfo = detectResult->defectInfos[defectIndex];

	QPixmap *defectImage = defectImages[defectIndex]; 
	if (defectImage->size() != ui.label_flaw->size()) {
		defectImage->scaled(ui.label_flaw->size(), Qt::KeepAspectRatio); //����
	}
	ui.label_flaw->clear(); //���������ʾ��ȱ��ͼ
	ui.label_flaw->setPixmap(*defectImage); //��ʾ�µ�ȱ��ͼ

	//��������ȱ����Ϣ
	ui.label_xLoc->setText(QString::number(flawInfo.xPos)); //����ȱ�ݵ�x����
	ui.label_yLoc->setText(QString::number(flawInfo.yPos)); //����ȱ�ݵ�y����
	ui.label_defectIndex->setText(QString::number(defectIndex + 1)); //��ʾȱ�ݱ��
	this->switchFlawIndicator(); //����ȱ������ͼ��(�޸�ָʾ������״̬)
	this->setFlickeringArrowPos(); //����PCB��ͼ�ϵ�С��ͷ��λ��
}

//����ȱ�����͵�ָʾͼ��
void RecheckUI::switchFlawIndicator()
{
	switch (detectResult->defectInfos[defectIndex].defectType)
	{
	case 1:
		ui.label_indicator1->setPixmap(lightOnIcon); break; //��·
	case 2:
		ui.label_indicator2->setPixmap(lightOnIcon); break; //��·
	case 3:
		ui.label_indicator3->setPixmap(lightOnIcon); break; //͹��
	case 4:
		ui.label_indicator4->setPixmap(lightOnIcon); break; //ȱʧ
	default:
		break;
	}
}


/****************** ���� *******************/

//������ذ���
void RecheckUI::on_pushButton_return_clicked()
{
	this->reset(); //��ս����ϵļ����
	emit recheckFinished_recheckUI();
}


/****************** ���� *******************/

//���ð���
void RecheckUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_plus2->setEnabled(enable);
	ui.pushButton_minus2->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}

//��������
//void PCBRecheck::showMessageBox(MessageBoxType boxType, RecheckStatus status)
//{
//	RecheckStatus tempStatus = (status == Default) ? recheckStatus : status;
//	if (tempStatus == NoError) return;
//
//	QString message = "";
//	switch (status)
//	{
//	case PCBRecheck::Unchecked:
//		message = pcb::chinese("ϵͳ״̬δ֪!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::CurrentBatchRechecked:
//		message = pcb::chinese("�����ε����������Ѿ��������!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::InvalidFullImageName:
//		message = pcb::chinese("PCB��ͼ�ļ����ļ�����Ч!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::FullImageNotFound:
//		message = pcb::chinese("û���ҵ�PCB��ͼ!  \n");
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::LoadFullImageFailed:
//		message = pcb::chinese("�޷���PCB��ͼ!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::FlawImageNotFound:
//		message = pcb::chinese("û���ҵ�PCBȱ��ͼ!  \n");
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::LoadFlawImageFailed:
//		message = pcb::chinese("�޷�����Ӧ��ȱ��ͼ!  \n"); 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::OpenFlawImageFolderFailed:
//		message = pcb::chinese("·����λʧ�ܣ��޷���ȡ��Ӧ�ļ����!\n"); 
//		//message += QString("path: ./output" + runtimeParams.getRelativeFolderPath()) + "\n"; 
//		message += "Recheck: Main: ErrorCode: " + QString::number(tempStatus); break;
//	case PCBRecheck::Default:
//		break;
//	}
//
//	//��ʾ����
//	//MyMessageBox messageBox;
//	//messageBox.set(boxType, message);
//	//messageBox.doShow();
//	//pcb::delay(10);//��ʱ
//}

//�����־
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
