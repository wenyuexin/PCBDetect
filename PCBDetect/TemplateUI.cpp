#include "TemplateUI.h"

using Ui::DetectConfig;
using Ui::DetectParams;


TemplateUI::TemplateUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//���ü�����ľ۽�����
	this->setFocusPolicy(Qt::ClickFocus);

	//ģ����ȡ�߳�
	templExtractor = new TemplateExtractor;
	connect(templExtractor, SIGNAL(sig_extractState_extractor(int)), this, SLOT(update_extractState_extractor(int)));

	templThread = new TemplateThread();
	templThread->setTemplateExtractor(templExtractor);
}

TemplateUI::~TemplateUI()
{
	deletePointersInItemArray(); //ɾ��ͼԪ�����е�ָ��
	deletePointersInSampleImages(); //�Ƴ��������Ѿ����ص�ͼԪ
	delete templThread;
	delete templExtractor;
}


/****************** ���� *********************/

//���ò����ṹ���ָ��
void TemplateUI::setDetectConfig(DetectConfig *ptr) { config = ptr; }

void TemplateUI::setDetectParams(DetectParams *ptr) { params = ptr; }


/***************** ������Ӧ *****************/

//��ȡ
void TemplateUI::on_pushButton_getTempl_clicked()
{
	ui.label_status->setText(QString::fromLocal8Bit("������ȡģ��"));

	//����ͼ���浽template�ļ�����
	// ...
	Ui::delay(1000);

	ui.label_status->setText(QString::fromLocal8Bit("ģ����ȡ����"));
}

//���
void TemplateUI::on_pushButton_clear_clicked()
{
	removeItemsFromGraphicsScene(); //�Ƴ��������Ѿ����ص�ͼԪ
	deletePointersInItemArray();//ɾ��ͼԪ�����е�ָ��
	currentRow_show = -1; //��ʾ�кŵĸ�λ
	params->currentRow_extract = -1; //��ȡ�кŵĸ�λ
	ui.graphicsView->centerOn(0, 0); //��ֱ������λ
	ui.label_status->setText(QString::fromLocal8Bit("�������������"));
}

//����
void TemplateUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}


/****************** ͼ����ʾ *******************/

//�Ի�ͼ�ؼ�GraphicsView�ĳ�ʼ������
void TemplateUI::initGraphicsView()
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
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //��
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //��
			QPointF pos(itemSpacing, itemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		itemGrid.append(posList);
	}

	//itemArray�ĳ�ʼ��
	itemArray.resize(nPhotographing); //���ô�С
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //��
		itemArray[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //��
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//sampleImages�ĳ�ʼ��
	samples.resize(nPhotographing);
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		samples[iPhotographing].resize(nCamera);
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}

	//��ʼ���������ڼ��ӳ�������״̬�ı���
	currentRow_show = -1; //��ʾ�к�
	params->currentRow_extract = -1; //����к�
	eventCounter = 0; //�¼�������

	//������ȡ�߳�
	templExtractor->setDetectParams(params);
	templExtractor->setDetectConfig(config);
	templThread->setDetectParams(params);
	templThread->setDetectConfig(config);
	templThread->setSampleImages(&samples);

	//��ͼ�ؼ�������
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //���þ۽�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView->setScene(&scene); //����ͼ����ӳ���
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //���ô�ֱ��������λ��
}


//��ȡ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void TemplateUI::readSampleImages()
{
	//��ȡ��ӦĿ¼��·��
	QString dirpath = SampleDirPath + "/" + params->sampleModelNum + "/" + params->sampleBatchNum + "/" + params->sampleNum;

	//��ȡĿ¼�µ�����ͼ��
	QDir dir(dirpath);
	dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNummberOfSampleImage(); return; }

	if (params->imageSize.width() < 0) {
		QImage img = QImage(fileList.at(0).absoluteFilePath());//��ͼ
		params->imageSize = img.size();
		templThread->initTemplFunc();
	}

	//��ȡ������ͼ��
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1; //0 1 ... nPhotographing-1
		int iCamera = idxs[1].toInt() - 1; //0 1 ... nCamera-1
		if (iPhotographing != currentRow_show) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //����ͼ��·��
		QImage *img = new QImage(filepath); //��ͼ
		samples[currentRow_show][iCamera] = img;
	}
}


