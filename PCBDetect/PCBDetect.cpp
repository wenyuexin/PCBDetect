#include "PCBDetect.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	launcher = Q_NULLPTR;
	settingUI = Q_NULLPTR;
	templateUI = Q_NULLPTR;
	detectUI = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;

	//选择在主屏或者是副屏上显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);//副屏区域
	if (screenRect.width() < 1440 || screenRect.height() < 900) {
		screenRect = desktop->screenGeometry(0);//主屏区域
	}
	this->setGeometry(screenRect);

	//运动控制器
	motionControler = new MotionControler;//运动控制器
	motionControler->setAdminConfig(&adminConfig);
	motionControler->setDetectConfig(&detectConfig);
	motionControler->setDetectParams(&detectParams);

	//相机控制器
	cameraControler = new CameraControler;
	cameraControler->setAdminConfig(&adminConfig);
	cameraControler->setDetectParams(&detectParams);

	//启动界面
	launcher = new LaunchUI(Q_NULLPTR, screenRect);
	launcher->setAdminConfig(&adminConfig);
	launcher->setDetectConfig(&detectConfig);
	launcher->setDetectParams(&detectParams);
	launcher->setMotionControler(motionControler);
	launcher->setCameraControler(cameraControler);
	launcher->runInitThread(); //运行初始化线程
	connect(launcher, SIGNAL(initGraphicsView_launchUI(int)), this, SLOT(on_initGraphicsView_launchUI(int)));
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//参数设置界面
	settingUI = new SettingUI(Q_NULLPTR, screenRect);
	settingUI->setAdminConfig(&adminConfig);
	settingUI->setDetectConfig(&detectConfig);
	settingUI->setDetectParams(&detectParams);
	settingUI->doConnect();//信号连接
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnMainUI_settingUI()), this, SLOT(do_enableButtonsOnMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemState_settingUI()), this, SLOT(do_checkSystemState_settingUI()));
	
	//模板提取界面
	templateUI = new TemplateUI(Q_NULLPTR, screenRect);
	templateUI->setAdminConfig(&adminConfig);
	templateUI->setDetectConfig(&detectConfig);
	templateUI->setDetectParams(&detectParams);
	templateUI->setMotionControler(motionControler);
	templateUI->setCameraControler(cameraControler);
	templateUI->doConnect();//信号连接
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//检测界面
	detectUI = new DetectUI(Q_NULLPTR, screenRect);
	detectUI->setAdminConfig(&adminConfig);
	detectUI->setDetectConfig(&detectConfig);
	detectUI->setDetectParams(&detectParams);
	detectUI->setMotionControler(motionControler);
	detectUI->setCameraControler(cameraControler);
	detectUI->doConnect();//信号连接
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));

	//显示启动界面
	launcher->showFullScreen(); 
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
void PCBDetect::on_initGraphicsView_launchUI(int launchCode)
{
	if (launchCode == -1) { //系统参数和用户参数已经正常初始化
		settingUI->refreshSettingUI();//更新参数设置界面的信息
	}
	else if (launchCode == 0) { //正常启动 (运行参数也正常初始化)
		templateUI->initGraphicsView();//模板界面中图像显示的初始化
		detectUI->initGraphicsView();//检测界面中图像显示的初始化
	}
	else { //存在错误
		//用户参数配置文件丢失，生成了默认文件
		if (detectConfig.getErrorCode() != DetectConfig::Uncheck) {
			settingUI->refreshSettingUI();//更新参数设置界面的信息
		}
	}
}

//启动结束
void PCBDetect::on_launchFinished_launchUI(int launchCode)
{
	if (!launchCode) { //正常启动
		this->setPushButtonsEnabled(true);//模板提取、检测按键设为可点击
	}
	else { //存在错误
		this->setPushButtonsEnabled(false);//模板提取、检测按键设为不可点击
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
	this->showSettingUI();
}

void PCBDetect::on_pushButton_set2_clicked()
{
	this->showSettingUI();
}

//模板提取
void PCBDetect::on_pushButton_getTempl_clicked()
{
	//设置按键
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//运动结构复位
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); //复位
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);//错误提示
	}

	//更新相机参数
	templateUI->refreshCameraControler();
	//显示模板提取界面
	this->showTemplateUI();
}

void PCBDetect::on_pushButton_getTempl2_clicked()
{
	//设置按键
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//运动结构复位
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); //复位
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
	}
	//根据模板提取的参数更新相机控制器
	templateUI->refreshCameraControler();
	//显示模板提取界面
	this->showTemplateUI();
}

