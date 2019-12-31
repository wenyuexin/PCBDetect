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
	//delete sysInitThread;
	//delete serialNumberUI;
	//delete exitQueryUI;
}

/********************* �����ʼ�� *********************/

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

	//�ı���ȵı���ɫ
	//QPalette palette;
	//palette.setColor(QPalette::Background, QColor(250, 250, 250));
	//ui.label_flaw->setAutoFillBackground(true); //ȱ��ͼ
	//ui.label_flaw->setPalette(palette);
	//ui.label_xLoc->setAutoFillBackground(true); //ȱ��λ��x
	//ui.label_xLoc->setPalette(palette);
	//ui.label_yLoc->setAutoFillBackground(true); //ȱ��λ��y
	//ui.label_yLoc->setPalette(palette);
	//ui.label_modelType->setAutoFillBackground(true); //�������
	//ui.label_modelType->setPalette(palette);

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
}

/******************* ��ʼ���߳� *******************/

////���û�������Ч
//void PCBRecheck::on_userConfigError_initThread()
//{
//	userConfig.showMessageBox(this); //������ʾ
//	pcb::delay(10); //��ʱ
//	this->exitRecheckSystem(); //�˳�ϵͳ
//}


//void PCBRecheck::on_outFolderHierarchyError_initThread()
//{
//	QString message = pcb::chinese("�޴��������������Ƚ��м��!  ");
//
//	MyMessageBox messageBox;
//	messageBox.set(pcb::MessageBoxType::Information, message);
//	messageBox.doShow();//��ʾ������Ϣ����
//
//	pcb::delay(10); //��ʱ
//	this->exitRecheckSystem(); //�˳�ϵͳ
//}

//ϵͳ��ʼ������
//void PCBRecheck::on_sysInitFinished_initThread()
//{
//	//�����Ŀ¼�µ��ļ��в�δ��ݸ�������ý���
//	serialNumberUI->setFolderHierarchy(&OutFolderHierarchy);
//
//	//��ʾPCB���ѯ�ʽ���
//	this->showSerialNumberUI();
//}


/******************* �˳�ѯ�ʽ��� *****************/

//��ʾ�˳�ѯ�ʽ���
//void PCBRecheck::showExitQueryUI()
//{
//	this->setPushButtonsEnabled(false);
//	pcb::delay(5); //��ʱ
//	exitQueryUI->show(); //�����˳�ѯ�ʿ�
//	flickeringArrow.stopFlickering();
//}

//�����˳�ѯ�ʽ��棬����ʾ������ý���
//void PCBRecheck::do_showSerialNumberUI_exitUI()
//{
//	exitQueryUI->hide(); //�����˳�ѯ�ʽ���
//	pcb::delay(5); //��ʱ
//	
//	if (!serialNumberUI->getNextSerialNum()) { //��ȡͬ���ε���һ����Ʒ���
//		recheckStatus = CurrentBatchRechecked; //��ʾ�������Ѿ�������
//		this->showMessageBox(MessageBoxType::Information, recheckStatus);
//	}
//	
//	showSerialNumberUI(); //��ʾ������ý���
//	pcb::delay(10); //��ʱ
//	this->hide(); //����������
//}

//���˳����淵��������
//void PCBRecheck::do_showRecheckMainUI_exitUI()
//{
//	exitQueryUI->hide();
//	this->setPushButtonsEnabled(true);
//	flickeringArrow.startFlickering(500);
//}


/********* ��ʼ��ʾ������PCB��ͼ����1��ȱ��Сͼ�� ********/

//ˢ�¼�ʱ����С��ͷ
void RecheckUI::on_refreshArrow_arrow()
{
	flickeringArrow.update(-100, -100, 200, 200);
}

