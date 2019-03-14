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
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(configError_initThread(int)), this, SLOT(on_configError_initThread(int)));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));
}

//������ʾ
void LaunchUI::on_configError_initThread(int ErrorCode)
{
	QString message; //��ʾ��Ϣ
	switch (ErrorCode)
	{
	case Ui::ConfigFileMissing:
		message = QString::fromLocal8Bit("config�ļ���ʧ��������Ĭ��config�ļ�!  \n")
			+ QString::fromLocal8Bit("ϵͳ�������»�ȡ���ò��� ...  \n");
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"),
			message + "ErrorCode: " + QString::number(ErrorCode),
			QString::fromLocal8Bit("ȷ��"));
		initThread->start(); //�����߳�
		break;
	case Ui::Invalid_SampleDirPath:
	case Ui::Invalid_TemplDirPath:
	case Ui::Invalid_OutputDirPath:
		message = QString::fromLocal8Bit("��ʷ�����������ڲ������ý�����������!  \n");
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"),
			message + "ErrorCode: " + QString::number(ErrorCode),
			QString::fromLocal8Bit("ȷ��"));
		Ui::delay(10); //��ʱ
		update_sysInitStatus_initThread(QString::fromLocal8Bit("��ʷ�������û�ȡ����  "));
		Ui::delay(1000); //��ʱ
		break;
	default:
		break;
	}

	Ui::delay(10); //��ʱ
	emit launchFinished_launchUI(ErrorCode);
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
