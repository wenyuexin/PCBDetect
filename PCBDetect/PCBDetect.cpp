#include "PCBDetect.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	launcher = Q_NULLPTR;
	settingUI = Q_NULLPTR;
	extractUI = Q_NULLPTR;
	detectUI = Q_NULLPTR;
	motionControler = Q_NULLPTR;
	cameraControler = Q_NULLPTR;

	//选择在主屏或者是副屏上显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);//副屏区域
	if (screenRect.width() < 1440 || screenRect.height() < 900) {
		screenRect = desktop->screenGeometry(0);//主屏区域
	}
	runtimeParams.ScreenRect = screenRect;
	this->setGeometry(screenRect);

	//启用开发者模式，此模式将开启测试后门以便于调试
	runtimeParams.DeveloperMode = false;

	//运动控制器
	motionControler = new MotionControler;//运动控制器
	motionControler->setAdminConfig(&adminConfig);
	motionControler->setUserConfig(&userConfig);
	motionControler->setRuntimeParams(&runtimeParams);

	//相机控制器
	cameraControler = new CameraControler;
	cameraControler->setAdminConfig(&adminConfig);
	cameraControler->setRuntimeParams(&runtimeParams);

	//启动界面
	launcher = new LaunchUI();
	launcher->setAdminConfig(&adminConfig);
	launcher->setUserConfig(&userConfig);
	launcher->setRuntimeParams(&runtimeParams);
	launcher->setMotionControler(motionControler);
	launcher->setCameraControler(cameraControler);
	launcher->init();
	launcher->runInitThread(); //运行初始化线程
	connect(launcher, SIGNAL(initGraphicsView_launchUI(int)), this, SLOT(on_initGraphicsView_launchUI(int)));
	connect(launcher, SIGNAL(launchFinished_launchUI(int)), this, SLOT(on_launchFinished_launchUI(int)));

	//参数设置界面
	settingUI = new SettingUI();
	settingUI->setAdminConfig(&adminConfig);
	settingUI->setUserConfig(&userConfig);
	settingUI->setRuntimeParams(&runtimeParams);
	settingUI->init();
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));
	connect(settingUI, SIGNAL(resetDetectSystem_settingUI(int)), this, SLOT(do_resetDetectSystem_settingUI(int)));
	connect(settingUI, SIGNAL(enableButtonsOnMainUI_settingUI()), this, SLOT(do_enableButtonsOnMainUI_settingUI()));
	connect(settingUI, SIGNAL(checkSystemState_settingUI()), this, SLOT(do_checkSystemState_settingUI()));
	
	//模板提取界面
	extractUI = new ExtractUI();
	extractUI->setAdminConfig(&adminConfig);
	extractUI->setUserConfig(&userConfig);
	extractUI->setRuntimeParams(&runtimeParams);
	extractUI->setMotionControler(motionControler);
	extractUI->setCameraControler(cameraControler);
	connect(extractUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//检测界面
	detectUI = new DetectUI();
	detectUI->setAdminConfig(&adminConfig);
	detectUI->setUserConfig(&userConfig);
	detectUI->setRuntimeParams(&runtimeParams);
	detectUI->setMotionControler(motionControler);
	detectUI->setCameraControler(cameraControler);
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));

	//显示启动界面
	launcher->showFullScreen(); 
}

PCBDetect::~PCBDetect()
{
	qDebug() << "~PCBDetect";
	delete launcher; 
	launcher = Q_NULLPTR;
	delete settingUI; 
	settingUI = Q_NULLPTR;
	delete extractUI; 
	extractUI = Q_NULLPTR;
	delete detectUI; 
	detectUI = Q_NULLPTR;
	delete motionControler; 
	motionControler = Q_NULLPTR;
	delete cameraControler; 
	cameraControler = Q_NULLPTR;
}


/**************** 启动界面 *****************/

