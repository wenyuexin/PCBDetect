#include "LaunchUI.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


LaunchUI::LaunchUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//成员变量初始化
	initThread = Q_NULLPTR; //系统初始化线程
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	cameraControler = Q_NULLPTR; //相机控制器
}

void LaunchUI::init()
{
	//多屏状态下选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//启动界面不显示鼠标
	this->setCursor(Qt::BlankCursor);
}

LaunchUI::~LaunchUI()
{
	delete initThread;
	initThread = Q_NULLPTR;
}


/***************** 初始化线程 ******************/

//开启初始化线程
void LaunchUI::runInitThread()
{
	//初始化线程
	initThread = new SysInitThread;
	initThread->setAdminConfig(adminConfig);
	initThread->setUserConfig(userConfig);
	initThread->setRuntimeParams(runtimeParams);
	initThread->setMotionControler(motionControler);
	initThread->setCameraControler(cameraControler);

	//初始化线程的信号连接
	connect(initThread, SIGNAL(sysInitStatus_initThread(QString)), this, SLOT(update_sysInitStatus_initThread(QString)));
	connect(initThread, SIGNAL(adminConfigError_initThread()), this, SLOT(on_adminConfigError_initThread()));
	connect(initThread, SIGNAL(userConfigError_initThread()), this, SLOT(on_userConfigError_initThread()));
	connect(initThread, SIGNAL(runtimeParamsError_initThread()), this, SLOT(on_runtimeParamsError_initThread()));
	connect(initThread, SIGNAL(initGraphicsView_initThread(int)), this, SLOT(on_initGraphicsView_initThread(int)));

	connect(initThread, SIGNAL(motionError_initThread(int)), this, SLOT(on_motionError_initThread(int)));
	connect(initThread, SIGNAL(cameraError_initThread()), this, SLOT(on_cameraError_initThread()));
	connect(initThread, SIGNAL(sysInitFinished_initThread()), this, SLOT(on_sysInitFinished_initThread()));

	//启动线程
	initThread->start(); 
}

//转发初始化界面中GraphicsView的信号
void LaunchUI::on_initGraphicsView_initThread(int launchCode)
{
	emit initGraphicsView_launchUI(launchCode);
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
	pcb::delay(600); //延时
	emit launchFinished_launchUI(adminConfig->getErrorCode());
}

//用户参数userConfig的初始化错误提示
void LaunchUI::on_userConfigError_initThread()
{
	userConfig->showMessageBox(this);
	pcb::delay(10); //延时

	if (userConfig->getErrorCode() == AdminConfig::ConfigFileMissing)
		userConfig->resetErrorCode(); //错误代码设为Uncheck
	update_sysInitStatus_initThread(pcb::chinese("用户参数获取结束  "));

	pcb::delay(600); //延时
	emit initGraphicsView_launchUI(userConfig->getErrorCode());
	pcb::delay(10); //延时
	emit launchFinished_launchUI(userConfig->getErrorCode());
}

//运行参数adminConfig的初始化错误提示
void LaunchUI::on_runtimeParamsError_initThread()
{
	runtimeParams->showMessageBox(this);
	pcb::delay(10); //延时
	update_sysInitStatus_initThread(pcb::chinese("系统参数获取结束  "));
	pcb::delay(600); //延时
	emit launchFinished_launchUI(runtimeParams->getErrorCode());
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
