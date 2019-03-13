#include "DetectUI.h"
#include <time.h>

using Ui::DetectConfig;
using Ui::DetectParams;


DetectUI::DetectUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//���ü�����ľ۽�����
	this->setFocusPolicy(Qt::ClickFocus);
	
	//����ϵͳ����״̬
	isStarted = false;

	//���ز���ʾĬ�ϵ�ָʾ��ͼ��
	IconFolder = QDir::currentPath() + "/Icons";
	QImage greyIcon = QImage(IconFolder + "/grey.png"); //grey
	QPixmap defaultIcon = QPixmap::fromImage(greyIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio));
	ui.label_indicator->setPixmap(defaultIcon); //����
	
	//��������ָʾ��ͼ��
	QImage redIcon = QImage(IconFolder + "/red.png"); //red
	lightOnIcon = QPixmap::fromImage(redIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio));
	QImage greenIcon = QImage(IconFolder + "/green.png"); //grey
	lightOffIcon = QPixmap::fromImage(greenIcon.scaled(ui.label_indicator->size(), Qt::KeepAspectRatio));

	//����̵߳��ź�����
	detectCore = new DetectCore;
	connect(detectCore, SIGNAL(sig_detectState_detectCore(int)), this, SLOT(update_detectState_detectCore(int)));

	detectThread = new DetectThread;
	detectThread->setDetectCore(detectCore);
}


DetectUI::~DetectUI()
{
	deletePointersInItemArray(); //ɾ��ͼԪ�����е�ָ��
	deletePointersInSampleImages(); //ɾ������ͼ�������е�ָ��
	delete detectThread; //ɾ������߳�
	delete detectCore; //ɾ��������
}


/******************** ���� **********************/

void DetectUI::setDetectConfig(DetectConfig *ptr) { config = ptr; }

void DetectUI::setDetectParams(DetectParams *ptr) { params = ptr; }


/****************** ������Ӧ *******************/

//���
void DetectUI::on_pushButton_start_clicked()
{
	isStarted = !isStarted; //״̬��ת
	if (isStarted) {
		ui.pushButton_return->setEnabled(false); //���÷��ؼ�
		ui.pushButton_start->setText(QString::fromLocal8Bit("��ͣ"));
		ui.label_status->setText(QString::fromLocal8Bit("ϵͳ������"));
	}
	else {
		ui.pushButton_return->setEnabled(true); //���÷��ؼ�
		ui.pushButton_start->setText(QString::fromLocal8Bit("��ʼ"));
		ui.label_status->setText(QString::fromLocal8Bit("ϵͳ��ͣ��"));
	}
}

//����
void DetectUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}


/**************** ͼ��ļ�������ʾ *****************/

