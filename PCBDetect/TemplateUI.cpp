#include "TemplateUI.h"

using Ui::DetectConfig;
using Ui::DetectParams;
using cv::Mat;


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

	//��Ʒ���ʶ�����
	connect(&serialNumberUI, SIGNAL(recognizeFinished_serialNumberUI()), this, SLOT(on_recognizeFinished_serialNumberUI()));
	connect(&serialNumberUI, SIGNAL(switchImage_serialNumberUI()), this, SLOT(on_switchImage_serialNumberUI()));

	//ģ����ȡ�߳�
	templExtractor = new TemplateExtractor;
	connect(templExtractor, SIGNAL(extractState_extractor(int)), this, SLOT(update_extractState_extractor(int)));

	templThread = new TemplateThread();
	templThread->setTemplateExtractor(templExtractor);
}

TemplateUI::~TemplateUI()
{
	deletePointersInItemArray(itemArray); //ɾ��ͼԪ�����е�ָ��
	deletePointersInCvMatArray(cvmatSamples); //ɾ��cvmatSamples�е�ָ��
	deletePointersInQPixmapArray(qpixmapSamples);//ɾ��qpixmapSamples�е�ָ��
	delete templThread;
	delete templExtractor;
}

//��Ϊ����ʵ���Ĺ����ʵ��ָ�봫�ݵ�ʱ������
//�����źźͲۺ�������ֱ���ڹ��캯�������ӣ�����������
void TemplateUI::doConnect()
{
	//�˶�����
	connect(motionControler, SIGNAL(moveForwardFinished_motion()), this, SLOT(on_moveForwardFinished_motion()));
	connect(motionControler, SIGNAL(resetControlerFinished_motion(int)), this, SLOT(on_resetControlerFinished_motion(int)));
	//�������
	connect(cameraControler, SIGNAL(initCamerasFinished_camera(int)), this, SLOT(on_initCamerasFinished_camera(int)));
	connect(cameraControler, SIGNAL(takePhotosFinished_camera(int)), this, SLOT(on_takePhotosFinished_camera(int)));
	//ת���߳�
	connect(&imgConvertThread, SIGNAL(convertFinished_convertThread()), this, SLOT(on_convertFinished_convertThread()));
}


/****************** ����ĳ�ʼ�������� *******************/

//�Ի�ͼ�ؼ�GraphicsView�ĳ�ʼ������
void TemplateUI::initGraphicsView()
{
	initItemGrid(itemGrid);//��ʼ��ͼԪ����
	initPointersInItemArray(itemArray);//��ʼ��itemArray
	initPointersInCvMatArray(cvmatSamples);//��ʼ��cvmatSamples
	initPointersInQPixmapArray(qpixmapSamples);//��ʼ��qpixmapSamples

	//��ʼ���������ڼ��ӳ�������״̬�ı���
	currentRow_show = -1; //��ʾ�к�
	detectParams->currentRow_extract = -1; //����к�
	eventCounter = 0; //�¼�������

	//����ת���߳�
	imgConvertThread.setCvMats(&cvmatSamples);
	imgConvertThread.setQPixmaps(&qpixmapSamples);
	imgConvertThread.setCurrentRow(&currentRow_show);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);

	//��Ʒ���ʶ�����
	serialNumberUI.setDetectParams(detectParams);
	serialNumberUI.setCvMatArray(&cvmatSamples);
	serialNumberUI.setQPixmapArray(&qpixmapSamples);

	//������ȡ�߳�
	templThread->setDetectParams(detectParams);
	templThread->setDetectConfig(detectConfig);
	templExtractor->setDetectParams(detectParams);
	templExtractor->setDetectConfig(detectConfig);
	templExtractor->setSampleImages(&cvmatSamples);

	//��ͼ�ؼ�������
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //���þ۽�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView->setScene(&scene); //����ͼ����ӳ���
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //���ô�ֱ��������λ��
}

//����ģ����ȡ����
void TemplateUI::resetTemplateUI()
{
	removeItemsFromGraphicsScene(); //�Ƴ��������Ѿ����ص�ͼԪ
	deletePointersInItemArray(itemArray); //ɾ��ͼԪ�����е�ָ��
	deletePointersInCvMatArray(cvmatSamples); //ɾ��cvmatSamples�е�ָ��
	deletePointersInQPixmapArray(qpixmapSamples);//ɾ��qpixmapSamples�е�ָ��
	currentRow_show = -1; //��ʾ�кŵĸ�λ
	detectParams->currentRow_extract = -1; //��ȡ�кŵĸ�λ
	eventCounter = 0; //�¼�������
	ui.graphicsView->centerOn(0, 0); //��ֱ������λ
	qApp->processEvents();
}