//参数类初始化结束
void PCBDetect::on_initGraphicsView_launchUI(int launchCode)
{
	if (launchCode == -1) { //系统参数和用户参数已经正常初始化
		settingUI->refreshSettingUI();//更新参数设置界面的信息
	}
	else if (launchCode == 0) { //运行参数也正常初始化（所有参数类都正常）
		extractUI->init();//模板提取界面的实例初始化
		detectUI->init();//检测界面的实例初始化
	}
	else { //存在错误
		//用户参数配置文件丢失，生成了默认文件
		if (userConfig.getErrorCode() != UserConfig::Unchecked) {
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


//退出 - 图标
void PCBDetect::on_pushButton_exit_clicked()
{
	this->eixtDetectSystem();
}

//退出 - 按键
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

//按键响应 - 设置图标
void PCBDetect::on_pushButton_set_clicked()
{
	this->showSettingUI();
}

//按键响应 - 设置按键
void PCBDetect::on_pushButton_set2_clicked()
{
	this->showSettingUI();
}

//显示主界面，隐藏设置界面
void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //显示主界面
	pcb::delay(10); //延时
	settingUI->hide(); //隐藏设置界面
}

//显示设置界面，隐藏主界面
void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //显示设置界面
	pcb::delay(10); //延时
	this->hide(); //隐藏主界面
}

//重置检测系统
void PCBDetect::do_resetDetectSystem_settingUI(int code)
{
	bool noError = true; //系统是否正常

	//重新初始化运动控制模块
	if (noError && ((code & 0b000100000) > 0)) {
		motionControler->setOperation(MotionControler::InitControler);
		motionControler->start();
		while (motionControler->isRunning()) pcb::delay(50);
	}
	noError &= motionControler->isReady();

	//重新初始化相机
	if (noError && ((code & 0b000010000) > 0)) {
		cameraControler->setOperation(CameraControler::InitCameras);
		cameraControler->start();
		while (cameraControler->isRunning()) pcb::delay(50);
	}
	noError &= cameraControler->isReady();

	//重置模板提取界面，并初始化其中的图像显示的空间
	if (noError && ((code & 0b000000010) > 0)) {
		extractUI->resetTemplateUI();
		extractUI->initGraphicsView();
	}

	//重置检测界面，并初始化其中的图像显示的空间
	if (noError && ((code & 0b000000001) > 0)) {
		detectUI->resetDetectUI();
		detectUI->initGraphicsView();
	}

	//若各模块状态正常，则将模板提取按键、检测按键设为可点击
	if (noError) { this->setPushButtonsEnabled(true); }
}

//将模板提取按键、检测按键设为可点击
void PCBDetect::do_enableButtonsOnMainUI_settingUI()
{
	this->setPushButtonsEnabled(true);
}

//检查系统的工作状态
void PCBDetect::do_checkSystemState_settingUI()
{
	if (checkSystemState(true)) {
		//启用主界面上的模板提取和检测按键
		this->setPushButtonsEnabled(true);
	}
}

/* 检查系统状态
 * 输入：showError 若值为true，则当发现模块存在错误时将弹窗提示
 * 输出：noErrorFlag 系统所有模式是否不存在错误
 */
bool PCBDetect::checkSystemState(bool showError)
{
	bool noErrorFlag = true; //系统是否正常

	noErrorFlag = checkParametricClasses(showError);

	//检测运动结构
	if (noErrorFlag && !motionControler->isReady()) {
		MotionControler::ErrorCode code = motionControler->getErrorCode();
		if (code == MotionControler::InitFailed || code == MotionControler::Unchecked) {
			if (showError) {
				//询问是否需要重新初始化
				int choice = QMessageBox::warning(settingUI, pcb::chinese("警告"),
					pcb::chinese("运动控制器未初始化或初始化失败，是否重新执行初始化？ \n"),
					pcb::chinese("确定"), pcb::chinese("取消"));
				//判断是否重新初始化
				if (choice == 0) do_resetDetectSystem_settingUI(0b000100000);
			}
		}
		else { //和初始化无关的错误
			if (showError) motionControler->showMessageBox(settingUI);
		}
	}
	noErrorFlag &= motionControler->isReady();

	//检查相机
	if (noErrorFlag && !cameraControler->isReady()) {
		CameraControler::ErrorCode code = cameraControler->getErrorCode();
		if (code == CameraControler::InitFailed || code == CameraControler::Unchecked) {
			if (showError) {
				//询问是否需要重新初始化
				int choice = QMessageBox::warning(settingUI, pcb::chinese("警告"),
					pcb::chinese("相机控制器未初始化或初始化失败，是否重新执行初始化？ \n"),
					pcb::chinese("确定"), pcb::chinese("取消"));
				//判断是否重新初始化
				if (choice == 0) do_resetDetectSystem_settingUI(0b000010000);
			}
		}
		else { //和初始化无关的错误
			if (showError) cameraControler->showMessageBox(settingUI);
		}
	}
	noErrorFlag &= cameraControler->isReady();

	return noErrorFlag;
}

/* 检查参数类的状态
 * 输入：showError 若值为true，则当发现参数类存在错误时将弹窗提示
 * 输出：noErrorFlag 系统所有的参数类是否不存在错误
 */
bool PCBDetect::checkParametricClasses(bool showError)
{
	bool noErrorFlag = true; 

	//检查系统参数
	if (noErrorFlag && !adminConfig.isValid(true)) {
		if (showError) adminConfig.showMessageBox(settingUI);
		noErrorFlag = false;
	}

	//检查用户参数
	if (noErrorFlag && !userConfig.isValid(&adminConfig)) {
		if (showError) userConfig.showMessageBox(settingUI);
		noErrorFlag = false;
	}

	//检查运行参数
	if (noErrorFlag && !runtimeParams.isValid(RuntimeParams::Index_All_SysInit, true, &adminConfig)) {
		if (showError) runtimeParams.showMessageBox(settingUI);
		noErrorFlag = false;
	}

	return noErrorFlag;
}



/***************** 模板提取界面 *****************/

//模板提取 - 图标
void PCBDetect::on_pushButton_getTempl_clicked()
{
	this->switchToTemplateUI();
}

//模板提取 - 按键
void PCBDetect::on_pushButton_getTempl2_clicked()
{
	this->switchToTemplateUI();
}

//从主界面切换至模板提取界面
void PCBDetect::switchToTemplateUI()
{
	//设置按键
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	if (!runtimeParams.DeveloperMode) {
		//运动结构复位
		motionControler->setOperation(MotionControler::ResetControler);
		motionControler->start(); //复位
		while (motionControler->isRunning()) pcb::delay(100);
		if (!motionControler->isReady()) {
			motionControler->showMessageBox(this);//错误提示
		}
	}

	extractUI->resetTemplateUI(); //重置提取界面
	if (!runtimeParams.DeveloperMode) extractUI->refreshCameraControler();//更新相机参数
	this->showTemplateUI(); //显示模板提取界面
}

//显示模板提取界面，隐藏主界面
void PCBDetect::showTemplateUI()
{
	extractUI->showFullScreen(); //显示模板提取界面
	pcb::delay(10); //延时
	this->hide(); //隐藏主界面
}

//显示主界面，隐藏模板提取界面
void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
	this->showFullScreen(); //显示主界面
	pcb::delay(10); //延时
	extractUI->hide(); //隐藏模板提取界面
}


