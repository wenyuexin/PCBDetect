#include "ExtractUI.h"

using pcb::UserConfig;
using pcb::RuntimeParams;
using cv::Mat;


ExtractUI::ExtractUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//��Ա������ʼ��
	adminConfig = Q_NULLPTR;
	userConfig = Q_NULLPTR;
	runtimeParams = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;
	serialNumberUI = Q_NULLPTR;
	templExtractor = Q_NULLPTR;
	extractThread = Q_NULLPTR;
	initCounter = 0;
}

void ExtractUI::init()
{
	//����״̬��ѡ�����������Ǹ�������ʾ
	this->setGeometry(runtimeParams->ScreenRect);

	//���ü�����ľ۽�����
	this->setFocusPolicy(Qt::ClickFocus);

	//������ȡ����
	this->resetExtractUI();

	//�Ի�ͼ�ؼ�GraphicsView�ĳ�ʼ������
	this->initGraphicsView();

	//��Ʒ���ʶ�����
	delete serialNumberUI;
	serialNumberUI = new SerialNumberUI();
	serialNumberUI->setAdminConfig(adminConfig);
	serialNumberUI->setRuntimeParams(runtimeParams);
	serialNumberUI->setCvMatArray(&cvmatSamples);
	serialNumberUI->setQPixmapArray(&qpixmapSamples);
	serialNumberUI->setMaskRoiWidgetsVisible(true);
	serialNumberUI->init();
	connect(serialNumberUI, SIGNAL(recognizeFinished_serialNumUI()), this, SLOT(on_recognizeFinished_serialNumUI()));
	connect(serialNumberUI, SIGNAL(switchImage_serialNumUI()), this, SLOT(on_switchImage_serialNumUI()));
	connect(serialNumberUI, SIGNAL(showPreviousUI_serialNumUI()), this, SLOT(do_showPreviousUI_serialNumUI()));
	connect(serialNumberUI, SIGNAL(getMaskRoiFinished_serialNumUI()), this, SLOT(on_getMaskRoiFinished_serialNumUI()));

	//ģ����ȡ��
	delete templExtractor;
	templExtractor = new TemplateExtractor;
	connect(templExtractor, SIGNAL(extractState_extractor(int)), this, SLOT(update_extractState_extractor(int)));

	//ģ����ȡ�߳�
	delete extractThread;
	extractThread = new ExtractThread();
	extractThread->setAdminConfig(adminConfig);
	extractThread->setUserConfig(userConfig);
	extractThread->setRuntimeParams(runtimeParams);
	extractThread->setSampleImages(&cvmatSamples);
	extractThread->setTemplateExtractor(templExtractor);
	extractThread->init();

	//�˶�����
	connect(motionControler, SIGNAL(moveToInitialPosFinished_motion(int)), this, SLOT(on_moveToInitialPosFinished_motion(int)), Qt::UniqueConnection);
	connect(motionControler, SIGNAL(moveForwardFinished_motion(int)), this, SLOT(on_moveForwardFinished_motion(int)), Qt::UniqueConnection);
	connect(motionControler, SIGNAL(motionResetFinished_motion(int)), this, SLOT(on_motionResetFinished_motion(int)), Qt::UniqueConnection);

	//�������
	connect(cameraControler, SIGNAL(initCamerasFinished_camera(int)), this, SLOT(on_initCamerasFinished_camera(int)), Qt::UniqueConnection);
	connect(cameraControler, SIGNAL(takePhotosFinished_camera(int)), this, SLOT(on_takePhotosFinished_camera(int)), Qt::UniqueConnection);
	
	//ת���߳�
	imgConvertThread.setCvMats(&cvmatSamples);
	imgConvertThread.setQPixmaps(&qpixmapSamples);
	imgConvertThread.setCurrentRow(&currentRow_show);
	imgConvertThread.setCvtCode(ImageConverter::CvMat2QPixmap);
	connect(&imgConvertThread, SIGNAL(convertFinished_convertThread()), this, SLOT(on_convertFinished_convertThread()), Qt::UniqueConnection);
}

