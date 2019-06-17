#include "PCBDetect.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	launcher = Q_NULLPTR;
	settingUI = Q_NULLPTR;
	extractUI = Q_NULLPTR;
	detectUI = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;

	//ѡ�������������Ǹ�������ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);//��������
	if (screenRect.width() < 1440 || screenRect.height() < 900) {
		screenRect = desktop->screenGeometry(0);//��������
	}
	runtimeParams.ScreenRect = screenRect;
	this->setGeometry(screenRect);

	//���ÿ�����ģʽ����ģʽ���������Ժ����Ա��ڵ���
	runtimeParams.DeveloperMode = false;

	//�˶�������
	motionControler = new MotionControler;//�˶�������
	motionControler->setAdminConfig(&adminConfig);
	motionControler->setUserConfig(&userConfig);
	motionControler->setRuntimeParams(&runtimeParams);

	//���������
	cameraControler = new CameraControler;
	cameraControler->setAdminConfig(&adminConfig);
	cameraControler->setRuntimeParams(&runtimeParams);

	//��������
	launcher = new LaunchUI();
	launcher->setAdminConfig(&adminConfig);
	launcher->setUserConfig(&userConfig);
	launcher->setRuntimeParams(&runtimeParams);
	launcher->setMotionControler(motionControler);
	launcher->setCameraControler(cameraControler);
	launcher->init();
	launcher->runInitThread(); //���г�ʼ���߳�
	connect(launcher, SIGNAL(initGraphicsView_launchUI(int)), this, SLOT(on_initGraphicsView_launchUI(int)));
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//�������ý���
	settingUI = new SettingUI();
	settingUI->setAdminConfig(&adminConfig);
	settingUI->setUserConfig(&userConfig);
	settingUI->setRuntimeParams(&runtimeParams);
	settingUI->setMotionControler(motionControler);
	settingUI->init();
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnMainUI_settingUI()), this, SLOT(do_enableButtonsOnMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemState_settingUI()), this, SLOT(do_checkSystemState_settingUI()));
	
	//ģ����ȡ����
	extractUI = new ExtractUI();
	extractUI->setAdminConfig(&adminConfig);
	extractUI->setUserConfig(&userConfig);
	extractUI->setRuntimeParams(&runtimeParams);
	extractUI->setMotionControler(motionControler);
	extractUI->setCameraControler(cameraControler);
	connect(extractUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//������
	detectUI = new DetectUI();
	detectUI->setAdminConfig(&adminConfig);
	detectUI->setUserConfig(&userConfig);
	detectUI->setRuntimeParams(&runtimeParams);
	detectUI->setMotionControler(motionControler);
	detectUI->setCameraControler(cameraControler);
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));

	//��ʾ��������
	launcher->showFullScreen(); 
}

PCBDetect::~PCBDetect()
{
	qDebug() << "~PCBDetect";
	delete launcher; 
	launcher = Q_NULLPTR;
	delete settingUI; 
	settingUI = Q_NULLPTR;
	delete extractUI; 
	extractUI = Q_NULLPTR;
	delete detectUI; 
	detectUI = Q_NULLPTR;
	delete motionControler; 
	motionControler = Q_NULLPTR;
	delete cameraControler; 
	cameraControler = Q_NULLPTR;
}


/**************** �������� *****************/

//�������ʼ������
void PCBDetect::on_initGraphicsView_launchUI(int launchCode)
{
	if (launchCode == -1) { //ϵͳ�������û������Ѿ�������ʼ��
		settingUI->refresh();//���²������ý���
	}
	else if (launchCode == 0) { //���в���Ҳ������ʼ�������в����඼������
		extractUI->init();//ģ����ȡ�����ʵ����ʼ��
		detectUI->init();//�������ʵ����ʼ��
	}
	else { //���ڴ���
		//�û����������ļ���ʧ��������Ĭ���ļ�
		if (userConfig.getErrorCode() != UserConfig::Unchecked) {
			settingUI->refresh();//���²������ý���
		}
	}
}

