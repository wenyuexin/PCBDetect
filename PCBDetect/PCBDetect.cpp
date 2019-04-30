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
	templateUI = Q_NULLPTR;
	detectUI = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;

	//ѡ�������������Ǹ�������ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);//��������
	if (screenRect.width() < 1440 || screenRect.height() < 900) {
		screenRect = desktop->screenGeometry(0);//��������
	}
	this->setGeometry(screenRect);

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
	launcher = new LaunchUI(Q_NULLPTR, screenRect);
	launcher->setAdminConfig(&adminConfig);
	launcher->setUserConfig(&userConfig);
	launcher->setRuntimeParams(&runtimeParams);
	launcher->setMotionControler(motionControler);
	launcher->setCameraControler(cameraControler);
	launcher->runInitThread(); //���г�ʼ���߳�
	connect(launcher, SIGNAL(initGraphicsView_launchUI(int)), this, SLOT(on_initGraphicsView_launchUI(int)));
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//�������ý���
	settingUI = new SettingUI(Q_NULLPTR, screenRect);
	settingUI->setAdminConfig(&adminConfig);
	settingUI->setUserConfig(&userConfig);
	settingUI->setRuntimeParams(&runtimeParams);
	settingUI->doConnect();//�ź�����
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnMainUI_settingUI()), this, SLOT(do_enableButtonsOnMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemState_settingUI()), this, SLOT(do_checkSystemState_settingUI()));
	
	//ģ����ȡ����
	templateUI = new TemplateUI(Q_NULLPTR, screenRect);
	templateUI->setAdminConfig(&adminConfig);
	templateUI->setUserConfig(&userConfig);
	templateUI->setRuntimeParams(&runtimeParams);
	templateUI->setMotionControler(motionControler);
	templateUI->setCameraControler(cameraControler);
	templateUI->doConnect();//�ź�����
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//������
	detectUI = new DetectUI(Q_NULLPTR, screenRect);
	detectUI->setAdminConfig(&adminConfig);
	detectUI->setUserConfig(&userConfig);
	detectUI->setRuntimeParams(&runtimeParams);
	detectUI->setMotionControler(motionControler);
	detectUI->setCameraControler(cameraControler);
	detectUI->doConnect();//�ź�����
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));

	//��ʾ��������
	launcher->showFullScreen(); 
}

PCBDetect::~PCBDetect()
{
	qDebug() << "~PCBDetect";
	delete launcher; launcher = Q_NULLPTR;
	delete settingUI; settingUI = Q_NULLPTR;
	delete templateUI; templateUI = Q_NULLPTR;
	delete detectUI; detectUI = Q_NULLPTR;
	delete motionControler; motionControler = Q_NULLPTR;
	delete cameraControler; cameraControler = Q_NULLPTR;
}


/**************** �������� *****************/

//�������ʼ������
void PCBDetect::on_initGraphicsView_launchUI(int launchCode)
{
	if (launchCode == -1) { //ϵͳ�������û������Ѿ�������ʼ��
		settingUI->refreshSettingUI();//���²������ý������Ϣ
	}
	else if (launchCode == 0) { //�������� (���в���Ҳ������ʼ��)
		templateUI->initGraphicsView();//ģ�������ͼ����ʾ�ĳ�ʼ��
		detectUI->initGraphicsView();//��������ͼ����ʾ�ĳ�ʼ��
	}
	else { //���ڴ���
		//�û����������ļ���ʧ��������Ĭ���ļ�
		if (userConfig.getErrorCode() != UserConfig::Uncheck) {
			settingUI->refreshSettingUI();//���²������ý������Ϣ
		}
	}
}

//��������
void PCBDetect::on_launchFinished_launchUI(int launchCode)
{
	if (!launchCode) { //��������
		this->setPushButtonsEnabled(true);//ģ����ȡ����ⰴ����Ϊ�ɵ��
	}
	else { //���ڴ���
		this->setPushButtonsEnabled(false);//ģ����ȡ����ⰴ����Ϊ���ɵ��
	}

	//�رճ�ʼ�����棬��ʾ������
	this->showFullScreen();
	pcb::delay(10); //��ʱ
	launcher->close();
}


/*************** �����Ŀ�������Ӧ *****************/

//���� - ͼ��
void PCBDetect::on_pushButton_set_clicked()
{
	this->showSettingUI();
}

//���� - ����
void PCBDetect::on_pushButton_set2_clicked()
{
	this->showSettingUI();
}

//ģ����ȡ - ͼ��
void PCBDetect::on_pushButton_getTempl_clicked()
{
	//���ð���
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//�˶��ṹ��λ
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); //��λ
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);//������ʾ
	}

	//������ȡ����
	templateUI->resetTemplateUI();
	//�����������
	templateUI->refreshCameraControler();
	//��ʾģ����ȡ����
	this->showTemplateUI();
}

//ģ����ȡ - ����
void PCBDetect::on_pushButton_getTempl2_clicked()
{
	//���ð���
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//�˶��ṹ��λ
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); //��λ
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
	}

	//������ȡ����
	templateUI->resetTemplateUI();
	//����ģ����ȡ�Ĳ����������������
	templateUI->refreshCameraControler();
	//��ʾģ����ȡ����
	this->showTemplateUI();
}