ExtractUI::~ExtractUI()
{
	qDebug() << "~ExtractUI";

	deletePointersInItemArray(itemArray); //ɾ��ͼԪ�����е�ָ��
	deletePointersInCvMatArray(cvmatSamples); //ɾ��cvmatSamples�е�ָ��
	deletePointersInQPixmapArray(qpixmapSamples);//ɾ��qpixmapSamples�е�ָ��
	delete extractThread; //��ȡ�߳�
	extractThread = Q_NULLPTR;
	delete templExtractor; //ģ����ȡ��
	templExtractor = Q_NULLPTR;
	delete serialNumberUI; //��Ʒ���ʶ�����
	serialNumberUI = Q_NULLPTR;
}


/****************** ����ĳ�ʼ�������� *******************/

//�Ի�ͼ�ؼ�GraphicsView�ĳ�ʼ������
void ExtractUI::initGraphicsView()
{
	initItemGrid(itemGrid);//��ʼ��ͼԪ����
	initPointersInItemArray(itemArray);//��ʼ��itemArray
	initPointersInCvMatArray(cvmatSamples);//��ʼ��cvmatSamples
	initPointersInQPixmapArray(qpixmapSamples);//��ʼ��qpixmapSamples

	//��ʼ���������ڼ��ӳ�������״̬�ı���
	currentRow_show = -1; //��ʾ�к�
	runtimeParams->currentRow_extract = -1; //����к�
	eventCounter = 0; //�¼�������

	//��ͼ�ؼ�������
	ui.graphicsView->setFocusPolicy(Qt::NoFocus); //���þ۽�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //����ˮƽ������
	ui.graphicsView->setScene(&scene); //����ͼ����ӳ���
	ui.graphicsView->centerOn(sceneSize.width() / 2, 0); //���ô�ֱ��������λ��
}

//����ģ����ȡ����
void ExtractUI::resetExtractUI()
{
	serialNumberUI->reset();//�������ʶ�����

	ui.label_status->setText(""); //���״̬��
	removeItemsFromGraphicsScene(); //�Ƴ��������Ѿ����ص�ͼԪ
	deletePointersInItemArray(itemArray); //ɾ��ͼԪ�����е�ָ��
	deletePointersInCvMatArray(cvmatSamples); //ɾ��cvmatSamples�е�ָ��
	deletePointersInQPixmapArray(qpixmapSamples);//ɾ��qpixmapSamples�е�ָ��
	currentRow_show = -1; //��ʾ�кŵĸ�λ
	runtimeParams->currentRow_extract = -1; //��ȡ�кŵĸ�λ
	eventCounter = 0; //�¼�������
	ui.graphicsView->centerOn(0, 0); //��ֱ������λ
	qApp->processEvents();
}

//��������
void ExtractUI::setPushButtonsEnabled(bool enable)
{
	ui.pushButton_start->setEnabled(enable);
	ui.pushButton_return->setEnabled(enable);
}


/********* ͼԪ���������ͼ�����ĳ�ʼ����ɾ���Ȳ��� ***********/

//��ʼ��ͼԪ����
void ExtractUI::initItemGrid(pcb::ItemGrid &grid)
{
	//��������
	int nCamera = runtimeParams->nCamera; //�������
	int nPhotographing = runtimeParams->nPhotographing; //�������
	QString SampleDirPath = userConfig->SampleDirPath; //sample�ļ��е�·�� 

	//�����ܼ��
	QSizeF totalSpacing; //�ܼ��
	totalSpacing.setWidth(ItemSpacing * (nCamera + 1)); //����ܿ��
	totalSpacing.setHeight(ItemSpacing * (nPhotographing + 1)); //����ܸ߶�

	//����ͼԪ�ߴ�
	QSizeF viewSize = ui.graphicsView->size(); //��ͼ�ߴ�
	itemSize.setWidth(1.0 * (viewSize.width() - totalSpacing.width()) / nCamera); //ͼԪ���
	qreal itemAspectRatio = adminConfig->ImageAspectRatio; //��߱�
	itemSize.setHeight(1.0 * itemSize.width() / itemAspectRatio); //ͼԪ�߶�

	//���㳡���ߴ�
	sceneSize = totalSpacing;
	sceneSize += QSizeF(itemSize.width()*nCamera, itemSize.height()*nPhotographing);
	scene.setSceneRect(1, 1, sceneSize.width() + 2, sceneSize.height() + 2);

	//���ɻ�ͼ����
	QSizeF spacingBlock = QSizeF(ItemSpacing, ItemSpacing);
	gridSize = itemSize + spacingBlock; //ÿ������ĳߴ�

	//�ж�itemGrid�Ƿ�ִ�й���ʼ��
	if (grid.size() > 0) grid.clear();

	//��ʼ����ֵ
	for (int iPhotographing = 0; iPhotographing < nPhotographing; iPhotographing++) { //��
		QList<QPointF> posList;
		for (int iCamera = 0; iCamera < nCamera; iCamera++) { //��
			QPointF pos(ItemSpacing, ItemSpacing);
			pos += QPointF(gridSize.width()*iCamera, gridSize.height()*iPhotographing); //(x,y)
			posList.append(pos);
		}
		grid.append(posList);
	}
}