//��������
void PCBDetect::on_launchFinished_launchUI(int launchCode)
{
	if (!launchCode) { //��������
		this->setPushButtonsEnabled(true);//ģ����ȡ����ⰴ����Ϊ�ɵ��
		settingUI->refresh();//���²������ý���
	}
	else { //���ڴ���
		this->setPushButtonsEnabled(false);//ģ����ȡ����ⰴ����Ϊ���ɵ��
	}

	//�رճ�ʼ�����棬��ʾ������
	this->showFullScreen();
	pcb::delay(10); //��ʱ
	launcher->close();
}


/****************** ���ý��� ******************/

//������Ӧ - ����ͼ��
void PCBDetect::on_pushButton_set_clicked()
{
	this->showSettingUI();
}

//������Ӧ - ���ð���
void PCBDetect::on_pushButton_set2_clicked()
{
	this->showSettingUI();
}

//��ʾ���ý��棬����������
void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //��ʾ���ý���
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}

//��ʾ�����棬�������ý���
void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	settingUI->hide(); //�������ý���
}


//���ü��ϵͳ
void PCBDetect::do_resetDetectSystem_settingUI(int code)
{
	bool noError = true; //ϵͳ�Ƿ�����

	//���³�ʼ���˶�����ģ��
	if (noError && ((code & 0b000100000) > 0)) {
		motionControler->setOperation(MotionControler::InitControler);
		motionControler->start();
		while (motionControler->isRunning()) pcb::delay(50);
	}
	noError &= motionControler->isReady();

	//���³�ʼ�����
	if (noError && ((code & 0b000010000) > 0)) {
		cameraControler->setOperation(CameraControler::InitCameras);
		cameraControler->start();
		while (cameraControler->isRunning()) pcb::delay(50);
	}
	noError &= cameraControler->isReady();

	//���³�ʼ��ģ����ȡ����
	if (noError && ((code & 0b000000010) > 0)) {
		extractUI->init();
	}

	//���³�ʼ��ȱ�ݼ�����
	if (noError && ((code & 0b000000001) > 0)) {
		detectUI->init();
	}

	//����ģ��״̬��������ģ����ȡ��������ⰴ����Ϊ�ɵ��
	if (noError) { this->setPushButtonsEnabled(true); }
}

//��ģ����ȡ��������ⰴ����Ϊ�ɵ��
void PCBDetect::do_enableButtonsOnMainUI_settingUI()
{
	this->setPushButtonsEnabled(true);
}

//���ϵͳ�Ĺ���״̬
void PCBDetect::do_checkSystemState_settingUI()
{
	if (checkSystemState(true)) {
		//�����������ϵ�ģ����ȡ�ͼ�ⰴ��
		this->setPushButtonsEnabled(true);
	}
}

/* ���ϵͳ״̬
 * ���룺showError ��ֵΪtrue���򵱷���ģ����ڴ���ʱ��������ʾ
 * �����noErrorFlag ϵͳ����ģʽ�Ƿ񲻴��ڴ���
 */
bool PCBDetect::checkSystemState(bool showError)
{
	bool noErrorFlag = true; //ϵͳ�Ƿ�����

	noErrorFlag = checkParametricClasses(showError);

	//����˶��ṹ
	if (noErrorFlag && !motionControler->isReady()) {
		MotionControler::ErrorCode code = motionControler->getErrorCode();
		if (code == MotionControler::InitFailed || code == MotionControler::Unchecked) {
			if (showError) {
				//ѯ���Ƿ���Ҫ���³�ʼ��
				int choice = QMessageBox::warning(settingUI, pcb::chinese("����"),
					pcb::chinese("�˶�������δ��ʼ�����ʼ��ʧ�ܣ��Ƿ�����ִ�г�ʼ���� \n"),
					pcb::chinese("ȷ��"), pcb::chinese("ȡ��"));
				//�ж��Ƿ����³�ʼ��
				if (choice == 0) do_resetDetectSystem_settingUI(0b000100000);
			}
		}
		else { //�ͳ�ʼ���޹صĴ���
			if (showError) motionControler->showMessageBox(settingUI);
		}
	}
	noErrorFlag &= motionControler->isReady();

	//������
	if (noErrorFlag && !cameraControler->isReady()) {
		CameraControler::ErrorCode code = cameraControler->getErrorCode();
		if (code == CameraControler::InitFailed || code == CameraControler::Unchecked) {
			if (showError) {
				//ѯ���Ƿ���Ҫ���³�ʼ��
				int choice = QMessageBox::warning(settingUI, pcb::chinese("����"),
					pcb::chinese("���������δ��ʼ�����ʼ��ʧ�ܣ��Ƿ�����ִ�г�ʼ���� \n"),
					pcb::chinese("ȷ��"), pcb::chinese("ȡ��"));
				//�ж��Ƿ����³�ʼ��
				if (choice == 0) do_resetDetectSystem_settingUI(0b000010000);
			}
		}
		else { //�ͳ�ʼ���޹صĴ���
			if (showError) cameraControler->showMessageBox(settingUI);
		}
	}
	noErrorFlag &= cameraControler->isReady();

	return noErrorFlag;
}

