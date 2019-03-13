#include "LaunchUI.h"

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
	connect(initThread, SIGNAL(initializeStatus_initThread(QString)), this, SLOT(do_updateInitializeStatus_initThread(QString)));
	connect(initThread, SIGNAL(configError_initThread(int)), this, SLOT(on_configError_initThread(int)));
	connect(initThread, SIGNAL(initializeFinished_initThread()), this, SLOT(on_initializeFinished_initThread()));
}

//错误提示
void LaunchUI::on_configError_initThread(int ErrorCode)
{
	QMessageBox::warning(this, QString::fromLocal8Bit("提示"),
		QString::fromLocal8Bit("默认参数错误，请重新设置!  \nConfigCode: ") + QString::number(ErrorCode),
		QString::fromLocal8Bit("确定"));
	Ui::delay(10); //延时
	//showSettingUI(); //显示设置界面
}

//更新初始化状态
void LaunchUI::do_updateInitializeStatus_initThread(QString status)
{
	QString time = QDateTime::currentDateTime().toString("hh:mm:ss ");
	ui.label_status->setText(time + status);
}

//初始化结束
void LaunchUI::on_initializeFinished_initThread()
{
	do_updateInitializeStatus_initThread(QString::fromLocal8Bit("初始化结束,系统已启动  "));
	Ui::delay(1000); //延时
	emit launchFinished_launchUI();
}
