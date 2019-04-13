#include "DetectUI.h"
#include <time.h>

using pcb::DetectConfig;
using pcb::DetectParams;


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
	
	//��Ա������ʼ��
	adminConfig = Q_NULLPTR;
	detectConfig = Q_NULLPTR;
	detectParams = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;
	detectState = DefectDetecter::Default;

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

	//��Ʒ���ʶ�����
	connect(&serialNumberUI, SIGNAL(recognizeFinished_serialNumUI()), this, SLOT(on_recognizeFinished_serialNumUI()));
	connect(&serialNumberUI, SIGNAL(switchImage_serialNumUI()), this, SLOT(on_switchImage_serialNumUI()));

	//����̵߳��ź�����
	defectDetecter = new DefectDetecter;
	connect(defectDetecter, SIGNAL(updateDetectState_detecter(int)), this, SLOT(do_updateDetectState_detecter(int)));

	detectThread = new DetectThread;
	detectThread->setDefectDetecter(defectDetecter);
}


DetectUI::~DetectUI()
{
	deletePointersInItemArray(itemArray); //ɾ��ͼԪ�����е�ָ��
	deletePointersInCvMatArray(cvmatSamples); //ɾ��cvmatSamples�е�ָ��
	deletePointersInQPixmapArray(qpixmapSamples);//ɾ��qpixmapSamples�е�ָ��
	delete detectThread; //ɾ������߳�
	delete defectDetecter; //ɾ��������
}

//��Ϊ����ʵ���Ĺ����ʵ��ָ�봫�ݵ�ʱ������
//�����źźͲۺ�������ֱ���ڹ��캯�������ӣ�����������
void DetectUI::doConnect()
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
void DetectUI::initGraphicsView()
{
	initItemGrid(itemGrid);//��ʼ��ͼԪ����
	initPointersInItemArray(itemArray);//��ʼ��itemArray
	initPointersInCvMatArray(cvmatSamples);//��ʼ��cvmatSamples
	initPointersInQPixmapArray(qpixmapSamples);//��ʼ��qpixmapSamples

	//��ʼ���������ڼ��ӳ�������״̬�ı���
	currentRow_show = -1; //��ʾ�к�
	detectParams->currentRow_detect = -1; //����к�
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

	//���ü���߳�
	detectThread->setAdminConfig(adminConfig);
	detectThread->setDetectConfig(detectConfig);
	detectThread->setDetectParams(detectParams);
	detectThread->setSampleImages(&cvmatSamples);
	detectThread->setDetectResult(&detectResult);
	detectThread->initDefectDetecter();

	//��ͼ�ؼ�������
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //���þ۽�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView->setScene(&scene); //����ͼ����ӳ���
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //���ô�ֱ��������λ��
}

//���ü���ӽ���
void DetectUI::resetDetectUI()
{
	ui.label_status->setText(""); //���״̬��
	removeItemsFromGraphicsScene(); //�Ƴ��������Ѿ����ص�ͼԪ
	deletePointersInItemArray(itemArray); //ɾ��ͼԪ�����е�ָ��
	deletePointersInCvMatArray(cvmatSamples); //ɾ��cvmatSamples�е�ָ��
	deletePointersInQPixmapArray(qpixmapSamples);//ɾ��qpixmapSamples�е�ָ��
	currentRow_show = -1; //��ʾ�кŵĸ�λ
	detectParams->currentRow_detect = -1; //����кŵĸ�λ
	eventCounter = 0; //�¼�������
	ui.graphicsView->centerOn(0, 0); //��ֱ������λ
	qApp->processEvents();
}


/********* ͼԪ���������ͼ�����ĳ�ʼ����ɾ���Ȳ��� ***********/

