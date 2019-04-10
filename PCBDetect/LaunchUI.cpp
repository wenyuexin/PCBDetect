#include "LaunchUI.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


LaunchUI::LaunchUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//�������治��ʾ���
	this->setCursor(Qt::BlankCursor);

	//��Ա������ʼ��
	initThread = Q_NULLPTR; //ϵͳ��ʼ���߳�
	adminConfig = Q_NULLPTR; //ϵͳ����
	detectConfig = Q_NULLPTR; //�û�����
	detectParams = Q_NULLPTR; //���в���
	cameraControler = Q_NULLPTR; //���������
}

LaunchUI::~LaunchUI()
{
}


/***************** ��ʼ���߳� ******************/

//������ʼ���߳�
void LaunchUI::runInitThread()
{
	//��ʼ���߳�
	initThread = new SysInitThread;
	initThread->setAdminConfig(adminConfig);
	initThread->setDetectConfig(detectConfig);
	initThread->setDetectParams(detectParams);
	initThread->setMotionControler(motionControler);
	initThread->setCameraControler(cameraControler);

	//��ʼ���̵߳��ź�����
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(adminConfigError_initThread()), this, SLOT(on_adminConfigError_initThread()));
	connect(initThread, SIGNAL(detectConfigError_initThread()), this, SLOT(on_detectConfigError_initThread()));
	connect(initThread, SIGNAL(detectParamsError_initThread()), this, SLOT(on_detectParamsError_initThread()));
	connect(initThread, SIGNAL(motionError_initThread(int)), this, SLOT(on_motionError_initThread(int)));
	connect(initThread, SIGNAL(cameraError_initThread()), this, SLOT(on_cameraError_initThread()));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));

	//�����߳�
	initThread->start(); 
}


/***************** ������ʾ ******************/

//ϵͳ����adminConfig�ĳ�ʼ��������ʾ
void LaunchUI::on_adminConfigError_initThread()
{
	adminConfig->showMessageBox(this);
	pcb::delay(10); //��ʱ
	if (adminConfig->getErrorCode() == AdminConfig::ConfigFileMissing)
		adminConfig->resetErrorCode(); //���������ΪUncheck
	update_sysInitStatus_initThread(pcb::chinese("ϵͳ������ȡ����  "));
	pcb::delay(1000); //��ʱ
	emit launchFinished_launchUI(adminConfig->getErrorCode());
}

//�û�����detectConfig�ĳ�ʼ��������ʾ
void LaunchUI::on_detectConfigError_initThread()
{
	detectConfig->showMessageBox(this);
	pcb::delay(10); //��ʱ

	if (detectConfig->getErrorCode() == AdminConfig::ConfigFileMissing)
		detectConfig->resetErrorCode(); //���������ΪUncheck
	update_sysInitStatus_initThread(pcb::chinese("�û�������ȡ����  "));

	pcb::delay(1000); //��ʱ
	emit launchFinished_launchUI(detectConfig->getErrorCode());
}


//���в���adminConfig�ĳ�ʼ��������ʾ
void LaunchUI::on_detectParamsError_initThread()
{
	detectParams->showMessageBox(this);
	pcb::delay(10); //��ʱ
	update_sysInitStatus_initThread(pcb::chinese("ϵͳ������ȡ����  "));
	pcb::delay(1000); //��ʱ
	emit launchFinished_launchUI(detectParams->getErrorCode());
}

//�˶��ṹ�ĵĳ�ʼ��������ʾ
void LaunchUI::on_motionError_initThread(int code)
{
	//��������
	if (code == MotionControler::Default) 
		motionControler->showMessageBox(this); 
	else
		motionControler->showMessageBox(this, (MotionControler::ErrorCode) code);
	pcb::delay(10); //��ʱ
	update_sysInitStatus_initThread(pcb::chinese("�˶��ṹ��ʼ������    "));
	pcb::delay(600); //��ʱ
	emit launchFinished_launchUI(motionControler->getErrorCode());
}

//����ĵĳ�ʼ��������ʾ
void LaunchUI::on_cameraError_initThread()
{
	cameraControler->showMessageBox(this); //��������
	pcb::delay(10); //��ʱ
	update_sysInitStatus_initThread(pcb::chinese("�����ʼ������    "));
	pcb::delay(600); //��ʱ
	emit launchFinished_launchUI(cameraControler->getErrorCode());
}


/******************* �������������״̬��Ϣ *******************/

//�������������ϵĳ�ʼ��״̬
void LaunchUI::update_sysInitStatus_initThread(QString status)
{
	QString time = QDateTime::currentDateTime().toString("hh:mm:ss ");
	ui.label_status->setText(time + status);
	qApp->processEvents();
}

//��ʼ������
void LaunchUI::on_sysInitFinished_initThread()
{
	update_sysInitStatus_initThread(pcb::chinese("��ʼ������,ϵͳ������  "));
	pcb::delay(1000); //��ʱ
	emit launchFinished_launchUI(0);
}