//��ʼ��ͼԪ�����е�ָ�� - ItemArray
void ExtractUI::initPointersInItemArray(pcb::ItemArray &items)
{
	if (items.size() > 0) {
		deletePointersInItemArray(items);//��ִ�й�init����������deleteָ��
	}
	
	items.resize(runtimeParams->nPhotographing); //���ô�С
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //��
		items[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) { //��
			items[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//ɾ��ͼԪ�����е�ָ�� - ItemArray
void ExtractUI::deletePointersInItemArray(pcb::ItemArray &items)
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
void ExtractUI::initPointersInCvMatArray(pcb::CvMatArray &cvmats)
{
	//��ִ�й�init����������deleteָ��
	if (cvmats.size() > 0) {
		deletePointersInCvMatArray(cvmats);
	}

	//����������������մ����Ծ�����г�ʼ��
	cvmats.resize(runtimeParams->nPhotographing);
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //��
		cvmats[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
			cvmats[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//ɾ������ͼ�������е�ָ�� - CvMatArray
void ExtractUI::deletePointersInCvMatArray(pcb::CvMatArray &cvmats)
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
void ExtractUI::initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
{
	if (qpixmaps.size() > 0) {
		deletePointersInQPixmapArray(qpixmaps);//��ִ�й�init����������deleteָ��
	}
	
	qpixmaps.resize(runtimeParams->nPhotographing);
	for (int iPhotographing = 0; iPhotographing < runtimeParams->nPhotographing; iPhotographing++) { //��
		qpixmaps[iPhotographing].resize(runtimeParams->nCamera);
		for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
			qpixmaps[iPhotographing][iCamera] = Q_NULLPTR;
		}
	}
}

//ɾ������ͼ�������е�ָ�� - QPixmapArray
void ExtractUI::deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps)
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
void ExtractUI::removeItemsFromGraphicsScene()
{
	QList<QGraphicsItem *> itemList = scene.items();
	for (int i = 0; i < itemList.size(); i++) {
		scene.removeItem(itemList[i]);
	}
}


/***************** ������Ӧ *****************/

//��ʼ��ȡ�µ�PCB��
void ExtractUI::on_pushButton_start_clicked()
{
	if (runtimeParams->currentRow_extract == -1 && !extractThread->isRunning()) {
		ui.label_status->setText(pcb::chinese("��ʼ����"));
		this->setPushButtonsEnabled(false); //���ð���
		this->resetExtractUI(); //����ģ����ȡ��ģ��
		
		if (runtimeParams->DeveloperMode) { //������ģʽ
			serialNumberUI->reset(); //����
			serialNumberUI->showFullScreen(); //ֱ����ʾ��Ʒ��Ž���
			pcb::delay(10);//�ӳ�
			this->hide(); //���ؼ�����
		}
		else { //��׼ģʽ
			if (runtimeParams->currentRow_detect == -1 && !extractThread->isRunning()) {
				//�˶�����ʼ����λ��
				ui.label_status->setText(pcb::chinese("�˶��ṹǰ����"));
				qApp->processEvents();
				motionControler->setOperation(MotionControler::MoveToInitialPos);
				motionControler->start();
			}
		}
	}
}

//����
void ExtractUI::on_pushButton_return_clicked()
{
	//���ð���
	this->setPushButtonsEnabled(false);
	//����ģ����ȡ���棬��ջ�������
	this->resetExtractUI(); 

	//�˶��ṹ��λ
	//if (!runtimeParams->DeveloperMode) {
	//	ui.label_status->setText(pcb::chinese("�˶��ṹ��λ��"));
	//	qApp->processEvents();
	//	motionControler->setOperation(MotionControler::ResetControler);
	//	motionControler->start();
	//	while (motionControler->isRunning()) pcb::delay(100);
	//	if (!motionControler->isReady()) {
	//		motionControler->showMessageBox(this);
	//		pcb::delay(10);
	//	}
	//}

	//���ð���
	this->setPushButtonsEnabled(true);
	//���ͷ����ź�
	emit showDetectMainUI(); 
}


/***************** ��ȡ�ⲿ�ź� ******************/

//��ʱʹ���û����̰���ģ���ⲿ�ź�
void ExtractUI::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_PageUp:
		qDebug() << "==================== Key_PageUp";
		break;
	case Qt::Key_PageDown:
		qDebug() << "==================== Key_PageDown";
		break;
	case Qt::Key_Up:
		qDebug() << "==================== Up";
		break;
	case Qt::Key_Down:
		qDebug() << "==================== Down";
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return:
		qDebug() << "==================== Enter";
		break;
	default:
		break;
	}
}


/***************** ��ͼ����ʾ����ȡ *****************/

//��ȡ����������һ���ͼ - ֱ�Ӵ�Ӳ���϶�ͼ
void ExtractUI::readSampleImages()
{
	clock_t t1 = clock();
	qDebug() << "====================" << pcb::chinese("��ȡ����")
		<< "( currentRow_show =" << currentRow_show << ")" << endl;

	//�����кź�״̬��
	ui.label_status->setText(pcb::chinese("���ڶ�ȡ��") +
		QString::number(currentRow_show + 1) +
		pcb::chinese("�з�ͼ"));//����״̬
	qApp->processEvents();

	//��ȡĿ¼�µ�����ͼ��
	QDir dir(runtimeParams->currentSampleDir);
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
		if (iPhotographing < 0 || iPhotographing >= runtimeParams->nPhotographing) continue;
		if (iCamera < 0 || iCamera >= runtimeParams->nCamera) continue;

		QString filepath = fileList.at(i).absoluteFilePath(); //����ͼ��·��
		cv::Mat img = cv::imread(filepath.toStdString(), cv::IMREAD_COLOR);
		cvmatSamples[currentRow_show][iCamera] = new cv::Mat(img);
	}
	
	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("��ͼ��ȡ������")
		<< (t2 - t1) << "ms ( currentRow_show =" << currentRow_show << ")" << endl;

	//ͼ������ת��
	imgConvertThread.start();
}


//��ʾ����������һ���ͼ��ͼ����ʾ�����е�һ�У�
void ExtractUI::showSampleImages()
{
	qDebug() << "====================" << pcb::chinese("��ʾ��ͼ")
		<< " ( currentRow_show =" << currentRow_show << ")" << endl;
	clock_t t1 = clock();

	QSize _itemSize(itemSize.width(), itemSize.height());
	if (ItemSpacing == 0) _itemSize += QSize(2, 2); //��ֹ���ַ�϶
	for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
		QPixmap scaledImg = (*qpixmapSamples[currentRow_show][iCamera]).scaled(_itemSize, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //����ͼԪ
		item->setPos(itemGrid[currentRow_show][iCamera]); //ͼԪ����ʾλ��
		itemArray[currentRow_show][iCamera] = item; //����ͼ����
	}

	//����������������һ��ͼԪ
	for (int iCamera = 0; iCamera < runtimeParams->nCamera; iCamera++) {
		scene.addItem(itemArray[currentRow_show][iCamera]);
	}

	//��ͼ��ʾ����
	int y_SliderPos = itemGrid[currentRow_show][0].y() + itemSize.height() / 2;
	ui.graphicsView->centerOn(sceneSize.width() / 2, y_SliderPos); //���ô�ֱ��������λ��
	ui.graphicsView->show();//��ʾ

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("��ͼ��ʾ������") << (t2 - t1)
		<< "ms ( currentRow_show =" << currentRow_show << ")" << endl;
}


/******************** �ַ�ʶ�� ********************/

//ͨ�����˫�����ַ�ʶ�����
void ExtractUI::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (currentRow_show == -1) return;//��û����ʾ��Ӧ������ͼ��ֱ�ӷ���
	if (event->button() == Qt::RightButton) return;//�����Ҽ����

	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	QRect graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	QPoint mousePosition(event->x(), event->y());
	if (!graphicsViewRect.contains(mousePosition)) return;

	//�ж�����������ĸ���ͼ
	//QPoint relativePos = mousePosition - graphicsViewPos; //���λ��
	//int gridRowIdx = (int) (relativePos.y() / gridSize.height());//���λ���ڵڼ���
	//int gridColIdx = (int) (relativePos.x() / gridSize.width());//���λ���ڵڼ���

	QPointF posInScene = ui.graphicsView->mapToScene(event->pos());//���λ������ڳ���������
	int gridRowIdx = (int)(posInScene.y() / gridSize.height());//������λ���ڵڼ���
	int gridColIdx = (int)(posInScene.x() / gridSize.width());//������λ���ڵڼ���
	
	if (gridRowIdx <= currentRow_show && qpixmapSamples[gridRowIdx][gridColIdx] != Q_NULLPTR) {
		serialNumberUI->showSampleImage(gridRowIdx, gridColIdx);
		pcb::delay(3);//�ӳ�
		serialNumberUI->showFullScreen();//��ʾ���ʶ�����
		pcb::delay(10);//�ӳ�
		this->hide();
	}
}

