#include "RecheckUI.h"

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

void RecheckUI::reset()
{
	//��վ�̬�ı����е�����
	ui.label_defectIndex->setText("");
	ui.label_defectNum->setText("");
	ui.label_xLoc->setText("");
	ui.label_yLoc->setText("");
	ui.label_modelType->setText("");
	ui.label_detecttionDate->setText("");

	//���õ�ǰ������ʾ��ȱ��Сͼ����ȱ������ָʾ��
	ui.label_indicator1->setPixmap(lightOffIcon); //��·
	ui.label_indicator2->setPixmap(lightOffIcon); //ȱʧ
	ui.label_indicator3->setPixmap(lightOffIcon); //��·
	ui.label_indicator4->setPixmap(lightOffIcon); //͹��

	//�������
	
}

//ˢ�½�������ʾ����Ϣ
void RecheckUI::refresh()
{
	//logging(runtimeParams.serialNum);

	//���½����е�PCB�ͺ�
	ui.label_modelType->setText(runtimeParams->productID.modelType);

	//���ز���ʾPCB��ͼ
	this->loadFullImage();

	//����ȱ��ͼ���������Ϣ
	this->loadFlawInfos();

	//������˸�ļ�ͷ
	defectIndex = 0;
	this->initFlickeringArrow();

	//���ó�������ʾ��ͼ
	ui.graphicsView_full->setScene(&fullImageScene); //���ó���
	ui.graphicsView_full->show(); //��ʾͼ��

	//���ز���ʾ��1��ȱ��Сͼ
	this->showFlawImage(); //��ʾȱ��ͼ

	recheckStatus = NoError;
}


/************** ���ء�ɾ��GraphicView�е�ͼ����Դ *************/

//����PCB��ͼ
void RecheckUI::loadFullImage()
{
	ImageConverter imageConverter; //ͼ��ת����
	QPixmap fullImage;
	imageConverter.set(&detectResult->fullImage, &fullImage, ImageConverter::CvMat2QPixmap);
	imageConverter.start();

	//��ʾȱ������
	defectNum = detectResult->flawInfos.size();
	if (defectNum >= 0) {
		ui.label_defectNum->setText(QString::number(defectNum));
	}

	//ɾ��������֮ǰ���ص�Ԫ��
	QList<QGraphicsItem *> itemList = fullImageScene.items();
	for (int i = 0; i < itemList.size(); i++) {
		fullImageScene.removeItem(itemList[i]);  //��scene���Ƴ�
	}

	//��PCB��ͼ���ص�������
	while (imageConverter.isRunning()) { pcb::delay(1); }
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
	qreal xLoc = (detectResult->flawInfos)[defectIndex].xPos;
	xLoc *= (1.0*fullImageItemSize.width()/originalFullImageSize.width());
	qreal yLoc = (detectResult->flawInfos)[defectIndex].yPos;
	yLoc *= (1.0*fullImageItemSize.height()/originalFullImageSize.height());
	flickeringArrow.setPos(xLoc, yLoc); //���ü�ͷ��λ��
}



/************** ���ء�ɾ��GraphicView�е�ͼ����Դ *************/

//ɾ��graphicView��
void RecheckUI::loadFlawInfos()
{

}

//����ȱ��
void RecheckUI::loadFlawInfos()
{
	//תdefections Qpixmap
	//detectResult->flawInfos[0].flawImage

}

/***************** �л�ȱ��Сͼ ****************/

//�û�С���̵� + -
void RecheckUI::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) 
	{	
	case Qt::Key_Plus: //�л�����ʾ��һ��ȱ��
		qDebug() << "Key_Plus";
		showNextFlawImage(); 
		break;
	case Qt::Key_Minus: //�л�����ʾ��һ��ȱ��
		qDebug() << "Key_Minus";
		showLastFlawImage(); 
		break;
	case Qt::Key_Asterisk: //ֱ����ʾ�˳�ѯ�ʽ���
		qDebug() << "Key_Asterisk";
		flickeringArrow.stopFlickering();
		//showExitQueryUI();
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
		//showExitQueryUI(); //��ʾ�˳�ѯ�ʿ�
	}
	else {
		showFlawImage();
	}
}

//�л�����ʾ��һ��ȱ��ͼ
void RecheckUI::showNextFlawImage()
{
	////��ȡ ��ǰ�� index �ж��Ƿ� ����߽�
	//defectIndex += 1;
	//if (defectIndex > flawImageInfoVec.size() - 1) {  //�߽�
	//	defectIndex -= 1;
	//	qDebug() << "this is the last one";
	//	showExitQueryUI(); //��ʾ�˳�ѯ�ʿ�
	//}
	//else {
	//	showFlawImage();
	//}
}

//��ȱ��ͼ���ز���ʾ����Ӧ��lebal�ؼ���
void RecheckUI::showFlawImage()
{
	//if (defectNum <= 0) return;
	//QFileInfo flawImgInfo(flawImageInfoVec[defectIndex].filePath);
	//if (!flawImgInfo.isFile()) {
	//	recheckStatus = FlawImageNotFound;
	//	this->showMessageBox(MessageBoxType::Warning, recheckStatus);
	//	return;
	//}

	//QImage flawImg(flawImageInfoVec[defectIndex].filePath); //��ͼ
	//flawImg = flawImg.scaled(ui.label_flaw->size(), Qt::KeepAspectRatio); //����
	//QPixmap flawImage(QPixmap::fromImage(flawImg)); //ת��
	//ui.label_flaw->clear(); //���
	//ui.label_flaw->setPixmap(flawImage); //��ʾͼ��

	////��������ȱ����Ϣ
	//ui.label_xLoc->setText(flawImageInfoVec[defectIndex].xPos); //����ȱ�ݵ�x����
	//ui.label_yLoc->setText(flawImageInfoVec[defectIndex].yPos); //����ȱ�ݵ�y����
	ui.label_defectIndex->setText(QString::number(defectIndex + 1)); //��ʾȱ�ݱ��
	this->switchFlawIndicator(); //����ȱ������ͼ��(�޸�ָʾ������״̬)

	//����PCB��ͼ�ϵ�С��ͷ��λ��
	this->setFlickeringArrowPos();
}

//����ȱ�����͵�ָʾͼ��
void RecheckUI::switchFlawIndicator()
{
	//flawIndicatorStatus = pow(2, flawImageInfoVec[defectIndex].flawType.toInt() - 1);

	flawIndicatorStatus = 1;
	ui.label_indicator1->setPixmap(bool((flawIndicatorStatus & 0x1) >> 0) ? lightOnIcon : lightOffIcon); //A
	ui.label_indicator2->setPixmap(bool((flawIndicatorStatus & 0x2) >> 1) ? lightOnIcon : lightOffIcon); //B
	ui.label_indicator3->setPixmap(bool((flawIndicatorStatus & 0x4) >> 2) ? lightOnIcon : lightOffIcon); //C
	ui.label_indicator4->setPixmap(bool((flawIndicatorStatus & 0x8) >> 3) ? lightOnIcon : lightOffIcon); //D
}


/**************** �˳����� *****************/

//������ذ���
void RecheckUI::on_pushButton_return_clicked()
{
	this->reset(); //��ս����ϵļ����
	emit recheckFinished_recheckUI();
}


/******************** ���� *********************/

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
