#include "TemplateUI.h"

using pcb::DetectConfig;
using pcb::DetectParams;
using cv::Mat;


TemplateUI::TemplateUI(QWidget *parent, QRect &screenRect)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ�����������Ǹ�������ʾ
	this->setGeometry(screenRect);

	//���ü�����ľ۽�����
	this->setFocusPolicy(Qt::ClickFocus);

	//��Ա������ʼ��
	adminConfig = Q_NULLPTR;
	detectConfig = Q_NULLPTR;
	detectParams = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;

	//��Ʒ���ʶ�����
	connect(&serialNumberUI, SIGNAL(recognizeFinished_serialNumUI()), this, SLOT(on_recognizeFinished_serialNumUI()));
	connect(&serialNumberUI, SIGNAL(switchImage_serialNumUI()), this, SLOT(on_switchImage_serialNumUI()));
	connect(&serialNumberUI, SIGNAL(showPreviousUI_serialNumUI()), this, SLOT(do_showPreviousUI_serialNumUI()));

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
	connect(motionControler, SIGNAL(resetControlerFinished_motion(int)), this, SLOT(on_resetControlerFinished_motion()));
	connect(motionControler, SIGNAL(moveToInitialPosFinished_motion(int)), this, SLOT(on_moveToInitialPosFinished_motion()));
	connect(motionControler, SIGNAL(moveForwardFinished_motion(int)), this, SLOT(on_moveForwardFinished_motion()));
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
	serialNumberUI.setAdminConfig(adminConfig);
	serialNumberUI.setDetectParams(detectParams);
	serialNumberUI.setCvMatArray(&cvmatSamples);
	serialNumberUI.setQPixmapArray(&qpixmapSamples);

	//������ȡ�߳�
	templThread->setAdminConfig(adminConfig);
	templThread->setDetectConfig(detectConfig);
	templThread->setDetectParams(detectParams);
	templThread->setSampleImages(&cvmatSamples);
	templThread->initTemplateExtractor();

	//��ͼ�ؼ�������
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //���þ۽�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView->setScene(&scene); //����ͼ����ӳ���
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //���ô�ֱ��������λ��
}

//����ģ����ȡ����
void TemplateUI::resetTemplateUI()
{
	serialNumberUI.resetSerialNumberUI();//�������ʶ�����

	ui.label_status->setText(""); //���״̬��
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

//��������
void TemplateUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_start->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}


/********* ͼԪ���������ͼ�����ĳ�ʼ����ɾ���Ȳ��� ***********/

//��ʼ��ͼԪ����
void TemplateUI::initItemGrid(pcb::ItemGrid &grid)
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
	if (grid.size() > 0) grid.clear();

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
void TemplateUI::initPointersInItemArray(pcb::ItemArray &items)
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
void TemplateUI::deletePointersInItemArray(pcb::ItemArray &items)
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
void TemplateUI::initPointersInCvMatArray(pcb::CvMatArray &cvmats)
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
void TemplateUI::deletePointersInCvMatArray(pcb::CvMatArray &cvmats)
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
void TemplateUI::initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
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
void TemplateUI::deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
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
		ui.label_status->setText(pcb::chinese("��ʼ����"));
		this->setPushButtonsEnabled(false); //���ð���

		//����ģ����ȡ��ģ��
		this->resetTemplateUI();
		//�˶�����ʼ����λ��
		motionControler->setOperation(MotionControler::MoveToInitialPos);
		motionControler->start();
	}
}

//����
void TemplateUI::on_pushButton_return_clicked()
{
	//���ð���
	this->setPushButtonsEnabled(false);
	//����ģ����ȡ���棬��ջ�������
	this->resetTemplateUI(); 

	//�˶��ṹ��λ
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); 
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10);
	}

	//���ð���
	this->setPushButtonsEnabled(true);
	//���ͷ����ź�
	emit showDetectMainUI(); 
}


/***************** ��ȡ�ⲿ�ź� ******************/