/******************* 检测界面 ******************/


//按键响应 - 检测图标
void PCBDetect::on_pushButton_detect_clicked()
{
	this->switchToDetectUI();
}


//按键响应 - 检测按键
void PCBDetect::on_pushButton_detect2_clicked()
{
	this->switchToDetectUI();
}

//从主界面切换至
void PCBDetect::switchToDetectUI()
{
	//设置按键
	this->setPushButtonsEnabled(false, true);
	pcb::delay(10);

	if (!runtimeParams.DeveloperMode) {
		//运动结构复位
		motionControler->setOperation(MotionControler::ResetControler);
		motionControler->start();
		while (motionControler->isRunning()) pcb::delay(100);
		if (!motionControler->isReady()) {
			motionControler->showMessageBox(this);
		}
	}

	detectUI->resetDetectUI();//重置检测界面
	if (!runtimeParams.DeveloperMode) detectUI->refreshCameraControler();//更新相机控制器
	this->showDetectUI();//显示检测界面
}

//显示检测界面，隐藏主界面
void PCBDetect::showDetectUI()
{
	detectUI->showFullScreen(); //显示检测界面
	pcb::delay(10); //延时
	this->hide(); //隐藏主界面
}

//显示主界面，隐藏检测界面
void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->setPushButtonsEnabled(true, true);
	pcb::delay(10);
	this->showFullScreen(); //显示主界面
	pcb::delay(10); //延时
	detectUI->hide(); //隐藏检测界面
}


/******************* 其他函数 ******************/

//退出检测系统
void PCBDetect::eixtDetectSystem()
{
	//询问是否需要退出系统
	int choice = QMessageBox::question(settingUI, pcb::chinese("询问"),
		pcb::chinese("确定退出PCB缺陷检测系统？ \n"),
		pcb::chinese("确定"), pcb::chinese("取消"));
	//判断是否需要退出
	if (choice == 0) { 
		//this->close();
		qApp->exit(0);
	}
}

//测试后门 - 切换主界面按键的可点击状态
void PCBDetect::keyPressEvent(QKeyEvent *event)
{
	//bool buttonsEnabled = false;
	switch (event->key())
	{
	case Qt::Key_Shift:
		switchDeveloperMode(); break;
	}
}

//在开发者模式和非开发者模式之间切换
void PCBDetect::switchDeveloperMode()
{
	//参数类存在错误
	if (!checkParametricClasses(false)) {
		runtimeParams.DeveloperMode = false; 
		this->setPushButtonsEnabled(false); return;
	}

	if (runtimeParams.DeveloperMode) {
		//模式切换的提示
		int choice = QMessageBox::information(this, pcb::chinese("提示"),
			pcb::chinese("开发者模式已关闭！ \n"),
			pcb::chinese("确定"));
		runtimeParams.DeveloperMode = false; //关闭
		//根据系统状态判断是否要禁用模板提取、检测按键
		this->setPushButtonsEnabled(checkSystemState(false));
	}
	else {
		//模式切换的询问
		int choice = QMessageBox::question(this, pcb::chinese("询问"),
			pcb::chinese("是否要启用开发者模式？ \n"),
			pcb::chinese("确定"), pcb::chinese("取消"));
		if (choice == 0) runtimeParams.DeveloperMode = true; //启用
		//将模板提取、检测按键设为可点击
		this->setPushButtonsEnabled(true); 
	}
}