//�����ʶ������ò�Ʒ���֮��
void ExtractUI::on_recognizeFinished_serialNumUI()
{
	//�ж���ֱ�Ӵӱ��ض�ͼ�����������ȡͼ��
	if (runtimeParams->DeveloperMode) { //������ģʽ
		//�����ļ����ж�ȡԭʼͼ��
		runtimeParams->currentSampleDir = userConfig->SampleDirPath + "/"
			+ runtimeParams->sampleModelNum + "/"
			+ runtimeParams->sampleBatchNum + "/" + runtimeParams->sampleNum;
		if (!QFileInfo(runtimeParams->currentSampleDir).exists()) {
			runtimeParams->showMessageBox(this, RuntimeParams::Invalid_serialNum);
			return;
		}

		//��ʾ�µ�������
		if (currentRow_show + 1 < runtimeParams->nPhotographing) { 
			currentRow_show += 1; //������ʾ�к�
			readSampleImages(); //��ͼ - �൱���������		
		}
	}
	else { //��׼ģʽ
		if (eventCounter >= 1 && !extractThread->isRunning())
			extractTemplateImages(); //��ȡ
	}
}

//��ʾ���ʶ��������һ������ - ģ����ȡ����
void ExtractUI::do_showPreviousUI_serialNumUI()
{
	if (!runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)
		&& runtimeParams->DeveloperMode) 
	{
		this->setPushButtonsEnabled(true); //���ð���
	}

	this->showFullScreen();
	pcb::delay(10);//�ӳ�
	serialNumberUI->hide();
}