/********* ͼԪ���������ͼ�����ĳ�ʼ����ɾ���Ȳ��� ***********/

//��ʼ��ͼԪ����
void TemplateUI::initItemGrid(Ui::ItemGrid &grid)
{
	//��������
	int nCamera = detectConfig->nCamera; //�������
	int nPhotographing = detectConfig->nPhotographing; //�������
	QString SampleDirPath = detectConfig->SampleDirPath; //sample�ļ��е�·�� 
	//QSize imageSize = config->imageSize; //ԭͼ�ߴ�

	//�����ܼ��
	QSize totalSpacing; //�ܼ��
	totalSpacing.setWidth(itemSpacing * (nCamera + 1)); //����ܿ��
	totalSpacing.setHeight(itemSpacing * (nPhotographing + 1)); //����ܸ߶�

	//����ͼԪ�ߴ�
	QSize viewSize = ui.graphicsView->size(); //��ͼ�ߴ�
	itemSize.setWidth(int((viewSize.width() - totalSpacing.width()) / nCamera)); //ͼԪ���
	//qreal itemAspectRatio = qreal(imageSize.width()) / imageSize.height(); //��߱�
	qreal itemAspectRatio = detectConfig->ImageAspectRatio; //��߱�
	itemSize.setHeight(int(itemSize.width() / itemAspectRatio)); //ͼԪ�߶�

	//���㳡���ߴ�
	sceneSize = totalSpacing;
	sceneSize += QSize(itemSize.width()*nCamera, itemSize.height()*nPhotographing);
	scene.setSceneRect(0, 0, sceneSize.width(), sceneSize.height());

	//���ɻ�ͼ���� -- ������Ҫ�޸ģ����ǵ�һ�����о���Ҫ���itemGrid
	QSize spacingBlock = QSize(itemSpacing, itemSpacing);
	gridSize = itemSize + spacingBlock; //ÿ������ĳߴ�

	//�ж�itemGrid�Ƿ�ִ�й���ʼ��
	if (grid.size() > 0) {
		grid.clear();
	}

	//��ʼ����ֵ
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //��
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //��
			QPointF pos(itemSpacing, itemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		grid.append(posList);
	}
}