/* ���������״̬
 * ���룺showError ��ֵΪtrue���򵱷��ֲ�������ڴ���ʱ��������ʾ
 * �����noErrorFlag ϵͳ���еĲ������Ƿ񲻴��ڴ���
 */
bool PCBDetect::checkParametricClasses(bool showError)
{
	bool noErrorFlag = true; 

	//���ϵͳ����
	if (noErrorFlag && !adminConfig.isValid(true)) {
		if (showError) adminConfig.showMessageBox(settingUI);
		noErrorFlag = false;
	}

	//����û�����
	if (noErrorFlag && !userConfig.isValid(&adminConfig)) {
		if (showError) userConfig.showMessageBox(settingUI);
		noErrorFlag = false;
	}

	//������в���
	if (noErrorFlag && !runtimeParams.isValid(RuntimeParams::Index_All_SysInit, true, &adminConfig)) {
		if (showError) runtimeParams.showMessageBox(settingUI);
		noErrorFlag = false;
	}

	return noErrorFlag;
}



/***************** ģ����ȡ���� *****************/

//ģ����ȡ - ͼ��
void PCBDetect::on_pushButton_getTempl_clicked()
{
	this->switchToExtractUI();
}

//ģ����ȡ - ����
void PCBDetect::on_pushButton_getTempl2_clicked()
{
	this->switchToExtractUI();
}

//���������л���ģ����ȡ����
void PCBDetect::switchToExtractUI()
{
	//�����˶������� ���������ĵ�����
	motionControler->setCaller(1);
	cameraControler->setCaller(1);

	//���ð���
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	if (!runtimeParams.DeveloperMode) {
		//�˶��ṹ��λ
		motionControler->setOperation(MotionControler::MotionReset);
		motionControler->start(); //��λ
		while (motionControler->isRunning()) pcb::delay(100);
		if (!motionControler->isReady()) {
			motionControler->showMessageBox(this);//������ʾ
		}
	}

	extractUI->resetExtractUI(); //������ȡ����
	if (!runtimeParams.DeveloperMode) extractUI->refreshCameraControler();//�����������
	this->showExtractUI(); //��ʾģ����ȡ����
}

//��ʾģ����ȡ���棬����������
void PCBDetect::showExtractUI()
{
	extractUI->showFullScreen(); //��ʾģ����ȡ����
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}

//��ʾ�����棬����ģ����ȡ����
void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	extractUI->hide(); //����ģ����ȡ����
}


/******************* ������ ******************/


//������Ӧ - ���ͼ��
void PCBDetect::on_pushButton_detect_clicked()
{
	this->switchToDetectUI();
}


//������Ӧ - ��ⰴ��
void PCBDetect::on_pushButton_detect2_clicked()
{
	this->switchToDetectUI();
}