//��� - ͼ��
void PCBDetect::on_pushButton_detect_clicked()
{
	//���ð���
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//��λ
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); 
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
	}

	//���ü�����
	detectUI->resetDetectUI();
	//���ݼ��Ĳ����������������
	detectUI->refreshCameraControler();
	//��ʾ������
	this->showDetectUI();
}


//��� - ����
void PCBDetect::on_pushButton_detect2_clicked()
{
	//���ð���
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//�˶��ṹ��λ
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); //��λ
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
	}

	//���ü�����
	detectUI->resetDetectUI();
	//���ݼ��Ĳ����������������
	detectUI->refreshCameraControler();
	//��ʾ������
	this->showDetectUI();
}


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


//���ð����Ƿ�ɵ��
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


/****************** ���ý��� ******************/

//��ʾ�����棬�������ý���
void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	settingUI->hide(); //�������ý���
}

//��ʾ���ý��棬����������
void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //��ʾ���ý���
	pcb::delay(10); //��ʱ
	this->hide(); //����������
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

	//����ģ����ȡ���棬����ʼ�����е�ͼ����ʾ�Ŀռ�
	if (noError && ((code & 0b000000010) > 0)) {
		templateUI->resetTemplateUI();
		templateUI->initGraphicsView();
	}

	//���ü����棬����ʼ�����е�ͼ����ʾ�Ŀռ�
	if (noError && ((code & 0b000000001) > 0)) {
		detectUI->resetDetectUI();
		detectUI->initGraphicsView();
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
	bool noError = true; //ϵͳ�Ƿ�����

	//���ϵͳ����
	if (noError && !adminConfig.isValid(true)) {
		adminConfig.showMessageBox(settingUI);
		noError = false;
	}

	//����û�����
	if (noError && !userConfig.isValid(&adminConfig)) {
		userConfig.showMessageBox(settingUI);
		noError = false;
	}

	//������в���
	if (noError && !runtimeParams.isValid(RuntimeParams::Index_All_SysInit, true, &adminConfig)) {
		runtimeParams.showMessageBox(settingUI);
		noError = false;
	}

	//����˶��ṹ
	if (noError && !motionControler->isReady()) {
		MotionControler::ErrorCode code = motionControler->getErrorCode();
		if (code == MotionControler::InitFailed || code == MotionControler::Uncheck) {
			//ѯ���Ƿ���Ҫ���³�ʼ��
			int choice = QMessageBox::warning(settingUI, pcb::chinese("����"),
				pcb::chinese("�˶�������δ��ʼ�����ʼ��ʧ�ܣ��Ƿ�����ִ�г�ʼ���� \n"),
				pcb::chinese("ȷ��"), pcb::chinese("ȡ��"));
			//�ж��Ƿ����³�ʼ��
			if (choice == 0) { do_resetDetectSystem_settingUI(0b000100000); } 
		}
		else { //�ͳ�ʼ���޹صĴ���
			motionControler->showMessageBox(settingUI);
		}
	}
	noError &= motionControler->isReady();

	//������
	if (noError && !cameraControler->isReady()) {
		CameraControler::ErrorCode code = cameraControler->getErrorCode();
		if (code == CameraControler::InitFailed || code == CameraControler::Uncheck) {
			//ѯ���Ƿ���Ҫ���³�ʼ��
			int choice = QMessageBox::warning(settingUI, pcb::chinese("����"),
				pcb::chinese("���������δ��ʼ�����ʼ��ʧ�ܣ��Ƿ�����ִ�г�ʼ���� \n"),
				pcb::chinese("ȷ��"), pcb::chinese("ȡ��"));
			//�ж��Ƿ����³�ʼ��
			if (choice == 0) { do_resetDetectSystem_settingUI(0b000010000); }
		}
		else { //�ͳ�ʼ���޹صĴ���
			cameraControler->showMessageBox(settingUI);
		}
	}
	noError &= cameraControler->isReady();

	//�ж��Ƿ�Ҫ�����������ϵ�ģ����ȡ�ͼ�ⰴ��
	if (noError) { this->setPushButtonsEnabled(true); }
}


/***************** ģ����ȡ���� *****************/

//��ʾ�����棬����ģ����ȡ����
void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	templateUI->hide(); //����ģ����ȡ����
}

//��ʾģ����ȡ���棬����������
void PCBDetect::showTemplateUI()
{
	templateUI->showFullScreen(); //��ʾģ����ȡ����
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}


/******************* ������ ******************/

//��ʾ�����棬���ؼ�����
void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	detectUI->hide(); //���ؼ�����
}

//��ʾ�����棬����������
void PCBDetect::showDetectUI()
{
	detectUI->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}


/******************* �������� ******************/

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

//���Ժ��� - �л������水���Ŀɵ��״̬
void PCBDetect::keyPressEvent(QKeyEvent *event)
{
	//bool buttonsEnabled = false;
	//switch (event->key())
	//{
	//case Qt::Key_Shift:
	//	if (ui.pushButton_detect->isEnabled())
	//		setPushButtonsEnabled(false);
	//	else 
	//		setPushButtonsEnabled(true);
	//}
}