//��ʼ��ͼԪ�����е�ָ�� - ItemArray
void TemplateUI::initPointersInItemArray(Ui::ItemArray &items)
{
	if (items.size() > 0) {
		deletePointersInItemArray(items);//��ִ�й�init����������deleteָ��
	}
	else {
		items.resize(detectConfig->nPhotographing); //���ô�С
		for (int iPhotographing = 0; iPhotographing < detectConfig->nPhotographing; iPhotographing++) { //��
			items[iPhotographing].resize(detectConfig->nCamera);
			for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) { //��
				items[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//ɾ��ͼԪ�����е�ָ�� - ItemArray
void TemplateUI::deletePointersInItemArray(Ui::ItemArray &items)
{
	for (int iPhotographing = 0; iPhotographing < items.size(); iPhotographing++) {
		int vectorSize = items[iPhotographing].size();
		for (int iCamera = 0; iCamera < vectorSize; iCamera++) {
			delete items[iPhotographing][iCamera];
			items[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//��ʼ������ͼ�������е�ָ�� - CvMatArray
void TemplateUI::initPointersInCvMatArray(Ui::CvMatArray &cvmats)
{
	if (cvmats.size() > 0) {
		deletePointersInCvMatArray(cvmats);//��ִ�й�init����������deleteָ��
	}
	else {
		cvmats.resize(detectConfig->nPhotographing);
		for (int iPhotographing = 0; iPhotographing < detectConfig->nPhotographing; iPhotographing++) { //��
			cvmats[iPhotographing].resize(detectConfig->nCamera);
			for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
				cvmats[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//ɾ������ͼ�������е�ָ�� - CvMatArray
void TemplateUI::deletePointersInCvMatArray(Ui::CvMatArray &cvmats)
{
	for (int iPhotographing = 0; iPhotographing < cvmats.size(); iPhotographing++) {
		int vectorSize = cvmats[iPhotographing].size();
		for (int iCamera = 0; iCamera < vectorSize; iCamera++) {
			delete cvmats[iPhotographing][iCamera];
			cvmats[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}


//��ʼ������ͼ�������е�ָ�� - QPixmapArray
void TemplateUI::initPointersInQPixmapArray(Ui::QPixmapArray &qpixmaps)
{
	if (qpixmaps.size() > 0) {
		deletePointersInQPixmapArray(qpixmaps);//��ִ�й�init����������deleteָ��
	}
	else {
		qpixmaps.resize(detectConfig->nPhotographing);
		for (int iPhotographing = 0; iPhotographing < detectConfig->nPhotographing; iPhotographing++) { //��
			qpixmaps[iPhotographing].resize(detectConfig->nCamera);
			for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
				qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//ɾ������ͼ�������е�ָ�� - QPixmapArray
void TemplateUI::deletePointersInQPixmapArray(Ui::QPixmapArray &qpixmaps)
{
	for (int iPhotographing = 0; iPhotographing < qpixmaps.size(); iPhotographing++) {
		int vectorSize = qpixmaps[iPhotographing].size();
		for (int iCamera = 0; iCamera < vectorSize; iCamera++) {
			delete qpixmaps[iPhotographing][iCamera];
			qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
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


/***************** ������Ӧ *****************/

//��ʼ��ȡ�µ�PCB��
void TemplateUI::on_pushButton_start_clicked()
{
	if (detectParams->currentRow_extract == -1 && !templThread->isRunning()) {
		ui.label_status->setText(QString::fromLocal8Bit("��ʼ����"));
		ui.pushButton_start->setEnabled(false); //���ÿ�ʼ����
		ui.pushButton_return->setEnabled(false); //���÷��ذ���

		resetTemplateUI();//����ģ����ȡ��ģ��
		motionControler->resetControler(1); //�˶��ṹ��λ
	}
}

//���
//void TemplateUI::on_pushButton_clear_clicked()
//{
//	resetTemplateUI();
//	ui.label_status->setText(QString::fromLocal8Bit("�������������"));
//}

//����
void TemplateUI::on_pushButton_return_clicked()
{
	resetTemplateUI(); //����ģ����ȡ���棬��ջ�������
	emit showDetectMainUI(); //���ͷ����ź�
}


/***************** ��ȡ�ⲿ�ź� ******************/

//��ʱʹ���û����̰���ģ���ⲿ�ź�
void TemplateUI::keyPressEvent(QKeyEvent *event)
{
	detectParams->sampleModelNum = "1"; //�ͺ�
	detectParams->sampleBatchNum = "1"; //���κ�
	detectParams->sampleNum = "2"; //�������

	switch (event->key())
	{
	case Qt::Key_PageUp:
		qDebug() << "===== Key_PageUp";
		if (!templThread->isRunning()) {
			resetTemplateUI();//����ģ����ȡ��ģ��
			motionControler->resetControler(2); //�˶��ṹ��λ
		}
	case Qt::Key_PageDown: //���µ�PCB��ʱ�����
		qDebug() << "===== Key_PageDown";
		break;
	case Qt::Key_Up:
		qDebug() << "===== Up";
		break;
	case Qt::Key_Down:
		qDebug() << "===== Down";
		if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1 && !templThread->isRunning())
			resetTemplateUI();//����ģ����ȡ��ģ��

		//!imgConvertThread.isRunning()
		if (currentRow_show + 1 < detectConfig->nPhotographing && true) { //ֱ����ʾ�µ�������
			currentRow_show += 1; //������ʾ�к�
			qDebug() << "currentRow_show  - " << currentRow_show;

			ui.label_status->setText(QString::fromLocal8Bit("���������") +
				QString::number(currentRow_show + 1) +
				QString::fromLocal8Bit("��ͼ��"));//����״̬
			qApp->processEvents();

			readSampleImages2(); //��ͼ - �൱���������		
		}
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << "===== Enter";
		break;
	default:
		break;
	}
}


//�ڻ�ͼ��������ʾ��һ��ͼ��
void TemplateUI::nextRowOfSampleImages()
{
	if (currentRow_show + 1 < detectConfig->nPhotographing) { //ֱ����ʾ�µ�������
		currentRow_show += 1; //������ʾ�к�
		eventCounter += 1; //�����¼�������
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		//readSampleImages(); //��ͼ
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
	else if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1 && !templThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		resetTemplateUI();//����ģ����ȡ��ģ��
		nextRowOfSampleImages(); //����µ�PCB����ͼ
	}
}

//�ڻ�ͼ��������ʾ��һ��ͼ��
void TemplateUI::nextRowOfSampleImages2()
{
	if (currentRow_show + 1 < detectConfig->nPhotographing) { //ֱ����ʾ�µ�������
		currentRow_show += 1; //������ʾ�к�
		eventCounter += 1; //�����¼�������
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		readSampleImages2(); //��ͼ
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
	else if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1 && !templThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		resetTemplateUI();//����ģ����ȡ��ģ��
		nextRowOfSampleImages2(); //����µ�PCB����ͼ
	}
}


/***************** ��ͼ����ʾ����ȡ *****************/

//��ȡ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void TemplateUI::readSampleImages()
{
	//�����on_takePhotos_finished_camera()
}

//��ȡ����������һ���ͼ - ֱ�Ӵ�Ӳ���϶�ͼ
void TemplateUI::readSampleImages2()
{
	clock_t t1 = clock();

	//��ȡ��ӦĿ¼��·��
	QString dirpath = detectConfig->SampleDirPath + "/" + detectParams->sampleModelNum + "/"
		+ detectParams->sampleBatchNum + "/" + detectParams->sampleNum;

	//��ȡĿ¼�µ�����ͼ��
	QDir dir(dirpath);
	dir.setSorting(QDir::Time | QDir::Name | QDir::Reversed);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	if (fileList.isEmpty()) { emit invalidNummberOfSampleImage(); return; }

	if (detectParams->imageSize.width() < 0) { //С��0˵��imageSizeδ��ʼ��
		QImage img = QImage(fileList.at(0).absoluteFilePath());//��ͼ
		detectParams->imageSize = img.size();
		templThread->initTemplFunc();
	}

	//��ȡ������ͼ�� - ��Ӳ���϶�ͼ (������ʱ����)
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1; //0 1 ... nPhotographing-1
		int iCamera = idxs[1].toInt() - 1; //0 1 ... nCamera-1
		if (iPhotographing != currentRow_show) continue;
		if (iPhotographing < 0 || iPhotographing >= detectConfig->nPhotographing) continue;
		if (iCamera < 0 || iCamera >= detectConfig->nCamera) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //����ͼ��·��
		cv::Mat img = cv::imread(filepath.toStdString(), cv::IMREAD_COLOR);
		cvmatSamples[currentRow_show][iCamera] = new cv::Mat(img);
	}
	
	clock_t t2 = clock();
	qDebug() << "readSampleImages2: " << (t2 - t1) << "ms ( currentRow -" << currentRow_show << ")";

	//ͼ������ת��
	imgConvertThread.start();
}


//��ʾ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void TemplateUI::showSampleImages()
{
	for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
		QPixmap scaledImg = (*qpixmapSamples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //����ͼԪ
		item->setPos(itemGrid[currentRow_show][iCamera]); //ͼԪ����ʾλ��
		itemArray[currentRow_show][iCamera] = item; //����ͼ����
	}

	//����������������һ��ͼԪ
	for (int iCamera = 0; iCamera < detectConfig->nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//��ͼ��ʾ����
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //���ô�ֱ��������λ��
	ui.graphicsView->show();//��ʾ
}


/******************** �ַ�ʶ�� ********************/

//ͨ�����˫�����ַ�ʶ�����
void TemplateUI::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (currentRow_show == -1) return;//��û����ʾ��Ӧ������ͼ��ֱ�ӷ���
	if (event->button() == Qt::RightButton) return;//�����Ҽ����

	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	QPoint mousePosition(event->x(), event->y());
	
	//qDebug() << graphicsViewPos.x() << graphicsViewPos.y();
	//qDebug() << graphicsViewSize.width() << graphicsViewSize.height();
	//qDebug() << graphicsViewRect.contains(mousePosition);
	//qDebug() << mousePosition.x() << mousePosition.y();

	//todo �ж�����������ĸ���ͼ
	if (!graphicsViewRect.contains(mousePosition)) return;
	QPoint relativePos = mousePosition - graphicsViewPos; //���λ��
	int gridColIdx = (int) ceil(relativePos.x() / gridSize.width());//���λ���ڵڼ���
	int gridRowIdx = (int) ceil(relativePos.y() / gridSize.height());//���λ���ڵڼ���
	//qDebug() << gridColIdx << gridRowIdx;
	
	if (true && gridRowIdx <= currentRow_show) {
		serialNumberUI.showSampleImage(gridRowIdx, gridColIdx);
		Ui::delay(3);//�ӳ�
		serialNumberUI.showFullScreen();//��ʾ���ʶ�����
		Ui::delay(10);//�ӳ�
		this->hide();
	}

}

//�л���ͼ
void TemplateUI::on_switchImage_serialNumberUI()
{
	this->showFullScreen();
	Ui::delay(10);//�ӳ�
	serialNumberUI.hide();
}

//�����ʶ������ò�Ʒ���֮��
void TemplateUI::on_recognizeFinished_serialNumberUI()
{
	//�Ƚ���Ʒ���ת��Ϊ �ͺš����κš��������

	//Ȼ�������ȡ���� ��ʼ��ȡģ��

}


/******************** �˶����� ********************/

//�˶��ṹǰ������
void TemplateUI::on_moveForwardFinished_motion()
{
	//���������������
	if (currentRow_show + 1 < detectConfig->nPhotographing) {
		currentRow_show += 1; //������ʾ�к�

		ui.label_status->setText(QString::fromLocal8Bit("���������") +
			QString::number(currentRow_show + 1) + 
			QString::fromLocal8Bit("�з�ͼ"));//����״̬
		qApp->processEvents();

		cameraControler->start(); //����
	}
}

//��λ
void TemplateUI::on_resetControlerFinished_motion(int caller)
{
	switch (caller)
	{
	case 1: //TemplateUI::on_pushButton_start_clicked()
		Ui::delay(10);
		motionControler->moveForward(); //�˶��ṹǰ��
		break;
	default:
		break;
	}
}

/******************** ������� ********************/

//�����ʼ������
void TemplateUI::on_initCamerasFinished_camera(int)
{
}

//����������
void TemplateUI::on_takePhotosFinished_camera(int)
{
	//��ʼ�����ɱ���
	if (detectParams->imageSize.width() <= 0) {
		cv::Size size = cvmatSamples[0][0]->size();
		detectParams->imageSize = QSize(size.width, size.height);
		templThread->initTemplFunc();
	}

	//����ͼ������ת���߳�
	imgConvertThread.start();
}


/******************** ͼ��ת���߳� ********************/

//ͼ��ת���������ڽ�������ʾͼ��Ȼ����ȡģ��
void TemplateUI::on_convertFinished_convertThread()
{
	//�����¼�������
	eventCounter += 1;

	//����״̬��
	ui.label_status->setText(QString::fromLocal8Bit("��") +
		QString::number(currentRow_show + 1) +
		QString::fromLocal8Bit("���������"));
	qApp->processEvents();

	//�ڽ�������ʾ����ͼ
	clock_t t1 = clock();
	showSampleImages(); 
	clock_t t2 = clock();
	qDebug() << "showSampleImages: " << (t2 - t1) << "ms ( currentRow =" << currentRow_show << ")";

	//��ʾ������֮ǰ������е�ṹ�˶�
	Ui::delay(10); //�ӳ�
	if (currentRow_show + 1 < detectConfig->nPhotographing)
		motionControler->moveForward(); //�˶��ṹǰ��
	else
		motionControler->resetControler(3); //��ǰPCB������λ

	//�ж��Ƿ�ִ�м�����
	if (eventCounter == 1 && !templThread->isRunning()) {
		extractTemplateImages(); //��ȡ
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

//��ȡ��ǰ��һ������ͼ��
void TemplateUI::extractTemplateImages()
{
	//������ȡ�к�
	detectParams->currentRow_extract += 1;

	//����״̬��
	ui.label_status->setText(QString::fromLocal8Bit("������ȡ��") +
		QString::number(detectParams->currentRow_extract + 1) + 
		QString::fromLocal8Bit("��ģ��"));
	qApp->processEvents();

	//��������߳�
	templThread->start();
}

//��ȡ���������״̬
void TemplateUI::update_extractState_extractor(int state)
{
	//����ȡ��һ��ͼ��
	if ((TemplateExtractor::ExtractState)state == TemplateExtractor::Finished) { 
		eventCounter -= 1; //�����¼�������

		//����״̬��
		ui.label_status->setText(QString::fromLocal8Bit("��") +
			QString::number(detectParams->currentRow_extract + 1) +
			QString::fromLocal8Bit("����ȡ����"));
		qApp->processEvents();

		//����Ƿ���δ������¼�
		while (templThread->isRunning()) Ui::delay(50); //�ȴ��߳̽���
		if (detectParams->currentRow_extract == detectConfig->nPhotographing - 1) { //��ǰPCB��ȡ����
			ui.pushButton_start->setEnabled(true); //���ÿ�ʼ����
			ui.pushButton_return->setEnabled(true); //���÷��ذ���
		}
		else { //��ǰPCBδ��ȡ��
			if (eventCounter > 0) extractTemplateImages(); //��ȡ��һ�з�ͼ
		}
	}
}