//检测
void PCBDetect::on_pushButton_detect_clicked()
{
	//设置按键
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//复位
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); 
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
	}

	//根据检测的参数更新相机控制器
	detectUI->refreshCameraControler();
	//显示检测界面
	this->showDetectUI();
}

void PCBDetect::on_pushButton_detect2_clicked()
{
	//设置按键
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	//运动结构复位
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start(); //复位
	while (motionControler->isRunning()) pcb::delay(100);
	if (!motionControler->isReady()) {
		motionControler->showMessageBox(this);
	}
	//根据检测的参数更新相机控制器
	detectUI->refreshCameraControler();
	//显示检测界面
	this->showDetectUI();
}

//退出
void PCBDetect::on_pushButton_exit_clicked()
{
	this->eixtDetectSystem();
}

void PCBDetect::on_pushButton_exit2_clicked()
{
	this->eixtDetectSystem();
}

//设置按键是否可点击
void PCBDetect::setPushButtonsEnabled(bool enable, bool all)
{
	//模板提取
	ui.pushButton_getTempl->setEnabled(enable);
	ui.pushButton_getTempl2->setEnabled(enable);
	//检测
	ui.pushButton_detect->setEnabled(enable);
	ui.pushButton_detect2->setEnabled(enable);

	if (all) {
		//设置
		ui.pushButton_set->setEnabled(enable);
		ui.pushButton_set2->setEnabled(enable);
		//退出
		ui.pushButton_exit->setEnabled(enable);
		ui.pushButton_exit2->setEnabled(enable);
	}
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
	//重新初始化运动控制模块
	if (code & 0b000100000 == 0b000100000) {
		motionControler->initControler();
		while (motionControler->isRunning()) pcb::delay(50);
		motionControler->showMessageBox(settingUI);
	}

	//重新初始化相机
	if (code & 0b000010000 == 0b000010000) {
		cameraControler->start();
		while (cameraControler->isRunning()) pcb::delay(50);
		cameraControler->showMessageBox(settingUI);
	}

	//重置模板提取界面，并初始化其中的图像显示的空间
	if (code & 0b000000010 == 0b000000010) {
		templateUI->resetTemplateUI();
		templateUI->initGraphicsView();
	}

	//重置检测界面，并初始化其中的图像显示的空间
	if (code & 0b000000001 == 0b000000001) {
		detectUI->resetDetectUI();
		detectUI->initGraphicsView();
	}

	//将主界面的模板提取按键、检测按键设为可点击
	if (cameraControler->isReady() && motionControler->isReady()) {
		//若各个模块的状态正常，则设置按键
		this->setPushButtonsEnabled(true);
		pcb::delay(10);
		//向设置界面发送重置操作结束的信号
		emit resetDetectSystemFinished_mainUI();
	}
}

//将模板提取按键、检测按键设为可点击
void PCBDetect::do_enableButtonsOnMainUI_settingUI()
{
	this->setPushButtonsEnabled(true);
}

//检查系统的工作状态
void PCBDetect::do_checkSystemState_settingUI()
{
	bool enableButtonsOnMainUI = true;

	//检查系统参数
	if (enableButtonsOnMainUI && !adminConfig.isValid(true)) {
		adminConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//检查用户参数
	if (enableButtonsOnMainUI && !detectConfig.isValid(&adminConfig)) {
		detectConfig.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//检查运行参数
	if (enableButtonsOnMainUI && 
		!detectParams.isValid(DetectParams::Index_All_SysInit, true, &adminConfig)) 
	{
		detectParams.showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//检测运动结构
	if (enableButtonsOnMainUI && !motionControler->isReady()) {
		motionControler->showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//检查相机
	if (enableButtonsOnMainUI && !cameraControler->isReady()) {
		cameraControler->showMessageBox(settingUI);
		enableButtonsOnMainUI = false;
	}

	//判断是否要启用主界面上的模板提取和检测按键
	if (enableButtonsOnMainUI) { //系统正常
		this->setPushButtonsEnabled(true);
	}
}

/***************** 模板提取界面 *****************/

void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
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
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
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

//测试后门 - 切换主界面按键的可点击状态
void PCBDetect::keyPressEvent(QKeyEvent *event)
{
	bool buttonsEnabled = false;
	switch (event->key())
	{
	case Qt::Key_Shift:
		if (ui.pushButton_detect->isEnabled())
			setPushButtonsEnabled(false);
		else 
			setPushButtonsEnabled(true);
	}
}