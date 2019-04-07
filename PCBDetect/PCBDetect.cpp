#include "PCBDetect.h"

using pcb::DetectConfig;
using pcb::AdminConfig;

PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//运动控制器
	motionControler = new MotionControler;//运动控制器
	motionControler->setDetectConfig(&detectConfig);
	motionControler->setAdminConfig(&adminConfig);

	//相机控制器
	cameraControler = new CameraControler;
	cameraControler->setMaxCameraNum(&adminConfig.MaxCameraNum);
	cameraControler->setCameraNum(&detectConfig.nCamera);

	//启动界面
	launcher = new LaunchUI;
	launcher->showFullScreen(); //显示launcher

	launcher->setDetectConfig(&detectConfig);
	launcher->setAdminConfig(&adminConfig);
	launcher->setCameraControler(cameraControler);
	launcher->runInitThread(); //运行初始化线程
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//参数设置界面
	settingUI = new SettingUI;
	settingUI->setDetectConfig(&detectConfig);
	settingUI->setAdminConfig(&adminConfig);
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnDetectMainUI_settingUI()), this, SLOT(do_enableButtonsOnDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemWorkingState_settingUI()), this, SLOT(do_checkSystemWorkingState_settingUI()));
	
	//模板提取界面
	templateUI = new TemplateUI;
	templateUI->setDetectConfig(&detectConfig);
	templateUI->setDetectParams(&detectParams);
	templateUI->setMotionControler(motionControler);
	templateUI->setCameraControler(cameraControler);
	templateUI->doConnect();//信号连接
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//检测界面
	detectUI = new DetectUI;
	detectUI->setDetectConfig(&detectConfig);
	detectUI->setDetectParams(&detectParams);
	//detectUI->setMotionControler(motionControler);
	//detectUI->setCameraControler(cameraControler);
	//detectUI->doConnect();//信号连接
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));
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


/**************** 启动界面 *****************/

//启动结束
void PCBDetect::on_launchFinished_launchUI(int LaunchCode)
{
	if (!LaunchCode) { //正常启动
		settingUI->refreshSettingUI();//更新参数设置界面的信息
		templateUI->initGraphicsView();//模板界面中图像显示的初始化
		detectUI->initGraphicsView();//检测界面中图像显示的初始化
		motionControler->initControler(); //初始化控制器
		this->setPushButtonsToEnabled(true);//模板提取、检测按键设为可点击
	}
	else { //存在错误
		settingUI->refreshSettingUI();//更新参数设置界面的信息
		this->setPushButtonsToEnabled(false);//模板提取、检测按键设为不可点击
	}

	//关闭初始化界面，显示主界面
	this->showFullScreen();
	pcb::delay(10); //延时
	launcher->close();
}


/*************** 按键的控制与响应 *****************/

//设置
void PCBDetect::on_pushButton_set_clicked()
{
	showSettingUI();
}

void PCBDetect::on_pushButton_set2_clicked()
{
	showSettingUI();
}

//模板提取
void PCBDetect::on_pushButton_getTempl_clicked()
{
	showTemplateUI();
}

void PCBDetect::on_pushButton_getTempl2_clicked()
{
	showTemplateUI();
}

//检测
void PCBDetect::on_pushButton_detect_clicked()
{
	showDetectUI();
}

void PCBDetect::on_pushButton_detect2_clicked()
{
	showDetectUI();
}

//退出
void PCBDetect::on_pushButton_exit_clicked()
{
	eixtDetectSystem();
}

void PCBDetect::on_pushButton_exit2_clicked()
{
	eixtDetectSystem();
}

//设置按键是否可点击
void PCBDetect::setPushButtonsToEnabled(bool code)
{
	ui.pushButton_getTempl->setEnabled(code);
	ui.pushButton_getTempl2->setEnabled(code);
	ui.pushButton_detect->setEnabled(code);
	ui.pushButton_detect2->setEnabled(code);
}

/****************** 设置界面 ******************/

void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //显示主界面
	pcb::delay(10); //延时
	settingUI->hide(); //隐藏设置界面
}

void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //显示设置界面
	pcb::delay(10); //延时
	this->hide(); //隐藏主界面
}

void PCBDetect::do_resetDetectSystem_settingUI(int code)
{
	//重置模板提取界面，并初始化其中的图像显示的空间
	if (code & 0b1000 == 0b1000) {
		templateUI->resetTemplateUI();
		templateUI->initGraphicsView();
	}

	//重置检测界面，并初始化其中的图像显示的空间
	if (code & 0b0100 == 0b0100) {
		detectUI->resetDetectUI();
		detectUI->initGraphicsView();
	}

	//重新初始化运动控制模块
	if (code & 0b0010 == 0b0010) {
		motionControler->start();
		while (motionControler->isRunning()) pcb::delay(50);
		motionControler->showMessageBox(settingUI);
	}

	//重新初始化相机
	if (code & 0b0001 == 0b0001) {
		cameraControler->start();
		while (cameraControler->isRunning()) pcb::delay(50);
		cameraControler->showMessageBox(settingUI);
	}

	//将主界面的模板提取按键、检测按键设为可点击
	if (cameraControler->getErrorCode() == CameraControler::NoError &&
		motionControler->getErrorCode() == MotionControler::NoError)
		//若各个模块的状态正常，则设置按键
		this->setPushButtonsToEnabled(true);
}

//将模板提取按键、检测按键设为可点击
void PCBDetect::do_enableButtonsOnDetectMainUI_settingUI()
{
	this->setPushButtonsToEnabled(true);
}

//检查系统的工作状态
void PCBDetect::do_checkSystemWorkingState_settingUI()
{
	bool enableButtonsOnMainUI = true;

	//检查用户参数
	if (!detectConfig.isValid()) {
		detectConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//检查系统参数
	if (enableButtonsOnMainUI && !adminConfig.isValid()) {
		adminConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//检查相机
	if (enableButtonsOnMainUI && cameraControler.getErrorCode() != CameraControler::NoError) {
		cameraControler.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//判断是否要启用主界面上的模板提取和检测按键
	if (enableButtonsOnMainUI) { //系统正常
		this->setPushButtonsToEnabled(true);
	}
}

/***************** 模板提取界面 *****************/

void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->showFullScreen(); //显示主界面
	pcb::delay(10); //延时
	templateUI->hide(); //隐藏模板提取界面
}

void PCBDetect::showTemplateUI()
{
	templateUI->showFullScreen(); //显示模板提取界面
	pcb::delay(10); //延时
	this->hide(); //隐藏主界面
}


/******************* 检测界面 ******************/

void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->showFullScreen(); //显示主界面
	pcb::delay(10); //延时
	detectUI->hide(); //隐藏检测界面
}

void PCBDetect::showDetectUI()
{
	detectUI->showFullScreen(); //显示检测界面
	pcb::delay(10); //延时
	this->hide(); //隐藏主界面
}


/******************* 其他函数 ******************/

//退出检测系统
void PCBDetect::eixtDetectSystem()
{
	this->close();
	qApp->exit(0);
}
