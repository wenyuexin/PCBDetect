#include "LaunchUI.h"

using pcb::DetectConfig;

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
	initThread->setDetectConfig(detectConfig);
	initThread->setAdminConfig(adminConfig);
	initThread->setCameraControler(cameraControler);

	//��ʼ���̵߳��ź�����
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(detectConfigError_initThread()), this, SLOT(on_detectConfigError_initThread()));
	connect(initThread, SIGNAL(adminConfigError_initThread()), this, SLOT(on_adminConfigError_initThread()));
	connect(initThread, SIGNAL(cameraError_initThread()), this, SLOT(on_cameraError_initThread()));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));

	//�����߳�
	initThread->start(); 
}

//�û�����detectConfig�ĳ�ʼ��������ʾ
void LaunchUI::on_detectConfigError_initThread()
{
	detectConfig->showMessageBox(this);
	pcb::delay(10); //��ʱ
	update_sysInitStatus_initThread(pcb::chinese("�û�������ȡ����  "));
	pcb::delay(1000); //��ʱ
	emit launchFinished_launchUI(detectConfig->getErrorCode());
}

//ϵͳ����adminConfig�ĳ�ʼ��������ʾ
void LaunchUI::on_adminConfigError_initThread()
{
	adminConfig->showMessageBox(this);
	pcb::delay(10); //��ʱ
	update_sysInitStatus_initThread(pcb::chinese("ϵͳ������ȡ����  "));
	pcb::delay(1000); //��ʱ
	emit launchFinished_launchUI(adminConfig->getErrorCode());
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