//�Ի�ͼ�ؼ�GraphicsView�ĳ�ʼ������
void DetectUI::initGraphicsView()
{
	//��������
	itemSpacing = 3; //ͼԪ���
	nCamera = config->nCamera; //�������
	nPhotographing = config->nPhotographing; //�������
	SampleDirPath = config->SampleDirPath; //sample�ļ��е�·�� 
	//QSize imageSize = config->imageSize; //ԭͼ�ߴ�

	//�����ܼ��
	QSize totalSpacing; //�ܼ��
	totalSpacing.setWidth(itemSpacing * (nCamera + 1)); //����ܿ��
	totalSpacing.setHeight(itemSpacing * (nPhotographing + 1)); //����ܸ߶�

	//����ͼԪ�ߴ�
	QSize viewSize = ui.graphicsView->size(); //��ͼ�ߴ�
	itemSize.setWidth(int((viewSize.width() - totalSpacing.width()) / nCamera)); //ͼԪ���
	//qreal itemAspectRatio = qreal(imageSize.width()) / imageSize.height(); //��߱�
	qreal itemAspectRatio = config->imageAspectRatio; //��߱�
	itemSize.setHeight(int(itemSize.width() / itemAspectRatio)); //ͼԪ�߶�

	//���㳡���ߴ�
	sceneSize = totalSpacing; 
	sceneSize += QSize(itemSize.width()*nCamera, itemSize.height()*nPhotographing);
	scene.setSceneRect(0, 0, sceneSize.width(), sceneSize.height());

	//���ɻ�ͼ����
	QSize spacingBlock = QSize(itemSpacing, itemSpacing);
	QSize gridSize = itemSize + spacingBlock;
	for (int iPhotographing=0; iPhotographing<nPhotographing; iPhotographing++) { //��
		QList<QPointF> posList;
		for (int iCamera=0; iCamera<nCamera; iCamera++) { //��
			QPointF pos(itemSpacing, itemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		itemGrid.append(posList);
	}

	//itemArray�ĳ�ʼ��
	itemArray.resize(nPhotographing); //���ô�С
	for (int iPhotographing=0; iPhotographing<nPhotographing; iPhotographing++) { //��
		itemArray[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //��
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//sampleImages�ĳ�ʼ��
	samples.resize(nPhotographing);
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //��
		samples[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//��ʼ���������ڼ��ӳ�������״̬�ı���
	currentRow_show = -1; //��ʾ�к�
	params->currentRow_detect = -1; //����к�
	eventCounter = 0; //�¼�������

	//��ʼ����ͼ�ؼ�
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //���þ۽�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView->setScene(&scene); //����ͼ����ӳ���
	ui.graphicsView->centerOn(sceneSize.width()/2, 0); //���ô�ֱ��������λ��

	//����detectThread��detectCore
	detectCore->setDetectConfig(config);
	detectCore->setDetectParams(params);
	detectCore->setDetectResult(&detectResult); //���ô洢������Ľṹ��
	detectThread->setSampleImages(&samples);
	detectThread->setDetectParams(params);
}


//��ȡ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void DetectUI::readSampleImages()
{
	double t1 = clock();

	//��ȡ��ӦĿ¼��·��
	QString dirpath = SampleDirPath + "/" + params->sampleModelNum + "/" + params->sampleBatchNum + "/" + params->sampleNum;

	//��ȡĿ¼�µ�����ͼ��
	QDir dir(dirpath);
	//dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNummberOfSampleImage(); return; }

	if (params->imageSize.height() <= 0) {
		QImage img = QImage(fileList.at(0).absoluteFilePath());//��ͼ
		params->imageSize = img.size();
	}

	//��ȡ����ͼ��浽itemArray��sampleImages��
	double loadtime = 0;
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1;
		int iCamera = idxs[1].toInt() - 1;
		if (iPhotographing != currentRow_show) continue;

		/*
		int num = fileList.at(i).baseName().toInt(); //ͼƬ�ı��
		int iCamera = (num - 1) % nCamera; //0 1 ... nCamera-1
		int iPhotographing = (num - 1) / nCamera; //0 1 ... nPhotographing-1
		if (iPhotographing != currentRow) continue;
		*/

		QString filepath = fileList.at(i).absoluteFilePath(); //����ͼ��·��
		double t11 = clock();
		QImage *img = new QImage(filepath);//��ͼ
		double t22 = clock();
		loadtime += t22 - t11;

		samples[currentRow_show][iCamera] = img;
	}
	double t2 = clock();
	//qDebug() << "readSampleImages : " << (t2 - t1) << "ms   ( loadtime :" << loadtime << "ms )";
	qDebug() << "readSampleImages : " << (t2 - t1) << "ms  ( currentRow_show -" << currentRow_show << ")";
}


//��ʾ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void DetectUI::showSampleImages()
{
	//����ͼԪ
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		QImage imgScaled = (*samples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(imgScaled)); //����ͼԪ
		item->setPos(itemGrid[currentRow_show][iCamera]); //ͼԪ����ʾλ��
		itemArray[currentRow_show][iCamera] = item; //����ͼ����
	}

	//����������������һ��ͼԪ
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//������ʾ
	ui.graphicsView->show(); //��ʾͼ��

	//���ô�ֱ��������λ��
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height()/2;
	ui.graphicsView->centerOn(sceneSize.width()/2, y_SliderPos);
}


//ɾ��ͼԪ�����е�ָ��
void DetectUI::deletePointersInItemArray()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete itemArray[iPhotographing][iCamera];
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//ɾ������ͼ�������е�ָ��
void DetectUI::deletePointersInSampleImages()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { 
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete samples[iPhotographing][iCamera];
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//�Ƴ������е�����ͼԪ
void DetectUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);  
	}
}


/***************** ��ȡ�ⲿ�ź� ******************/

//��ʱʹ���û����̰���ģ���ⲿ�ź�
void DetectUI::keyPressEvent(QKeyEvent *event)
{
	params->sampleModelNum = "1"; //�ͺ�
	params->sampleBatchNum = "1"; //���κ�
	params->sampleNum = "3"; //�������

	switch (event->key())
	{
	case Qt::Key_Up:
		qDebug() << "Up";
		break;
	case Qt::Key_Down: //�л�����ʾ��һ��������ͼ
		qDebug() << "Down";
		if (!isStarted) return; //ϵͳδ������ֱ�ӷ���
		nextRowOfSampleImages(); 
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << "Enter";
		break;
	default:
		break;
	}
}


//�ڻ�ͼ��������ʾ��һ��ͼ��
void DetectUI::nextRowOfSampleImages()
{
	if (currentRow_show + 1 < nPhotographing) { //ֱ����ʾ�µ�������
		currentRow_show += 1; //������ʾ�к�
		eventCounter += 1; //�����¼�������
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		readSampleImages(); //��ͼ
		double t2 = clock();
		showSampleImages(); //������ʾ
		double t3 = clock();

		//qDebug() << "readSampleImages :" << (t2 - t1) << "ms";
		qDebug() << "showSampleImages :" << (t3 - t2) << "ms ( currentRow_show -" << currentRow_show << ")";

		//�ж��Ƿ�ִ�м�����
		if (!detectThread->isRunning() && eventCounter == 1) {
			detectSampleImages(); //ִ�м��
		}
	}
	else if (params->currentRow_detect == nPhotographing - 1 && !detectThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;
		//params->sampleNum = QString::number(params->sampleNum.toInt() + 1); //�������

		removeItemsFromGraphicsScene(); //�Ƴ������е�����ͼԪ
		deletePointersInItemArray(); //ɾ��֮ǰ��ͼԪ�����е�ָ��
		deletePointersInSampleImages(); //ɾ������ͼ�������е�ָ��
		ui.graphicsView->centerOn(0, 0); //��ֱ������λ
		currentRow_show = -1; //����ʾ�кŸ�λ
		params->currentRow_detect = -1; //������кŸ�λ

		nextRowOfSampleImages(); //����µ�PCB����ͼ
	}
}


/******************** ����߳� ********************/

//����߳̽�����
void DetectUI::on_detectFinished_detectThread(bool qualified)
{
	//��ʾ���
	ui.label_indicator->setPixmap((qualified) ? lightOffIcon : lightOnIcon); //�л�ָʾ��
	ui.label_result->setText((qualified) ? QString::fromLocal8Bit("�ϸ�") : QString::fromLocal8Bit("���ϸ�"));
}

//��⵱ǰ��һ������ͼ��
void DetectUI::detectSampleImages()
{
	params->currentRow_detect += 1; //���¼���к�
	qDebug() << "currentRow_detect  - " << params->currentRow_detect;

	ui.label_status->setText(QString::fromLocal8Bit("���ڼ���") +
		QString::number(params->currentRow_detect + 1) + QString::fromLocal8Bit("��ͼ��"));//����״̬

	//��������߳�
	detectThread->start();
}


/***************** ����״̬ ******************/

void DetectUI::update_detectState_detectCore(int state)
{
	if (state == 2) { //�����һ��ͼ��currentRow_detect

		eventCounter -= 1; //�����¼�������
		ui.label_status->setText(QString::fromLocal8Bit("��") +
			QString::number(params->currentRow_detect + 1) +
			QString::fromLocal8Bit("�м�����"));//����״̬��
		qApp->processEvents();

		//����Ƿ���δ������¼�
		while (detectThread->isRunning()) Ui::delay(100);
		if (eventCounter > 0) detectSampleImages();
	}
}