//��Ĥ�����ȡ����
void ExtractUI::on_getMaskRoiFinished_serialNumUI()
{
	if (runtimeParams->maskRoi_tl.x() < 0 || runtimeParams->maskRoi_tl.y() < 0 ||
		runtimeParams->maskRoi_br.x() < 0 || runtimeParams->maskRoi_br.y() < 0)
	{
		//ui.label_status->setText(pcb::chinese("�������ʶ�����\n")
		//	+ pcb::chinese("ȷ����Ĥ����"));
		//qApp->processEvents();
		//pcb::delay(10); //�ӳ�
		return;
	}

	if (eventCounter >= 1 && !extractThread->isRunning() &&
		runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false))
	{ 
		extractTemplateImages(); //��ȡ
	}
}

/******************** �˶����� ********************/

//����ʼ������λ��
void ExtractUI::on_moveToInitialPosFinished_motion(int errorcode)
{
	//if (motionControler->getCaller() != 1) return;

	////����˶��ṹ��״̬
	//if (!motionControler->isReady()) {
	//	motionControler->showMessageBox(this);
	//	pcb::delay(10); return;
	//}

	//���������������
	if (currentRow_show + 1 < runtimeParams->nPhotographing) {
		currentRow_show += 1; //������ʾ�к�

		ui.label_status->setText(pcb::chinese("���������") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("�з�ͼ"));//����״̬
		qApp->processEvents();

		cameraControler->start(); //����
	}
}