//��ʱʹ���û����̰���ģ���ⲿ�ź�
void TemplateUI::keyPressEvent(QKeyEvent *event)
{
	detectParams->serialNum = "01010004"; //��Ʒ���
	detectParams->parseSerialNum(); //��Ʒ��Ž���

	switch (event->key())
	{
	case Qt::Key_PageUp:
		qDebug() << "===== Key_PageUp";
	case Qt::Key_PageDown:
		qDebug() << "===== Key_PageDown";
		break;
	case Qt::Key_Up:
		qDebug() << "===== Up";
		break;
	case Qt::Key_Down:
		qDebug() << "===== Down";
		if (detectParams->currentRow_extract == detectParams->nPhotographing - 1 && !templThread->isRunning())
			resetTemplateUI();//����ģ����ȡ��ģ��

		//!imgConvertThread.isRunning()
		if (currentRow_show + 1 < detectParams->nPhotographing && true) { //ֱ����ʾ�µ�������
			currentRow_show += 1; //������ʾ�к�
			qDebug() << "currentRow_show  - " << currentRow_show;

			ui.label_status->setText(pcb::chinese("���������") +
				QString::number(currentRow_show + 1) +
				pcb::chinese("��ͼ��"));//����״̬
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


/***************** ��ͼ����ʾ����ȡ *****************/

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
	qDebug() << "readSampleImages2: " << (t2 - t1) << "ms ( currentRow -" << currentRow_show << ")";

	//ͼ������ת��
	imgConvertThread.start();
}


//��ʾ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void TemplateUI::showSampleImages()
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
void TemplateUI::mouseDoubleClickEvent(QMouseEvent *event)
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
	int gridColIdx = (int) ceil(relativePos.x() / gridSize.width());//���λ���ڵڼ���
	int gridRowIdx = (int) ceil(relativePos.y() / gridSize.height());//���λ���ڵڼ���
	
	if (true && gridRowIdx <= currentRow_show) {
		serialNumberUI.showSampleImage(gridRowIdx, gridColIdx);
		pcb::delay(3);//�ӳ�
		serialNumberUI.showFullScreen();//��ʾ���ʶ�����
		pcb::delay(10);//�ӳ�
		this->hide();
	}
}

//�����ʶ������ò�Ʒ���֮��
void TemplateUI::on_recognizeFinished_serialNumUI()
{
	//�ж��Ƿ�ִ�м�����
	if (detectParams->isValid(DetectParams::Index_All_SerialNum, true)
		&& eventCounter >= 1 && !templThread->isRunning())
	{
		extractTemplateImages(); //��ȡ
	}
}

//��ʾ���ʶ��������һ������
void TemplateUI::do_showPreviousUI_serialNumUI()
{
	this->showFullScreen();
	pcb::delay(10);//�ӳ�
	serialNumberUI.hide();
}


/******************** �˶����� ********************/

//�˶��ṹǰ������
void TemplateUI::on_moveForwardFinished_motion()
{
	//����˶��ṹ��״̬
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

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

//����ʼ������λ��
void TemplateUI::on_moveToInitialPosFinished_motion()
{
	//����˶��ṹ��״̬
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

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

//��λ����
void TemplateUI::on_resetControlerFinished_motion()
{
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10);
	}
}


/******************** ������� ********************/

//��������������Ĳ���
void TemplateUI::refreshCameraControler()
{
	cameraControler->setCurrentRow(&currentRow_show);//�����к�
	cameraControler->setCvMatSamples(&cvmatSamples);//����ͼ��
	cameraControler->setOperation(CameraControler::TakePhotos);//�����������
}

//�����ʼ������
void TemplateUI::on_initCamerasFinished_camera(int)
{
}

//����������
void TemplateUI::on_takePhotosFinished_camera(int)
{
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
	ui.label_status->setText(pcb::chinese("��") +
		QString::number(currentRow_show + 1) +
		pcb::chinese("���������"));
	qApp->processEvents();
	pcb::delay(10); //�ӳ�

	//�ڽ�������ʾ����ͼ
	clock_t t1 = clock();
	this->showSampleImages(); 
	clock_t t2 = clock();
	qDebug() << "showSampleImages: " << (t2 - t1) 
		<< "ms ( currentRow =" << currentRow_show << ")";

	//����״̬��
	if (!detectParams->isValid(DetectParams::Index_All_SerialNum, false)) {
		ui.label_status->setText(pcb::chinese("�������ʶ�����\n")
			+ pcb::chinese("��ȡ��Ʒ���"));
		qApp->processEvents();
		pcb::delay(10); //�ӳ�
	}

	//��ʾ������֮ǰ������е�ṹ�˶�
	pcb::delay(10); //�ӳ�
	if (currentRow_show + 1 < detectParams->nPhotographing) {
		//�˶��ṹǰ��
		motionControler->setOperation(MotionControler::MoveForward);
		motionControler->start(); 
	}
	else { //��ǰPCB����
		//�˶��ṹ��λ
		motionControler->setOperation(MotionControler::ResetControler);
		motionControler->start();
		
		//�����ʱ��û��ʼ��ȡ������Ե�����ذ���
		if (detectParams->currentRow_extract == -1) {
			ui.pushButton_return->setEnabled(true);
		}
	}

	//�ж��Ƿ�ִ�м�����
	if (detectParams->isValid(DetectParams::Index_All_SerialNum, false)
		&& eventCounter >= 1 && !templThread->isRunning()) 
	{
		extractTemplateImages(); //��ȡ
	}
}


/******************** ģ����ȡ�߳� ********************/

//��ȡ��ǰ��һ������ͼ��
void TemplateUI::extractTemplateImages()
{
	//���÷��ؼ�
	ui.pushButton_return->setEnabled(false);

	//������ȡ�к�
	detectParams->currentRow_extract += 1;

	//����״̬��
	ui.label_status->setText(pcb::chinese("������ȡ��") +
		QString::number(detectParams->currentRow_extract + 1) + 
		pcb::chinese("��ģ��"));
	qApp->processEvents();

	//��������߳�
	templThread->start();
}

//��ȡ���������״̬
void TemplateUI::update_extractState_extractor(int state)
{
	//����ȡ��һ��ͼ��
	if ((TemplateExtractor::ExtractState)state == TemplateExtractor::Finished) { 
		//�����¼�������
		eventCounter -= 1; 

		//����״̬��
		ui.label_status->setText(pcb::chinese("��") +
			QString::number(detectParams->currentRow_extract + 1) +
			pcb::chinese("����ȡ����"));
		qApp->processEvents();

		//����Ƿ���δ������¼�
		while (templThread->isRunning()) pcb::delay(50); //�ȴ��߳̽���
		if (detectParams->currentRow_extract == detectParams->nPhotographing - 1) { //��ǰPCB��ȡ����
			detectParams->currentRow_extract = detectParams->nPhotographing;
			this->setPushButtonsEnabled(true); //���ð���
		}
		else { //��ǰPCBδ��ȡ��
			if (eventCounter > 0) extractTemplateImages(); //��ȡ��һ�з�ͼ
		}
	}
}
