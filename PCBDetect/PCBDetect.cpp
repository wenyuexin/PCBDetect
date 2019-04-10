#include "PCBDetect.h"

using pcb::DetectConfig;
using pcb::AdminConfig;

PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//�˶�������
	motionControler = new MotionControler;//�˶�������
	motionControler->setAdminConfig(&adminConfig);
	motionControler->setDetectConfig(&detectConfig);

	//���������
	cameraControler = new CameraControler;
	cameraControler->setMaxCameraNum(&adminConfig.MaxCameraNum);
	cameraControler->setCameraNum(&detectParams.nCamera);

	//��������
	launcher = new LaunchUI;
	launcher->showFullScreen(); //��ʾlauncher
	launcher->setAdminConfig(&adminConfig);
	launcher->setDetectConfig(&detectConfig);
	launcher->setDetectParams(&detectParams);
	launcher->setMotionControler(motionControler);
	launcher->setCameraControler(cameraControler);
	launcher->runInitThread(); //���г�ʼ���߳�
	connect(launcher, SIGNAL(initGraphicsView_launchUI(int)), this, SLOT(on_initGraphicsView_launchUI(int)));
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//�������ý���
	settingUI = new SettingUI;
	settingUI->setAdminConfig(&adminConfig);
	settingUI->setDetectConfig(&detectConfig);
	settingUI->setDetectParams(&detectParams);
	settingUI->doConnect();//�ź�����
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnMainUI_settingUI()), this, SLOT(do_enableButtonsOnMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemState_settingUI()), this, SLOT(do_checkSystemState_settingUI()));
	
	//ģ����ȡ����
	templateUI = new TemplateUI;
	templateUI->setAdminConfig(&adminConfig);
	templateUI->setDetectConfig(&detectConfig);
	templateUI->setDetectParams(&detectParams);
	templateUI->setMotionControler(motionControler);
	templateUI->setCameraControler(cameraControler);
	templateUI->doConnect();//�ź�����
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//������
	detectUI = new DetectUI;
	detectUI->setAdminConfig(&adminConfig);
	detectUI->setDetectConfig(&detectConfig);
	detectUI->setDetectParams(&detectParams);
	detectUI->setMotionControler(motionControler);
	detectUI->setCameraControler(cameraControler);
	detectUI->doConnect();//�ź�����
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));
}

PCBDetect::~PCBDetect()
{
	delete launcher;
	delete settingUI;
	delete templateUI;
	delete detectUI;
	delete motionControler;
	delete cameraControler;
}


/**************** �������� *****************/

//��������
void PCBDetect::on_initGraphicsView_launchUI(int LaunchCode)
{
	if (!LaunchCode) { //��������
		settingUI->refreshSettingUI();//���²������ý������Ϣ
		templateUI->initGraphicsView();//ģ�������ͼ����ʾ�ĳ�ʼ��
		detectUI->initGraphicsView();//��������ͼ����ʾ�ĳ�ʼ��
	}
	else { //���ڴ���
		if (detectConfig.getErrorCode() != DetectConfig::Uncheck) {
			settingUI->refreshSettingUI();//���²������ý������Ϣ
		}
	}
}

//��������
void PCBDetect::on_launchFinished_launchUI(int LaunchCode)
{
	if (!LaunchCode) { //��������
		this->setPushButtonsToEnabled(true);//ģ����ȡ����ⰴ����Ϊ�ɵ��
	}
	else { //���ڴ���
		this->setPushButtonsToEnabled(false);//ģ����ȡ����ⰴ����Ϊ���ɵ��
	}

	//�رճ�ʼ�����棬��ʾ������
	this->showFullScreen();
	pcb::delay(10); //��ʱ
	launcher->close();
}


/*************** �����Ŀ�������Ӧ *****************/

//����
void PCBDetect::on_pushButton_set_clicked()
{
	showSettingUI();
}

void PCBDetect::on_pushButton_set2_clicked()
{
	showSettingUI();
}

//ģ����ȡ
void PCBDetect::on_pushButton_getTempl_clicked()
{
	showTemplateUI();
}

void PCBDetect::on_pushButton_getTempl2_clicked()
{
	showTemplateUI();
}

