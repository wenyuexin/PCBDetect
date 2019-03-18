#include "LaunchUI.h"

using Ui::DetectConfig;

LaunchUI::LaunchUI(QWidget *parent, QPixmap *pixmap)
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
	initThread->setDetectConfig(config);//�����߳�
	initThread->start(); //�����߳�

	//��ʼ���̵߳��ź�����
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(configError_initThread(int)), this, SLOT(on_configError_initThread(int)));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));
}

//������ʾ
void LaunchUI::on_configError_initThread(int errorCode)
{
	QString message; //��ʾ��Ϣ
	switch (errorCode)
	{
	case DetectConfig::ConfigFileMissing:
		DetectConfig::showMessageBox(this, (DetectConfig::ErrorCode) errorCode);
		Ui::delay(10); //��ʱ
		initThread->start(); //�����߳�
		break;
	default:
		DetectConfig::showMessageBox(this, (DetectConfig::ErrorCode) errorCode);
		Ui::delay(10); //��ʱ
		update_sysInitStatus_initThread(QString::fromLocal8Bit("��ʷ�������û�ȡ����  "));
		Ui::delay(1000); //��ʱ
		break;
	}

	Ui::delay(10); //��ʱ
	emit launchFinished_launchUI(errorCode);
}

//���³�ʼ��״̬
void LaunchUI::update_sysInitStatus_initThread(QString status)
{
	QString time = QDateTime::currentDateTime().toString("hh:mm:ss ");
	ui.label_status->setText(time + status);
}

//��ʼ������
void LaunchUI::on_sysInitFinished_initThread()
{
	update_sysInitStatus_initThread(QString::fromLocal8Bit("��ʼ������,ϵͳ������  "));
	Ui::delay(1000); //��ʱ
	emit launchFinished_launchUI(0);
}
