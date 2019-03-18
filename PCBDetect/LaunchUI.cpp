#include "LaunchUI.h"

using Ui::DetectConfig;

LaunchUI::LaunchUI(QWidget *parent, QPixmap *pixmap)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//启动界面不显示鼠标
	this->setCursor(Qt::BlankCursor);
}

LaunchUI::~LaunchUI()
{
}


/***************** 初始化线程 ******************/

//开启初始化线程
void LaunchUI::runInitThread()
{
	//初始化线程
	initThread = new SysInitThread;
	initThread->setDetectConfig(config);//配置线程
	initThread->start(); //启动线程

	//初始化线程的信号连接
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(configError_initThread(int)), this, SLOT(on_configError_initThread(int)));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));
}

//错误提示
void LaunchUI::on_configError_initThread(int errorCode)
{
	QString message; //提示信息
	switch (errorCode)
	{
	case DetectConfig::ConfigFileMissing:
		DetectConfig::showMessageBox(this, (DetectConfig::ErrorCode) errorCode);
		Ui::delay(10); //延时
		initThread->start(); //重启线程
		break;
	default:
		DetectConfig::showMessageBox(this, (DetectConfig::ErrorCode) errorCode);
		Ui::delay(10); //延时
		update_sysInitStatus_initThread(QString::fromLocal8Bit("历史参数配置获取结束  "));
		Ui::delay(1000); //延时
		break;
	}

	Ui::delay(10); //延时
	emit launchFinished_launchUI(errorCode);
}

//更新初始化状态
void LaunchUI::update_sysInitStatus_initThread(QString status)
{
	QString time = QDateTime::currentDateTime().toString("hh:mm:ss ");
	ui.label_status->setText(time + status);
}

//初始化结束
void LaunchUI::on_sysInitFinished_initThread()
{
	update_sysInitStatus_initThread(QString::fromLocal8Bit("初始化结束,系统已启动  "));
	Ui::delay(1000); //延时
	emit launchFinished_launchUI(0);
}