//�˶��ṹǰ������
void ExtractUI::on_moveForwardFinished_motion(int errorcode)
{
	if (motionControler->getCaller() != 1) return;

	//����˶��ṹ��״̬
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//���������������
	if (currentRow_show + 1 < runtimeParams->nPhotographing) {
		currentRow_show += 1; //������ʾ�к�

		ui.label_status->setText(pcb::chinese("���������") +
			QString::number(currentRow_show + 1) + 
			pcb::chinese("�з�ͼ"));//����״̬
		qApp->processEvents();

		cameraControler->start(); //����
	}
}

//��λ����
void ExtractUI::on_motionResetFinished_motion(int errorcode)
{
	if (motionControler->getCaller() != 1) return;

	//��λʧ��
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
		pcb::delay(10); return;
	}

	//�Ѿ��������з�ͼ����δ��ʼ��ȡ
	if (currentRow_show == runtimeParams->nPhotographing - 1
		&& runtimeParams->currentRow_extract == -1)
	{
		//��������Ч������Ȼ������ʾ
		if (!runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)) {
			ui.label_status->setText(pcb::chinese("�������ʶ�����\n")
				+ pcb::chinese("��ȡ��Ʒ���"));
			qApp->processEvents();
		}

		//���ÿ�ʼ���ͷ��ؼ�
		if (!motionControler->isRunning()) {
			this->setPushButtonsEnabled(true);
		}
	}
}


/******************** ������� ********************/

//��������������Ĳ���
void ExtractUI::refreshCameraControler()
{
	if (cameraControler->getCaller() != 1) return;

	cameraControler->setCurrentRow(&currentRow_show);//�����к�
	cameraControler->setCvMatSamples(&cvmatSamples);//����ͼ��
	cameraControler->setOperation(CameraControler::TakePhotos);//�����������
}

//�����ʼ������
void ExtractUI::on_initCamerasFinished_camera(int)
{
	if (cameraControler->getCaller() != 1) return;
}

//����������
void ExtractUI::on_takePhotosFinished_camera(int)
{
	if (cameraControler->getCaller() != 1) return;

	//����ͼ������ת���߳�
	imgConvertThread.start();
}


/******************** ͼ��ת���߳� ********************/