//���½�������ʾ����Ϣ
void RecheckUI::refresh()
{
	//logging(runtimeParams.serialNum);

	//���½����е�PCB�ͺ�
	ui.label_modelType->setText(runtimeParams->productID.modelType);

	////��������ڵ��ļ���
	//QString flawImageFolderPath = userConfig.OutputDirPath + "/"
	//	+ runtimeParams.getRelativeFolderPath();
	////�жϲ�Ʒ��Ŷ�Ӧ�ļ����Ƿ����
	//if (!QFileInfo(flawImageFolderPath).isDir()) {
	//	recheckStatus = OpenFlawImageFolderFailed;
	//	this->showMessageBox(MessageBoxType::Warning, recheckStatus);
	//	this->showSerialNumberUI(); //��ʾPCB���ѯ�ʽ���
	//	return;
	//}

	//���ز���ʾPCB��ͼ
	//if (!loadFullImage()) return;
	this->showFullImage();

	////��ȡ�ļ����ڵ�ͼƬ·��
	//getFlawImageInfo(flawImageFolderPath);

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

//����PCB��ͼ
//bool RecheckUI::loadFullImage()
//{
//	QString flawImageDirPath = userConfig.OutputDirPath + "/"
//		+ runtimeParams->sampleModelNum + "/"
//		+ runtimeParams.sampleBatchNum + "/"
//		+ runtimeParams.sampleNum; //��������ڵ��ļ���
//
//	QDir dir(flawImageDirPath + "/fullImage/");
//	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//	QStringList filters("*" + userConfig.ImageFormat); //���ض���ʽ��ͼƬ���˳��� 
//	dir.setNameFilters(filters);
//	QFileInfoList folder_list = dir.entryInfoList(); //��ȡ�ļ��б�
//
//	QString fullImagePath = "";
//	QStringList pcbInfoList;//��ͼ��ԭͼ��С+ȱ������
//	for (int i = 0; i < folder_list.size(); i++) {
//		if (folder_list.at(i).baseName().startsWith(fullImageNamePrefix)) {
//			fullImagePath = folder_list.at(i).absoluteFilePath();
//
//			//�����ļ�����ȡPCB��ͼ��ԭʼ�ߴ��ȱ������
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
//				//serialNumberUI->show(); //��ʾPCB���ѯ�ʽ���
//				return false;
//			}
//		}
//	}
//
//	//�ж��Ƿ��ҵ�fullImageΪǰ׺��ImageFormat��ʽ��ͼ
//	if (fullImagePath == "") {
//		//logging("FullImageNotFound");
//		recheckStatus = FullImageNotFound;
//		//this->showMessageBox(MessageBoxType::Warning, recheckStatus);
//		//serialNumberUI->show(); //��ʾPCB���ѯ�ʽ���
//		return false;
//	}
//
//	//���ش�ͼ
//	QImage fullImg; //��ͼ
//	if (!fullImg.load(fullImagePath)) {
//		//logging("LoadFullImageFailed: fullImagePath: " + fullImagePath);
//		recheckStatus = LoadFullImageFailed;
//		//this->showMessageBox(MessageBoxType::Warning, recheckStatus);
//		//serialNumberUI->show(); //��ʾPCB���ѯ�ʽ���
//		return false;
//	}
//
//	scaledFactor = qMin(qreal(ui.graphicsView_full->height() - 2) / fullImg.size().height(),
//		qreal(ui.graphicsView_full->width() - 2) / fullImg.size().width());//��ͼ�ĳߴ�任����
//	fullImg = fullImg.scaled(fullImg.size()*scaledFactor, Qt::KeepAspectRatio); //����
//	fullImage = QPixmap::fromImage(fullImg); //ת��
//	fullImageItemSize = fullImage.size(); //PCB��ͼ��ʵ����ʾ�ߴ�
//	return true;
//}

//���ز���ʾPCB��ͼ
//�Ӷ�Ӧ��output�ļ��е�fullImage���ļ����ж�ȡ��ͼ
void RecheckUI::showFullImage()
{
	//��ʾȱ������
	if (defectNum >= 0) {
		ui.label_defectNum->setText(QString::number(defectNum));
	}

	//ɾ��������֮ǰ���ص�Ԫ��
	QList<QGraphicsItem *> itemList = fullImageScene.items();
	for (int i = 0; i < itemList.size(); i++) {
		fullImageScene.removeItem(itemList[i]);  //��scene���Ƴ�
	}

	//��PCB��ͼ���ص�������
	fullImageScene.addPixmap(fullImage); //��ͼ����ؽ�������
	QRect sceneRect = QRect(QPoint(0, 0), fullImageItemSize); //������Χ
	fullImageScene.setSceneRect(sceneRect); //���ó�����Χ
}


// ��ȡ��ǰ��Ŷ�Ӧ������ȱ��ͼƬ�����ݻ��浽�ڴ��У�
// �����л�ȱ��ͼʱ��ֻ��������ж�ȡ���ݼ���
//void PCBRecheck::getFlawImageInfo(QString dirpath)
//{
//	QDir dir(dirpath);
//	dir.setSorting(QDir::Name | QDir::Time | QDir::Reversed);
//	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//
//	QStringList filters("*" + userConfig.ImageFormat); //���ض���ʽ��ͼƬ���˳��� 
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
//		flawImageInfo.flawIndex = QString::number(index); //ȱ�ݱ��
//		flawImageInfo.xPos = list.at(1); //x����
//		flawImageInfo.yPos = list.at(2); //y����
//		flawImageInfo.flawType = list.at(3); //ȱ������
//		flawImageInfoVec[index] = flawImageInfo;
//	}
//}

//���س�ʼ����˸��ͷ
void RecheckUI::initFlickeringArrow()
{
	if (defectNum <= 0) return;
	flickeringArrow.startFlickering(500); //��ʼ��˸
	setFlickeringArrowPos(); //������˸��ͷ��λ��
	flickeringArrow.setFullImageSize(&fullImageItemSize);
	fullImageScene.addItem(&flickeringArrow); //����ͷ���ؽ�������
}

//������˸��ͷ��λ��
void RecheckUI::setFlickeringArrowPos()
{
	//qreal xLoc = flawImageInfoVec[defectIndex].xPos.toDouble();
	//xLoc *= (1.0*fullImageItemSize.width()/originalFullImageSize.width());
	//qreal yLoc = flawImageInfoVec[defectIndex].yPos.toDouble();
	//yLoc *= (1.0*fullImageItemSize.height()/originalFullImageSize.height());
	//flickeringArrow.setPos(xLoc, yLoc); //���ü�ͷ��λ��

	qreal xLoc = 0;
	qreal yLoc = 0;
	flickeringArrow.setPos(xLoc, yLoc); //���ü�ͷ��λ��
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
