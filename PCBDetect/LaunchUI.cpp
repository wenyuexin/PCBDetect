#include "LaunchUI.h"

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
	connect(initThread, SIGNAL(initializeStatus_initThread(QString)), this, SLOT(do_updateInitializeStatus_initThread(QString)));
	connect(initThread, SIGNAL(configError_initThread(int)), this, SLOT(on_configError_initThread(int)));
	connect(initThread, SIGNAL(initializeFinished_initThread()), this, SLOT(on_initializeFinished_initThread()));
}

//������ʾ
void LaunchUI::on_configError_initThread(int ErrorCode)
{
	QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"),
		QString::fromLocal8Bit("Ĭ�ϲ�����������������!  \nConfigCode: ") + QString::number(ErrorCode),
		QString::fromLocal8Bit("ȷ��"));
	Ui::delay(10); //��ʱ
	//showSettingUI(); //��ʾ���ý���
}

//���³�ʼ��״̬
void LaunchUI::do_updateInitializeStatus_initThread(QString status)
{
	QString time = QDateTime::currentDateTime().toString("hh:mm:ss ");
	ui.label_status->setText(time + status);
}

//��ʼ������
void LaunchUI::on_initializeFinished_initThread()
{
	do_updateInitializeStatus_initThread(QString::fromLocal8Bit("��ʼ������,ϵͳ������  "));
	Ui::delay(1000); //��ʱ
	emit launchFinished_launchUI();
}