//��ʼ��ͼԪ����
void DetectUI::initItemGrid(pcb::ItemGrid &grid)
{
	//��������
	int nCamera = detectParams->nCamera; //�������
	int nPhotographing = detectParams->nPhotographing; //�������
	QString SampleDirPath = detectConfig->SampleDirPath; //sample�ļ��е�·�� 

	//�����ܼ��
	QSize totalSpacing; //�ܼ��
	totalSpacing.setWidth(itemSpacing * (nCamera + 1)); //����ܿ��
	totalSpacing.setHeight(itemSpacing * (nPhotographing + 1)); //����ܸ߶�

	//����ͼԪ�ߴ�
	QSize viewSize = ui.graphicsView->size(); //��ͼ�ߴ�
	itemSize.setWidth(int((viewSize.width() - totalSpacing.width()) / nCamera)); //ͼԪ���
	qreal itemAspectRatio = adminConfig->ImageAspectRatio; //��߱�
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
void DetectUI::initPointersInItemArray(pcb::ItemArray &items)
{
	if (items.size() > 0) {
		deletePointersInItemArray(items);//��ִ�й�init����������deleteָ��
	}
	else {
		items.resize(detectParams->nPhotographing); //���ô�С
		for (int iPhotographing = 0; iPhotographing < detectParams->nPhotographing; iPhotographing++) { //��
			items[iPhotographing].resize(detectParams->nCamera);
			for (int iCamera = 0; iCamera < detectParams->nCamera; iCamera++) { //��
				items[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//ɾ��ͼԪ�����е�ָ�� - ItemArray
void DetectUI::deletePointersInItemArray(pcb::ItemArray &items)
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
void DetectUI::initPointersInCvMatArray(pcb::CvMatArray &cvmats)
{
	if (cvmats.size() > 0) {
		deletePointersInCvMatArray(cvmats);//��ִ�й�init����������deleteָ��
	}
	else {
		cvmats.resize(detectParams->nPhotographing);
		for (int iPhotographing = 0; iPhotographing < detectParams->nPhotographing; iPhotographing++) { //��
			cvmats[iPhotographing].resize(detectParams->nCamera);
			for (int iCamera = 0; iCamera < detectParams->nCamera; iCamera++) {
				cvmats[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//ɾ������ͼ�������е�ָ�� - CvMatArray
void DetectUI::deletePointersInCvMatArray(pcb::CvMatArray &cvmats)
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
void DetectUI::initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
{
	if (qpixmaps.size() > 0) {
		deletePointersInQPixmapArray(qpixmaps);//��ִ�й�init����������deleteָ��
	}
	else {
		qpixmaps.resize(detectParams->nPhotographing);
		for (int iPhotographing = 0; iPhotographing < detectParams->nPhotographing; iPhotographing++) { //��
			qpixmaps[iPhotographing].resize(detectParams->nCamera);
			for (int iCamera = 0; iCamera < detectParams->nCamera; iCamera++) {
				qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
			}
		}
	}
}

//ɾ������ͼ�������е�ָ�� - QPixmapArray
void DetectUI::deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
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
void DetectUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);
	}
}


/****************** ������Ӧ *******************/

//��ʼ����µ�PCB��
void DetectUI::on_pushButton_start_clicked()
{
	if (detectParams->currentRow_detect == -1 && !detectThread->isRunning()) {
		ui.label_status->setText(pcb::chinese("��ʼ����"));
		ui.pushButton_start->setEnabled(false); //���ÿ�ʼ����
		ui.pushButton_return->setEnabled(false); //���÷��ذ���

		resetDetectUI();//���ü����ģ��
		motionControler->resetControler(1); //�˶��ṹ��λ
	}
}

//����
void DetectUI::on_pushButton_return_clicked()
{
	resetDetectUI(); //���ü���ӽ��棬��ջ�������
	emit showDetectMainUI(); //���ͷ����ź�
}



/***************** ��ȡ�ⲿ�ź� ******************/

//��ʱʹ���û����̰���ģ���ⲿ�ź�
void DetectUI::keyPressEvent(QKeyEvent *event)
{
	detectParams->sampleModelNum = "1"; //�ͺ�
	detectParams->sampleBatchNum = "1"; //���κ�
	detectParams->sampleNum = "5"; //�������

	switch (event->key())
	{
	case Qt::Key_PageUp:
		qDebug() << ">>>>>>>>>> Key_PageUp";
		if (!detectThread->isRunning()) {
			resetDetectUI();//���ü����ģ��
			motionControler->resetControler(2); //�˶��ṹ��λ
		}
	case Qt::Key_PageDown: //���µ�PCB��ʱ�����
		qDebug() << ">>>>>>>>>> Key_PageDown";
		break;
	case Qt::Key_Up:
		qDebug() << ">>>>>>>>>> Up";
		break;
	case Qt::Key_Down:
		qDebug() << ">>>>>>>>>> Down";
		if (detectParams->currentRow_detect == detectParams->nPhotographing - 1 && !detectThread->isRunning())
			resetDetectUI();//���ü����ģ��

		//!imgConvertThread.isRunning()
		if (currentRow_show + 1 < detectParams->nPhotographing && true) { //ֱ����ʾ�µ�������
			currentRow_show += 1; //������ʾ�к�
			qDebug() << "currentRow_show  - " << currentRow_show;

			ui.label_status->setText(pcb::chinese("���������") +
				QString::number(currentRow_show + 1) +
				pcb::chinese("�з�ͼ"));//����״̬
			qApp->processEvents();

			readSampleImages2(); //��ͼ - �൱���������		
		}
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << ">>>>>>>>>> Enter";
		break;
	default:
		break;
	}
}


//�ڻ�ͼ��������ʾ��һ��ͼ��
void DetectUI::nextRowOfSampleImages()
{
	if (currentRow_show + 1 < detectParams->nPhotographing) { //ֱ����ʾ�µ�������
		currentRow_show += 1; //������ʾ�к�
		eventCounter += 1; //�����¼�������
		qDebug() << ">>>>>>>>>> " << pcb::chinese("��ʾ��ͼ ... ") <<
			"currentRow_show  - " << currentRow_show;

		double t1 = clock();
		//readSampleImages(); //��ͼ
		double t2 = clock();
		showSampleImages(); //������ʾ
		double t3 = clock();

		qDebug() << ">>>>>>>>>> " << pcb::chinese("��ͼ��ʾ������") 
			<< (t3 - t2) << "ms ( currentRow_show -" << currentRow_show << ")";

		//�ж��Ƿ�ִ�м�����
		if (!detectThread->isRunning() && eventCounter == 1) {
			detectSampleImages(); //���
		}
	}
	else if (detectParams->currentRow_detect == detectParams->nPhotographing - 1 && !detectThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		resetDetectUI();//���ü����ģ��
		nextRowOfSampleImages(); //����µ�PCB����ͼ
	}
}

//�ڻ�ͼ��������ʾ��һ��ͼ��
void DetectUI::nextRowOfSampleImages2()
{
	if (currentRow_show + 1 < detectParams->nPhotographing) { //ֱ����ʾ�µ�������
		currentRow_show += 1; //������ʾ�к�
		eventCounter += 1; //�����¼�������
		qDebug() << "currentRow_show  - " << currentRow_show;

		double t1 = clock();
		readSampleImages2(); //��ͼ
		double t2 = clock();
		showSampleImages(); //������ʾ
		double t3 = clock();

		qDebug() << ">>>>>>>>>> " << pcb::chinese("��ͼ��ʾ���� - ")
			<< (t3 - t2) << "ms ( currentRow_show -" << currentRow_show << ")";

		//�ж��Ƿ�ִ�м�����
		if (!detectThread->isRunning() && eventCounter == 1) {
			detectSampleImages(); //���
		}
	}
	else if (detectParams->currentRow_detect == detectParams->nPhotographing - 1 && !detectThread->isRunning()) {
		qDebug() << "currentRow_show  - " << currentRow_show;

		resetDetectUI();//���ü����ģ��
		nextRowOfSampleImages2(); //����µ�PCB����ͼ
	}
}


/***************** ��ͼ����ʾ����ȡ *****************/

//��ȡ����������һ���ͼ - ֱ�Ӵ�Ӳ���϶�ͼ
void DetectUI::readSampleImages2()
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
	if (fileList.isEmpty()) { emit invalidNumberOfSampleImage(); return; }

	//��ȡ������ͼ�� - ��Ӳ���϶�ͼ (������ʱ����)
	for (int i = 0; i < fileList.size(); i++) {
		QString baseName = fileList.at(i).baseName();
		QStringList idxs = baseName.split("_");
		if (idxs.length() < 2) return;

		int iPhotographing = idxs[0].toInt() - 1; //0 1 ... nPhotographing-1
		int iCamera = idxs[1].toInt() - 1; //0 1 ... nCamera-1
		if (iPhotographing != currentRow_show) continue;
		if (iPhotographing < 0 || iPhotographing >= detectParams->nPhotographing) continue;
		if (iCamera < 0 || iCamera >= detectParams->nCamera) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //����ͼ��·��
		cv::Mat img = cv::imread(filepath.toStdString(), cv::IMREAD_COLOR);
		cvmatSamples[currentRow_show][iCamera] = new cv::Mat(img);
	}

	clock_t t2 = clock();
	qDebug() << ">>>>>>>>>> " << pcb::chinese("��ͼ��ȡ������")
		<< (t2 - t1) << "ms ( currentRow_show -" << currentRow_show << ")";

	//ͼ������ת��
	imgConvertThread.start();
}


//��ʾ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void DetectUI::showSampleImages()
{
	for (int iCamera = 0; iCamera < detectParams->nCamera; iCamera++) {
		QPixmap scaledImg = (*qpixmapSamples[currentRow_show][iCamera]).scaled(itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //����ͼԪ
		item->setPos(itemGrid[currentRow_show][iCamera]); //ͼԪ����ʾλ��
		itemArray[currentRow_show][iCamera] = item; //����ͼ����
	}

	//����������������һ��ͼԪ
	for (int iCamera = 0; iCamera < detectParams->nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//��ͼ��ʾ����
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //���ô�ֱ��������λ��
	ui.graphicsView->show();//��ʾ
}



/******************** �ַ�ʶ�� ********************/

//ͨ�����˫�����ַ�ʶ�����
void DetectUI::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (currentRow_show == -1) return;//��û����ʾ��Ӧ������ͼ��ֱ�ӷ���
	if (event->button() == Qt::RightButton) return;//�����Ҽ����

	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	QPoint mousePosition(event->x(), event->y());

	//�ж�����������ĸ���ͼ
	if (!graphicsViewRect.contains(mousePosition)) return;
	QPoint relativePos = mousePosition - graphicsViewPos; //���λ��
	int gridColIdx = (int)ceil(relativePos.x() / gridSize.width());//���λ���ڵڼ���
	int gridRowIdx = (int)ceil(relativePos.y() / gridSize.height());//���λ���ڵڼ���

	if (true && gridRowIdx <= currentRow_show) {
		serialNumberUI.showSampleImage(gridRowIdx, gridColIdx);
		pcb::delay(3);//�ӳ�
		serialNumberUI.showFullScreen();//��ʾ���ʶ�����
		pcb::delay(10);//�ӳ�
		this->hide();
	}
}

//�л���ͼ
void DetectUI::on_switchImage_serialNumUI()
{
	this->showFullScreen();
	pcb::delay(10);//�ӳ�
	serialNumberUI.hide();
}

//�����ʶ������ò�Ʒ���֮��
void DetectUI::on_recognizeFinished_serialNumUI()
{
	//�Ƚ���Ʒ���ת��Ϊ �ͺš����κš��������

	//Ȼ�������ȡ���� ��ʼ��ȡģ��

}



/******************** �˶����� ********************/

//�˶��ṹǰ������
void DetectUI::on_moveForwardFinished_motion()
{
	//���������������
	if (currentRow_show + 1 < detectParams->nPhotographing) {
		currentRow_show += 1; //������ʾ�к�

		ui.label_status->setText(pcb::chinese("���������") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("�з�ͼ"));//����״̬
		qApp->processEvents();

		cameraControler->start(); //����
	}
}

//��λ
void DetectUI::on_resetControlerFinished_motion(int caller)
{
	switch (caller)
	{
	case 1: //TemplateUI::on_pushButton_start_clicked()
		pcb::delay(10);
		motionControler->moveForward(); //�˶��ṹǰ��
		break;
	default:
		break;
	}
}

/******************** ������� ********************/

//�����ʼ������
void DetectUI::on_initCamerasFinished_camera(int)
{
}

//����������
void DetectUI::on_takePhotosFinished_camera(int)
{
	//��ʼ�����ɱ���
	//if (detectParams->imageSize.width() <= 0) {
	//	cv::Size size = cvmatSamples[0][0]->size();
	//	detectParams->imageSize = QSize(size.width, size.height);
	//	templThread->initTemplFunc();
	//}

	//����ͼ������ת���߳�
	imgConvertThread.start();
}


/******************** ͼ��ת���߳� ********************/

//ͼ��ת���������ڽ�������ʾͼ��Ȼ����ȡģ��
void DetectUI::on_convertFinished_convertThread()
{
	//�����¼�������
	eventCounter += 1;

	//����״̬��
	ui.label_status->setText(pcb::chinese("��") +
		QString::number(currentRow_show + 1) +
		pcb::chinese("���������"));
	qApp->processEvents();

	//�ڽ�������ʾ����ͼ
	clock_t t1 = clock();
	showSampleImages();
	clock_t t2 = clock();
	qDebug() << "showSampleImages: " << (t2 - t1) << "ms ( currentRow =" << currentRow_show << ")";

	//��ʾ������֮ǰ������е�ṹ�˶�
	pcb::delay(10); //�ӳ�
	if (currentRow_show + 1 < detectParams->nPhotographing)
		motionControler->moveForward(); //�˶��ṹǰ��
	else
		motionControler->resetControler(3); //��ǰPCB������λ

	//�ж��Ƿ�ִ�м�����
	if (eventCounter == 1 && !detectThread->isRunning()) {
		detectSampleImages(); //���
	}
}


/******************** ģ����ȡ�߳� ********************/

//��⵱ǰ��һ������ͼ��
void DetectUI::detectSampleImages()
{
	//������ȡ�к�
	detectParams->currentRow_detect += 1;

	//����״̬��
	ui.label_status->setText(pcb::chinese("���ڼ���") +
		QString::number(detectParams->currentRow_detect + 1) +
		pcb::chinese("�з�ͼ"));
	qApp->processEvents();

	//��������߳�
	detectThread->start();
}

//��ȡ���������״̬
void DetectUI::do_updateDetectState_detecter(int state)
{
	if ((DefectDetecter::DetectState)state == DefectDetecter::Finished) { //�����һ��ͼ��

		eventCounter -= 1; //�����¼�������
		ui.label_status->setText(QString::fromLocal8Bit("��") +
			QString::number(detectParams->currentRow_detect + 1) +
			QString::fromLocal8Bit("�м�����"));//����״̬��
		qApp->processEvents();

		//����Ƿ���δ������¼�
		while (detectThread->isRunning()) pcb::delay(50); //�ȴ��߳̽���
		if (detectParams->currentRow_detect == detectParams->nPhotographing - 1) { //��ǰPCB��ȡ����
			ui.pushButton_start->setEnabled(true); //���ÿ�ʼ����
			ui.pushButton_return->setEnabled(true); //���÷��ذ���
		}
		else { //��ǰPCBδ��ȡ��
			if (eventCounter > 0) detectSampleImages(); //�����һ�з�ͼ
		}

		on_detectFinished_detectThread(true);
	}
}


//����߳̽�����
void DetectUI::on_detectFinished_detectThread(bool qualified)
{
	//��ʾ���
	ui.label_indicator->setPixmap((qualified) ? lightOffIcon : lightOnIcon); //�л�ָʾ��
	ui.label_result->setText((qualified) ? QString::fromLocal8Bit("�ϸ�") : QString::fromLocal8Bit("���ϸ�"));
}