//��ʾ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void TemplateUI::showSampleImages()
{
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		QImage scaledImg = (*samples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(scaledImg)); //����ͼԪ
		item->setPos(itemGrid[currentRow_show][iCamera]); //ͼԪ����ʾλ��
		itemArray[currentRow_show][iCamera] = item; //����ͼ����
	}

	//����������������һ��ͼԪ
	for (int iCamera = 0; iCamera < nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//��ͼ��ʾ����
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //���ô�ֱ��������λ��
	ui.graphicsView->show();//��ʾ
}


//ɾ��ͼԪ�����е�ָ��
void TemplateUI::deletePointersInItemArray()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete itemArray[iPhotographing][iCamera];
			itemArray[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//ɾ������ͼ�������е�ָ��
void TemplateUI::deletePointersInSampleImages()
{
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) {
		for (int iCamera = 0; iCamera < nCamera; iCamera++) {
			delete samples[iPhotographing][iCamera];
			samples[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//�Ƴ��������Ѿ����ص�ͼԪ
void TemplateUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]); 
	}
}

/***************** ��ȡ�ⲿ�ź� ******************/

//��ʱʹ���û����̰���ģ���ⲿ�ź�
void TemplateUI::keyPressEvent(QKeyEvent *event)
{
	params->sampleModelNum = "1"; //�ͺ�
	params->sampleBatchNum = "1"; //���κ�
	params->sampleNum = "2"; //�������

	switch (event->key())
	{
	case Qt::Key_Up:
		qDebug() << "Up";
		break;
	case Qt::Key_Down: //�л�����ʾ��һ��������ͼ
		qDebug() << "Down";
		//if (!isStarted) return; //ϵͳδ������ֱ�ӷ���
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
void TemplateUI::nextRowOfSampleImages()
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
		if (!templThread->isRunning() && eventCounter == 1) {
			extractTemplateImages(); //��ȡ
		}
	}
	else if (params->currentRow_extract == nPhotographing - 1 && !templThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		removeItemsFromGraphicsScene(); //�Ƴ������е�����ͼԪ
		deletePointersInItemArray(); //ɾ��֮ǰ��ͼԪ�����е�ָ��
		deletePointersInSampleImages(); //ɾ������ͼ�������е�ָ��
		ui.graphicsView->centerOn(0, 0); //��ֱ������λ
		currentRow_show = -1; //����ʾ�кŸ�λ
		params->currentRow_extract = -1; //������кŸ�λ

		nextRowOfSampleImages(); //����µ�PCB����ͼ
	}
}


/******************** ģ����ȡ�߳� ********************/

//��ȡ�߳̽�����
//void DetectUI::on_detectFinished_detectThread(bool qualified)
//{
//	//��ʾ���
//	ui.label_indicator->setPixmap((qualified) ? lightOffIcon : lightOnIcon); //�л�ָʾ��
//	ui.label_result->setText((qualified) ? QString::fromLocal8Bit("�ϸ�") : QString::fromLocal8Bit("���ϸ�"));
//}


//��ǰ��һ������ͼ��
void TemplateUI::extractTemplateImages()
{
	params->currentRow_extract += 1;

	ui.label_status->setText(QString::fromLocal8Bit("������ȡ��") +
		QString::number(params->currentRow_extract + 1) + QString::fromLocal8Bit("��ģ��"));//����״̬

	//��������߳�
	templThread->start();
}

void TemplateUI::update_extractState_extractor(int state)
{
	if (state == 1) { //��ȡ��һ��ͼ��

		eventCounter -= 1; //�����¼�������
		ui.label_status->setText(QString::fromLocal8Bit("��") +
			QString::number(params->currentRow_extract + 1) +
			QString::fromLocal8Bit("����ȡ����"));//����״̬��
		qApp->processEvents();

		//����Ƿ���δ������¼�
		while (templThread->isRunning()) Ui::delay(100);
		if (eventCounter > 0) extractTemplateImages();
	}
}
