#include "PCBDetect.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


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
	motionControler->setDetectConfig(&detectConfig);
	motionControler->setDetectParams(&detectParams);

	//���������
	cameraControler = new CameraControler;
	cameraControler->setAdminConfig(&adminConfig);
	cameraControler->setDetectParams(&detectParams);

	//��������
	launcher = new LaunchUI(Q_NULLPTR, screenRect);
	launcher->setAdminConfig(&adminConfig);
	launcher->setDetectConfig(&detectConfig);
	launcher->setDetectParams(&detectParams);
	launcher->setMotionControler(motionControler);
	launcher->setCameraControler(cameraControler);
	launcher->runInitThread(); //���г�ʼ���߳�
	connect(launcher, SIGNAL(initGraphicsView_launchUI(int)), this, SLOT(on_initGraphicsView_launchUI(int)));
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//�������ý���
	settingUI = new SettingUI(Q_NULLPTR, screenRect);
	settingUI->setAdminConfig(&adminConfig);
	settingUI->setDetectConfig(&detectConfig);
	settingUI->setDetectParams(&detectParams);
	settingUI->doConnect();//�ź�����
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnMainUI_settingUI()), this, SLOT(do_enableButtonsOnMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemState_settingUI()), this, SLOT(do_checkSystemState_settingUI()));
	
	//ģ����ȡ����
	templateUI = new TemplateUI(Q_NULLPTR, screenRect);
	templateUI->setAdminConfig(&adminConfig);
	templateUI->setDetectConfig(&detectConfig);
	templateUI->setDetectParams(&detectParams);
	templateUI->setMotionControler(motionControler);
	templateUI->setCameraControler(cameraControler);
	templateUI->doConnect();//�ź�����
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//������
	detectUI = new DetectUI(Q_NULLPTR, screenRect);
	detectUI->setAdminConfig(&adminConfig);
	detectUI->setDetectConfig(&detectConfig);
	detectUI->setDetectParams(&detectParams);
	detectUI->setMotionControler(motionControler);
	detectUI->setCameraControler(cameraControler);
	detectUI->doConnect();//�ź�����
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));

	//��ʾ��������
	launcher->showFullScreen(); 
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
		if (detectConfig.getErrorCode() != DetectConfig::Uncheck) {
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

//����
void PCBDetect::on_pushButton_set_clicked()
{
	this->showSettingUI();
}

void PCBDetect::on_pushButton_set2_clicked()
{
	this->showSettingUI();
}

//ģ����ȡ
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

	//�����������
	templateUI->refreshCameraControler();
	//��ʾģ����ȡ����
	this->showTemplateUI();
}

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
	//����ģ����ȡ�Ĳ����������������
	templateUI->refreshCameraControler();
	//��ʾģ����ȡ����
	this->showTemplateUI();
}

//���
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

	//���ݼ��Ĳ����������������
	detectUI->refreshCameraControler();
	//��ʾ������
	this->showDetectUI();
}

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
	//���ݼ��Ĳ����������������
	detectUI->refreshCameraControler();
	//��ʾ������
	this->showDetectUI();
}

//�˳�
void PCBDetect::on_pushButton_exit_clicked()
{
	this->eixtDetectSystem();
}

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
	if (code & 0b000000010 == 0b000000010) {
		templateUI->resetTemplateUI();
		templateUI->initGraphicsView();
	}

	//���ü����棬����ʼ�����е�ͼ����ʾ�Ŀռ�
	if (code & 0b000000001 == 0b000000001) {
		detectUI->resetDetectUI();
		detectUI->initGraphicsView();
	}

	//���������ģ����ȡ��������ⰴ����Ϊ�ɵ��
	if (cameraControler->isReady() && motionControler->isReady()) {
		//������ģ���״̬�����������ð���
		this->setPushButtonsEnabled(true);
		pcb::delay(10);
		//�����ý��淢�����ò����������ź�
		emit resetDetectSystemFinished_mainUI();
	}
}

//��ģ����ȡ��������ⰴ����Ϊ�ɵ��
void PCBDetect::do_enableButtonsOnMainUI_settingUI()
{
	this->setPushButtonsEnabled(true);
}

//���ϵͳ�Ĺ���״̬
void PCBDetect::do_checkSystemState_settingUI()
{
	bool enableButtonsOnMainUI = true;

	//���ϵͳ����
	if (enableButtonsOnMainUI && !adminConfig.isValid(true)) {
		adminConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//����û�����
	if (enableButtonsOnMainUI && !detectConfig.isValid(&adminConfig)) {
		detectConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//������в���
	if (enableButtonsOnMainUI && 
		!detectParams.isValid(DetectParams::Index_All_SysInit, true, &adminConfig)) 
	{
		detectParams.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//����˶��ṹ
	if (enableButtonsOnMainUI && !motionControler->isReady()) {
		motionControler->showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//������
	if (enableButtonsOnMainUI && !cameraControler->isReady()) {
		cameraControler->showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//�ж��Ƿ�Ҫ�����������ϵ�ģ����ȡ�ͼ�ⰴ��
	if (enableButtonsOnMainUI) { //ϵͳ����
		this->setPushButtonsEnabled(true);
	}
}

/***************** ģ����ȡ���� *****************/

void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
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
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
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

//���Ժ��� - �л������水���Ŀɵ��״̬
void PCBDetect::keyPressEvent(QKeyEvent *event)
{
	bool buttonsEnabled = false;
	switch (event->key())
	{
	case Qt::Key_Shift:
		if (ui.pushButton_detect->isEnabled())
			setPushButtonsEnabled(false);
		else 
			setPushButtonsEnabled(true);
	}
}