//ͼ��ת���������ڽ�������ʾͼ��Ȼ����ȡģ��
void ExtractUI::on_convertFinished_convertThread()
{
	//�����¼�������
	eventCounter += 1;

	//����״̬��
	if (runtimeParams->DeveloperMode) {
		ui.label_status->setText(pcb::chinese("��") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("�ж�ȡ����"));
	}
	else {
		ui.label_status->setText(pcb::chinese("��") +
			QString::number(currentRow_show + 1) +
			pcb::chinese("���������"));
	}
	qApp->processEvents();
	pcb::delay(10); //�ӳ�

	//�ڽ�������ʾ����ͼ
	this->showSampleImages(); 

	//��ʾ������֮ǰ������е�ṹ�˶�
	pcb::delay(10); //�ӳ�
	if (!runtimeParams->DeveloperMode) {
		if (currentRow_show + 1 < runtimeParams->nPhotographing) {
			ui.label_status->setText(pcb::chinese("�˶��ṹǰ����"));
			qApp->processEvents();
			motionControler->setOperation(MotionControler::MoveForward);
			motionControler->start(); //�˶��ṹǰ��
		}
		else { //��ǰPCB������
			ui.label_status->setText(pcb::chinese("�˶��ṹ��λ��"));
			qApp->processEvents();
			motionControler->setOperation(MotionControler::MotionReset);
			motionControler->start(); //�˶��ṹ��λ
		}
	}
	else { //������ģʽ
	}

	//����״̬��
	runtimeParams->checkValidity(RuntimeParams::Index_All_SerialNum);//����Ʒ��ŵ���Ч��
	if (!runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false) && 
		!runtimeParams->DeveloperMode)
	{
		//������ʶ����治����ʾ״̬������������ʾ�У��������ʾ
		ui.label_status->setText(pcb::chinese("�������ʶ�����\n")
			+ pcb::chinese("��ȡ��Ʒ���"));
		qApp->processEvents();
		return;
	}

	//�ж��Ƿ�ִ����ȡ����
	if (runtimeParams->isValid(RuntimeParams::Index_All_SerialNum, false)
		&& eventCounter >= 1 && !extractThread->isRunning()) 
	{
		//��������һ��
		if (runtimeParams->currentRow_extract = runtimeParams->nPhotographing - 1) {
			//�����Ĥ������Ч������ʾȷ����Ĥ����
			if (runtimeParams->maskRoi_tl.x() < 0 || runtimeParams->maskRoi_tl.y() < 0 ||
				runtimeParams->maskRoi_br.x() < 0 || runtimeParams->maskRoi_br.y() < 0)
			{
				ui.label_status->setText(pcb::chinese("�������ʶ�����\nȷ����Ĥ����"));
				qApp->processEvents(); return;
			}
		}
		extractTemplateImages(); //��ȡ
	}
}


/******************** ģ����ȡ�߳� ********************/

//��ȡ��ǰ��һ������ͼ��
void ExtractUI::extractTemplateImages()
{
	//���ÿ�ʼ���ͷ��ؼ�
	this->setPushButtonsEnabled(false);

	//������ȡ�к�
	runtimeParams->currentRow_extract += 1;

	//����״̬��
	ui.label_status->setText(pcb::chinese("������ȡ��") +
		QString::number(runtimeParams->currentRow_extract + 1) + 
		pcb::chinese("��ģ��"));
	qApp->processEvents();

	//��������߳�
	extractThread->start();
}

//��ȡ���������״̬
void ExtractUI::update_extractState_extractor(int state)
{
	//����ȡ��һ��ͼ��
	if ((TemplateExtractor::ExtractState)state == TemplateExtractor::Finished) { 
		//�����¼�������
		eventCounter -= 1; 

		//����״̬��
		ui.label_status->setText(pcb::chinese("��") +
			QString::number(runtimeParams->currentRow_extract + 1) +
			pcb::chinese("����ȡ����"));
		qApp->processEvents();

		//��ʾ�µ�������
		if (runtimeParams->DeveloperMode && currentRow_show < runtimeParams->nPhotographing - 1) {
			currentRow_show += 1; //������ʾ�к�
			readSampleImages(); //��ͼ - �൱���������		
		}

		//����Ƿ���δ������¼�
		while (extractThread->isRunning()) pcb::delay(50); //�ȴ���ȡ�߳̽���
		if (runtimeParams->currentRow_extract == runtimeParams->nPhotographing - 1) { //��ǰPCB��ȡ����
			runtimeParams->currentRow_extract += 1;
			this->setPushButtonsEnabled(true); //���ð���
		}
		else { //��ǰPCBδ��ȡ��
			if (eventCounter <= 0) return;

			if (runtimeParams->currentRow_extract + 1 == runtimeParams->nPhotographing - 1) {
				if (runtimeParams->maskRoi_tl.x() < 0 || runtimeParams->maskRoi_tl.y() < 0 ||
					runtimeParams->maskRoi_br.x() < 0 || runtimeParams->maskRoi_br.y() < 0)
				{
					ui.label_status->setText(pcb::chinese("�������ʶ�����\nȷ����Ĥ����"));
					qApp->processEvents(); return;
				}
			}
			extractTemplateImages(); //��ȡ��һ�з�ͼ
		}
	}
}