//���������л���
void PCBDetect::switchToDetectUI()
{
	//�����˶������� ���������ĵ�����
	motionControler->setCaller(2);
	cameraControler->setCaller(2);

	//���ð���
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	if (!runtimeParams.DeveloperMode) {
		//�˶��ṹ��λ
		motionControler->setOperation(MotionControler::MotionReset);
		motionControler->start();
		while (motionControler->isRunning()) pcb::delay(100);
		if (!motionControler->isReady()) {
			motionControler->showMessageBox(this);
		}
	}

	detectUI->reset();//���ü�����
	if (!runtimeParams.DeveloperMode) detectUI->refreshCameraControler();//�������������
	this->showDetectUI();//��ʾ������
}

//��ʾ�����棬����������
void PCBDetect::showDetectUI()
{
	detectUI->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}

//��ʾ�����棬���ؼ�����
void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	detectUI->hide(); //���ؼ�����
}


/****************** �˳�ϵͳ ******************/

//�˳� - ͼ��
void PCBDetect::on_pushButton_exit_clicked()
{
	this->eixtDetectSystem();
}

//�˳� - ����
void PCBDetect::on_pushButton_exit2_clicked()
{
	this->eixtDetectSystem();
}

//�˳����ϵͳ
void PCBDetect::eixtDetectSystem()
{
	//ѯ���Ƿ���Ҫ�˳�ϵͳ
	int choice = QMessageBox::question(settingUI, pcb::chinese("ѯ��"),
		pcb::chinese("ȷ���˳�PCBȱ�ݼ��ϵͳ�� \n"),
		pcb::chinese("ȷ��"), pcb::chinese("ȡ��"));
	//�ж��Ƿ���Ҫ�˳�
	if (choice == 0) {
		//this->close();
		qApp->exit(0);
	}
}


/******************* �������� ******************/

//�����������ϵİ����Ƿ�ɵ��
void PCBDetect::setPushButtonsEnabled(bool enable, bool all)
{
	//ģ����ȡ
	ui.pushButton_getTempl->setEnabled(enable);
	ui.pushButton_getTempl2->setEnabled(enable);
	//���
	ui.pushButton_detect->setEnabled(enable);
	ui.pushButton_detect2->setEnabled(enable);

	if (all) {
		//����
		ui.pushButton_set->setEnabled(enable);
		ui.pushButton_set2->setEnabled(enable);
		//�˳�
		ui.pushButton_exit->setEnabled(enable);
		ui.pushButton_exit2->setEnabled(enable);
	}
}

//���Ժ��� - �л������水���Ŀɵ��״̬
void PCBDetect::keyPressEvent(QKeyEvent *event)
{
	//bool buttonsEnabled = false;
	switch (event->key())
	{
	case Qt::Key_Shift:
		switchDeveloperMode(); break;
	}
}

//�ڿ�����ģʽ�ͷǿ�����ģʽ֮���л�
void PCBDetect::switchDeveloperMode()
{
	//��������ڴ�����ֱ�ӷ���
	if (!checkParametricClasses(false)) {
		runtimeParams.DeveloperMode = false; 
		this->setPushButtonsEnabled(false); return;
	}

	//������ģʽ�ͱ�׼ģʽ֮����л�
	if (runtimeParams.DeveloperMode) {
		//ģʽ�л�����ʾ
		int choice = QMessageBox::information(this, pcb::chinese("��ʾ"),
			pcb::chinese("������ģʽ�ѹرգ� \n"),
			pcb::chinese("ȷ��"));
		runtimeParams.DeveloperMode = false; //�ر�
		//����ϵͳ״̬�ж��Ƿ�Ҫ����ģ����ȡ����ⰴ��
		this->setPushButtonsEnabled(checkSystemState(false));
	}
	else {
		//ģʽ�л���ѯ��
		int choice = QMessageBox::question(this, pcb::chinese("ѯ��"),
			pcb::chinese("�Ƿ�Ҫ���ÿ�����ģʽ�� \n"),
			pcb::chinese("ȷ��"), pcb::chinese("ȡ��"));
		if (choice == 0) { //���ÿ�����ģʽ
			runtimeParams.DeveloperMode = true; 
			this->setPushButtonsEnabled(true); //��ģ����ȡ����ⰴ����Ϊ�ɵ��
		}
	}
}