//���
void PCBDetect::on_pushButton_detect_clicked()
{
	showDetectUI();
}

void PCBDetect::on_pushButton_detect2_clicked()
{
	showDetectUI();
}

//�˳�
void PCBDetect::on_pushButton_exit_clicked()
{
	eixtDetectSystem();
}

void PCBDetect::on_pushButton_exit2_clicked()
{
	eixtDetectSystem();
}

//���ð����Ƿ�ɵ��
void PCBDetect::setPushButtonsToEnabled(bool code)
{
	//ģ����ȡ
	ui.pushButton_getTempl->setEnabled(code); 
	ui.pushButton_getTempl2->setEnabled(code);
	//���
	ui.pushButton_detect->setEnabled(code);
	ui.pushButton_detect2->setEnabled(code);
}

/****************** ���ý��� ******************/

void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	settingUI->hide(); //�������ý���
}

void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //��ʾ���ý���
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}

void PCBDetect::do_resetDetectSystem_settingUI(int code)
{
	//���³�ʼ���˶�����ģ��
	if (code & 0b000100000 == 0b000100000) {
		motionControler->initControler();
		while (motionControler->isRunning()) pcb::delay(50);
		motionControler->showMessageBox(settingUI);
	}

	//���³�ʼ�����
	if (code & 0b000010000 == 0b000010000) {
		cameraControler->start();
		while (cameraControler->isRunning()) pcb::delay(50);
		cameraControler->showMessageBox(settingUI);
	}

	//����ģ����ȡ���棬����ʼ�����е�ͼ����ʾ�Ŀռ�
	if (code & 0b000000100 == 0b000000100) {
		templateUI->resetTemplateUI();
		templateUI->initGraphicsView();
	}

	//���ü����棬����ʼ�����е�ͼ����ʾ�Ŀռ�
	if (code & 0b000000010 == 0b000000010) {
		detectUI->resetDetectUI();
		detectUI->initGraphicsView();
	}

	//���������ģ����ȡ��������ⰴ����Ϊ�ɵ��
	if (cameraControler->isReady() && motionControler->isReady()) {
		//������ģ���״̬�����������ð���
		this->setPushButtonsToEnabled(true);
		pcb::delay(10);
		//�����ý��淢�����ò����������ź�
		emit resetDetectSystemFinished_mainUI();
	}
}

//��ģ����ȡ��������ⰴ����Ϊ�ɵ��
void PCBDetect::do_enableButtonsOnMainUI_settingUI()
{
	this->setPushButtonsToEnabled(true);
}

//���ϵͳ�Ĺ���״̬
void PCBDetect::do_checkSystemState_settingUI()
{
	bool enableButtonsOnMainUI = true;

	//���ϵͳ����
	if (enableButtonsOnMainUI && !adminConfig.isValid()) {
		adminConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//����û�����
	if (enableButtonsOnMainUI && !detectConfig.isValid()) {
		detectConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//������
	if (enableButtonsOnMainUI && cameraControler->getErrorCode() != CameraControler::NoError) {
		cameraControler->showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//�ж��Ƿ�Ҫ�����������ϵ�ģ����ȡ�ͼ�ⰴ��
	if (enableButtonsOnMainUI) { //ϵͳ����
		this->setPushButtonsToEnabled(true);
	}
}

/***************** ģ����ȡ���� *****************/

void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	templateUI->hide(); //����ģ����ȡ����
}

void PCBDetect::showTemplateUI()
{
	templateUI->showFullScreen(); //��ʾģ����ȡ����
	pcb::delay(10); //��ʱ
	this->hide(); //����������
}


/******************* ������ ******************/

void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->showFullScreen(); //��ʾ������
	pcb::delay(10); //��ʱ
	detectUI->hide(); //���ؼ�����
}

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
	this->close();
	qApp->exit(0);
}

//���ڲ��Եĺ��� - �л������水���Ŀɵ��״̬
void PCBDetect::keyPressEvent(QKeyEvent *event)
{
	bool buttonsEnabled = false;
	switch (event->key())
	{
	case Qt::Key_Shift:
		if (ui.pushButton_detect->isEnabled())
			setPushButtonsToEnabled(false);
		else 
			setPushButtonsToEnabled(true);
	}
}