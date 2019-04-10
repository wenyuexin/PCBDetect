#include "LaunchUI.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


LaunchUI::LaunchUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//启动界面不显示鼠标
	this->setCursor(Qt::BlankCursor);

	//成员变量初始化
	initThread = Q_NULLPTR; //系统初始化线程
	adminConfig = Q_NULLPTR; //系统参数
	detectConfig = Q_NULLPTR; //用户参数
	detectParams = Q_NULLPTR; //运行参数
	cameraControler = Q_NULLPTR; //相机控制器
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
	initThread->setAdminConfig(adminConfig);
	initThread->setDetectConfig(detectConfig);
	initThread->setDetectParams(detectParams);
	initThread->setMotionControler(motionControler);
	initThread->setCameraControler(cameraControler);

	//初始化线程的信号连接
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(adminConfigError_initThread()), this, SLOT(on_adminConfigError_initThread()));
	connect(initThread, SIGNAL(detectConfigError_initThread()), this, SLOT(on_detectConfigError_initThread()));
	connect(initThread, SIGNAL(detectParamsError_initThread()), this, SLOT(on_detectParamsError_initThread()));
	connect(initThread, SIGNAL(motionError_initThread(int)), this, SLOT(on_motionError_initThread(int)));
	connect(initThread, SIGNAL(cameraError_initThread()), this, SLOT(on_cameraError_initThread()));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));

	//启动线程
	initThread->start(); 
}


/***************** 错误提示 ******************/

//系统参数adminConfig的初始化错误提示
void LaunchUI::on_adminConfigError_initThread()
{
	adminConfig->showMessageBox(this);
	pcb::delay(10); //延时
	if (adminConfig->getErrorCode() == AdminConfig::ConfigFileMissing)
		adminConfig->resetErrorCode(); //错误代码设为Uncheck
	update_sysInitStatus_initThread(pcb::chinese("系统参数获取结束  "));
	pcb::delay(1000); //延时
	emit launchFinished_launchUI(adminConfig->getErrorCode());
}

//用户参数detectConfig的初始化错误提示
void LaunchUI::on_detectConfigError_initThread()
{
	detectConfig->showMessageBox(this);
	pcb::delay(10); //延时

	if (detectConfig->getErrorCode() == AdminConfig::ConfigFileMissing)
		detectConfig->resetErrorCode(); //错误代码设为Uncheck
	update_sysInitStatus_initThread(pcb::chinese("用户参数获取结束  "));

	pcb::delay(1000); //延时
	emit launchFinished_launchUI(detectConfig->getErrorCode());
}


//运行参数adminConfig的初始化错误提示
void LaunchUI::on_detectParamsError_initThread()
{
	detectParams->showMessageBox(this);
	pcb::delay(10); //延时
	update_sysInitStatus_initThread(pcb::chinese("系统参数获取结束  "));
	pcb::delay(1000); //延时
	emit launchFinished_launchUI(detectParams->getErrorCode());
}

//运动结构的的初始化错误提示
void LaunchUI::on_motionError_initThread(int code)
{
	//弹窗警告
	if (code == MotionControler::Default) 
		motionControler->showMessageBox(this); 
	else
		motionControler->showMessageBox(this, (MotionControler::ErrorCode) code);
	pcb::delay(10); //延时
	update_sysInitStatus_initThread(pcb::chinese("运动结构初始化结束    "));
	pcb::delay(600); //延时
	emit launchFinished_launchUI(motionControler->getErrorCode());
}

//相机的的初始化错误提示
void LaunchUI::on_cameraError_initThread()
{
	cameraControler->showMessageBox(this); //弹窗警告
	pcb::delay(10); //延时
	update_sysInitStatus_initThread(pcb::chinese("相机初始化结束    "));
	pcb::delay(600); //延时
	emit launchFinished_launchUI(cameraControler->getErrorCode());
}


/******************* 更新启动界面的状态信息 *******************/

//更新启动界面上的初始化状态
void LaunchUI::update_sysInitStatus_initThread(QString status)
{
	QString time = QDateTime::currentDateTime().toString("hh:mm:ss ");
	ui.label_status->setText(time + status);
	qApp->processEvents();
}

//初始化结束
void LaunchUI::on_sysInitFinished_initThread()
{
	update_sysInitStatus_initThread(pcb::chinese("初始化结束,系统已启动  "));
	pcb::delay(1000); //延时
	emit launchFinished_launchUI(0);
}
