#include "SettingUI.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::Configurator;
using std::vector;


SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//成员变量初始化
	runtimeParams = Q_NULLPTR;//运行参数
	userConfig = Q_NULLPTR; //用户参数
	adminConfig = Q_NULLPTR; //系统参数
	adminSettingUI = Q_NULLPTR; //系统设置界面
	sysResetCode = 0b00000000; //系统重置代码
	NumberValidator = Q_NULLPTR;
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
	NumberValidator2 = new QRegExpValidator(QRegExp("[/.0-9]+$"));
}

void SettingUI::init()
{
	//多屏状态下选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//设置聚焦策略
	this->setFocusPolicy(Qt::ClickFocus);

	//设置光标
	this->setCursorLocation(UserConfig::Index_None);

	//第一次切换按键状态在显示上可能会出现延迟，故提前预热
	this->setPushButtonsEnabled(false);
	this->setPushButtonsEnabled(true);

	//限制参数的输入范围
	ui.lineEdit_ActualProductSize_W->setValidator(NumberValidator);
	ui.lineEdit_ActualProductSize_H->setValidator(NumberValidator);
	ui.lineEdit_nBasicUnitInRow->setValidator(NumberValidator);
	ui.lineEdit_nBasicUnitInCol->setValidator(NumberValidator);
	ui.lineEdit_concaveRateThresh->setValidator(NumberValidator);
	ui.lineEdit_convexRateThresh->setValidator(NumberValidator);
	ui.lineEdit_exposureTime_camera->setValidator(NumberValidator);
	ui.lineEdit_inetAddressOfRecheckPC->setValidator(NumberValidator2);

	//相机曝光时间的确认框设为一组
	CheckBoxGroup_colorMode.addButton(ui.lineEdit_colorMode_rgb_camera, 0);
	CheckBoxGroup_colorMode.addButton(ui.lineEdit_colorMode_gray_camera, 1);

	//将匹配精度的确认框设为一组
	QPushButton button;
	checkBoxGroup_matchingAccuracy.addButton(&button);
	checkBoxGroup_matchingAccuracy.addButton(ui.checkBox_matchingAccuracy_high, 1);
	checkBoxGroup_matchingAccuracy.addButton(ui.checkBox_matchingAccuracy_low, 2);

	//参数下拉框的槽函数连接
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imgFormat()));
	connect(ui.comboBox_clusterComPort, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_comPort()));

	//系统参数设置的登录界面
	passWordUI.setWindowFlags(passWordUI.windowFlags() | Qt::Dialog);
	passWordUI.setWindowModality(Qt::ApplicationModal);
	connect(&passWordUI, SIGNAL(showAdminSettingUI_pswdUI()), this, SLOT(do_showAdminSettingUI_pswdUI()));
	connect(&passWordUI, SIGNAL(closePassWordUI_pswdUI()), this, SLOT(on_closePassWordUI_pswdUI()));

	//系统参数设置界面
	adminSettingUI = new AdminSettingUI();
	adminSettingUI->setAdminConfig(adminConfig);
	adminSettingUI->setUserConfig(userConfig);
	adminSettingUI->setRuntimeParams(runtimeParams);
	adminSettingUI->init();
	connect(adminSettingUI, SIGNAL(showSettingUI_adminUI()), this, SLOT(do_showSettingUI_adminUI()));
	connect(adminSettingUI, SIGNAL(resetDetectSystem_adminUI()), this, SLOT(do_resetDetectSystem_adminUI()));
	connect(adminSettingUI, SIGNAL(checkSystemState_adminUI()), this, SLOT(do_checkSystemState_adminUI()));
}

SettingUI::~SettingUI()
{
	qDebug() << "~SettingUI";
	delete adminSettingUI; 
	adminSettingUI = Q_NULLPTR;
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
}


/************* 界面的设置、输入、更新 **************/

//更新界面
void SettingUI::refresh()
{
	//基本设置
	ui.lineEdit_SampleDirPath->setText(userConfig->SampleDirPath); //样本路径
	ui.lineEdit_TemplDirPath->setText(userConfig->TemplDirPath); //模板路径
	ui.lineEdit_OutputDirPath->setText(userConfig->OutputDirPath); //输出路径

	QString comPort = userConfig->clusterComPort.toUpper(); //运动控制串口号
	QString headBlankChars = "    "; //开头的空字符串
	ui.comboBox_clusterComPort->setCurrentText(headBlankChars + " " + comPort);

	QString imgFormat = userConfig->ImageFormat.toLower(); //图像格式
	if (imgFormat == ".tiff") imgFormat = ".tif";
	ui.comboBox_ImageFormat->setCurrentText(headBlankChars + "*" + imgFormat);

	ui.lineEdit_ActualProductSize_W->setText(QString::number(userConfig->ActualProductSize_W));//产品尺寸
	ui.lineEdit_ActualProductSize_H->setText(QString::number(userConfig->ActualProductSize_H));//产品尺寸
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(userConfig->nBasicUnitInRow));//基本单元数
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(userConfig->nBasicUnitInCol));//基本单元数

	//运动控制
	ui.pushButton_reset_motion->setEnabled(motionControler->isReady());

	//相机组
	ui.lineEdit_exposureTime_camera->setText(QString::number(userConfig->exposureTime));//曝光时间

	if (userConfig->colorMode == 0) //色彩模式
		ui.lineEdit_colorMode_rgb_camera->setChecked(true); //RGB彩色
	else if (userConfig->matchingAccuracyLevel == 1) {
		ui.lineEdit_colorMode_gray_camera->setChecked(true); //黑白
	}

	//检测算法
	vector<bool> vec = userConfig->defectTypeToBeProcessed; //待检测的缺陷类型
	ui.checkBox_defectType_short->setChecked(true);
	ui.checkBox_defectType_short->setDisabled(true);
	ui.checkBox_defectType_break->setChecked(true);
	ui.checkBox_defectType_break->setDisabled(true);
	ui.checkBox_defectType_convex->setChecked(vec[2]);
	ui.checkBox_defectType_concave->setChecked(vec[3]);

	if (userConfig->matchingAccuracyLevel == 1) //匹配精度等级
		ui.checkBox_matchingAccuracy_high->setChecked(true);
	else if (userConfig->matchingAccuracyLevel == 2) {
		ui.checkBox_matchingAccuracy_low->setChecked(true);
	}

	ui.lineEdit_concaveRateThresh->setText(QString::number(userConfig->concaveRateThresh)); //缺失率阈值
	ui.lineEdit_convexRateThresh->setText(QString::number(userConfig->convexRateThresh)); //凸起率阈值

	//文件传输
	ui.lineEdit_inetAddressOfRecheckPC->setText(userConfig->inetAddressOfRecheckPC); //复查设备的IP
}

//设置光标的位置
void SettingUI::setCursorLocation(UserConfig::ConfigIndex code)
{
	int textLen;
	switch (code)
	{
	case pcb::UserConfig::Index_All:
	case pcb::UserConfig::Index_None:
		ui.lineEdit_SampleDirPath->setFocus(); 
		ui.lineEdit_SampleDirPath->clearFocus(); break;

	//基本设置
	case pcb::UserConfig::Index_SampleDirPath:
		textLen = ui.lineEdit_SampleDirPath->text().length();
		ui.lineEdit_SampleDirPath->setCursorPosition(textLen);
		ui.lineEdit_SampleDirPath->setFocus(); break;
	case pcb::UserConfig::Index_TemplDirPath:
		ui.lineEdit_TemplDirPath->setFocus(); break;
	case pcb::UserConfig::Index_OutputDirPath:
		ui.lineEdit_OutputDirPath->setFocus(); break;
	case pcb::UserConfig::Index_ImageFormat:
		ui.comboBox_ImageFormat->setFocus(); break;
	
	case pcb::UserConfig::Index_ActualProductSize_W:
		ui.lineEdit_ActualProductSize_W->setFocus(); break;
	case pcb::UserConfig::Index_ActualProductSize_H:
		ui.lineEdit_ActualProductSize_H->setFocus(); break;
	case pcb::UserConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case pcb::UserConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;

	//运动结构
	case pcb::UserConfig::Index_clusterComPort:
		ui.comboBox_clusterComPort->setFocus(); break;

	//相机组
	case pcb::UserConfig::Index_exposureTime:
		ui.lineEdit_exposureTime_camera->setFocus(); break;
	case pcb::UserConfig::Index_colorMode:
		ui.lineEdit_colorMode_rgb_camera->setFocus(); break;

	//检测算法
	case pcb::UserConfig::Index_defectTypeToBeProcessed:
		ui.checkBox_defectType_short->setFocus(); break;
	case pcb::UserConfig::Index_matchingAccuracyLevel:
		ui.checkBox_matchingAccuracy_high->setFocus(); break;
	case pcb::UserConfig::Index_concaveRateThresh:
		ui.lineEdit_concaveRateThresh->setFocus(); break;
	case pcb::UserConfig::Index_convexRateThresh:
		ui.lineEdit_convexRateThresh->setFocus(); break;

	//文件传输
	case pcb::UserConfig::Index_inetAddressOfRecheckPC:
		ui.lineEdit_inetAddressOfRecheckPC->setFocus(); break;
	}
}

//设置按键的可点击状态
void SettingUI::setPushButtonsEnabled(bool code)
{
	ui.pushButton_SampleDirPath->setEnabled(code);
	ui.pushButton_TemplDirPath->setEnabled(code);
	ui.pushButton_OutputDirPath->setEnabled(code);

	ui.pushButton_initAndReturnToZero_motion->setEnabled(code);
	ui.pushButton_reset_motion->setEnabled(code && motionControler->isReady());

	ui.pushButton_confirm->setEnabled(code);//确认
	ui.pushButton_return->setEnabled(code);//返回
	ui.pushButton_admin->setEnabled(code);//系统设置
}


/***************** 基本参数设置 *****************/

//选择样本文件夹的路径
void SettingUI::on_pushButton_SampleDirPath_clicked()
{
	userConfig->SampleDirPath = pcb::selectDirPath(this);
	ui.lineEdit_SampleDirPath->setText(userConfig->SampleDirPath);
}

//选择模板文件夹的路径
void SettingUI::on_pushButton_TemplDirPath_clicked()
{
	userConfig->TemplDirPath = pcb::selectDirPath(this);
	ui.lineEdit_TemplDirPath->setText(userConfig->TemplDirPath);
}

//选择输出文件夹的路径
void SettingUI::on_pushButton_OutputDirPath_clicked()
{
	userConfig->OutputDirPath = pcb::selectDirPath(this);
	ui.lineEdit_OutputDirPath->setText(userConfig->OutputDirPath);
}


/***************** 运动控制 *****************/

//重置运动结构
void SettingUI::on_pushButton_initAndReturnToZero_motion_clicked()
{
	//禁用界面上的按键和输入框
	this->setPushButtonsEnabled(false);

	//重置运动控制器
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start();
	while (motionControler->isRunning()) pcb::delay(50);

	//启用界面上的按键和输入框
	this->setPushButtonsEnabled(true);
}

//运动结构复位
void SettingUI::on_pushButton_reset_motion_clicked()
{
	//禁用界面上的按键和输入框
	this->setPushButtonsEnabled(false);

	//运动复位
	motionControler->setOperation(MotionControler::MotionReset);
	motionControler->start();
	while (motionControler->isRunning()) pcb::delay(50);

	//启用界面上的按键和输入框
	this->setPushButtonsEnabled(true);
}


/***************** 确认键、返回键 *****************/

//确认键
void SettingUI::on_pushButton_confirm_clicked()
{
	//将参数设置界面的确认按键、返回按键设为不可点击
	this->setPushButtonsEnabled(false);

	//获取界面上的config参数
	getConfigFromSettingUI();

	//检查界面上config的有效性
	UserConfig::ErrorCode code;
	code = tempConfig.checkValidity(UserConfig::Index_All, adminConfig);
	if (code != UserConfig::ValidConfig) { //参数无效则报错
		tempConfig.showMessageBox(this); //弹窗警告
		this->setPushButtonsEnabled(true); //将按键设为可点击
		UserConfig::ConfigIndex index = UserConfig::convertCodeToIndex(code);
		this->setCursorLocation(index); //将光标定位到无效参数的输入框上
		return;
	}

	//设置聚焦位置
	this->setCursorLocation(UserConfig::Index_None);

	//判断参数是否已经修改
	sysResetCode = 0b00000000; //系统重置代码
	if (userConfig->unequals(tempConfig) != UserConfig::Index_None) {
		//判断是否重置检测系统
		sysResetCode |= userConfig->getSystemResetCode(tempConfig);
		//将临时配置拷贝到userConfig中
		tempConfig.copyTo(userConfig);
		//将参数保存到配置文件中
		Configurator::saveConfigFile(configFileName, userConfig);

		//更新运行参数
		if (adminConfig->isValid(false)) {
			runtimeParams->copyTo(&tempParams); //复制到临时对象中
			if (!tempParams.update(adminConfig, userConfig)) { //更新临时对象中的参数
				tempParams.showMessageBox(this);
				this->setPushButtonsEnabled(true); return;
			}
			sysResetCode |= runtimeParams->getSystemResetCode(tempParams); //获取重置代码
			tempParams.copyTo(runtimeParams); //将临时对象复制到运行参数中
		}

		//判断是否重置检测系统
		if (sysResetCode != 0 && adminConfig->isValid(true) && userConfig->isValid(adminConfig)
			&& runtimeParams->isValid(RuntimeParams::Index_All_SysInit, true, adminConfig))
		{
			QMessageBox::warning(this, pcb::chinese("提示"),
				pcb::chinese("您已修改关键参数，相关模块将重新启动！ \n"),
				pcb::chinese("确定"));
			pcb::delay(10);
			emit resetDetectSystem_settingUI(sysResetCode);
		}
	}
	else if (userConfig->getErrorCode() != tempConfig.getErrorCode()) {
		//将临时配置拷贝到用户参数对象中
		tempConfig.copyTo(userConfig);
		//将参数保存到配置文件中
		Configurator::saveConfigFile(configFileName, userConfig);
	}
		
	//向主界面发送消息
	emit checkSystemState_settingUI(); //检查系统的工作状态
	pcb::delay(10);
	
	//将本界面上的按键设为可点击
	this->setPushButtonsEnabled(true);
}

//返回键
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
	//如果界面上的参数无效，而userConfig有效，则显示userConfig
	if (!tempConfig.isValid(adminConfig) && userConfig->isValid(adminConfig)) {
		this->refresh();
	}
}


/************** 获取界面上的参数 *****************/

//串口号
void SettingUI::on_currentIndexChanged_comPort()
{
	switch (ui.comboBox_clusterComPort->currentIndex())
	{
	case 0:
		tempConfig.clusterComPort = ""; break;
	case 1:
		tempConfig.clusterComPort = "COM1"; break;
	case 2:
		tempConfig.clusterComPort = "COM2"; break;
	case 3:
		tempConfig.clusterComPort = "COM3"; break;
	case 4:
		tempConfig.clusterComPort = "COM4"; break;
	case 5:
		tempConfig.clusterComPort = "COM5"; break;
	case 6:
		tempConfig.clusterComPort = "COM6"; break;
	case 7:
		tempConfig.clusterComPort = "COM7"; break;
	case 8:
		tempConfig.clusterComPort = "COM8"; break;
	case 9:
		tempConfig.clusterComPort = "COM9"; break;
	default:
		break;
	}
}

//图像格式
void SettingUI::on_currentIndexChanged_imgFormat()
{
	switch ((pcb::ImageFormat) ui.comboBox_ImageFormat->currentIndex())
	{
	case pcb::ImageFormat::Unknown:
		tempConfig.ImageFormat = ""; break;
	case pcb::ImageFormat::BMP:
		tempConfig.ImageFormat = ".bmp"; break;
	case pcb::ImageFormat::JPG:
		tempConfig.ImageFormat = ".jpg"; break;
	case pcb::ImageFormat::PNG:
		tempConfig.ImageFormat = ".png"; break;
	case pcb::ImageFormat::TIF:
		tempConfig.ImageFormat = ".tif"; break;
	default:
		break;
	}
}

//从设置界面上获取参数
void SettingUI::getConfigFromSettingUI()
{
	tempConfig.resetErrorCode(); //重置错误代码
	
	//基本设置
	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //样本路径
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //模板路径
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//输出路径

	tempConfig.ActualProductSize_W = ui.lineEdit_ActualProductSize_W->text().toInt();
	tempConfig.ActualProductSize_H = ui.lineEdit_ActualProductSize_H->text().toInt();
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//每一行中的基本单元数
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//每一列中的基本单元数

	QString defectTypeToBeProcessed = "";
	if (ui.checkBox_defectType_short->isChecked()) defectTypeToBeProcessed;

	//相机组
	tempConfig.exposureTime = ui.lineEdit_ActualProductSize_W->text().toInt();//曝光时间
	if (ui.lineEdit_colorMode_rgb_camera->isChecked()) tempConfig.colorMode = 0;//色彩模式
	if (ui.lineEdit_colorMode_gray_camera->isChecked()) tempConfig.colorMode = 1;

	//检测算法
	tempConfig.defectTypeToBeProcessed[0] = ui.checkBox_defectType_short->isChecked();//待检测的缺陷类型
	tempConfig.defectTypeToBeProcessed[1] = ui.checkBox_defectType_break->isChecked();
	tempConfig.defectTypeToBeProcessed[2] = ui.checkBox_defectType_convex->isChecked();
	tempConfig.defectTypeToBeProcessed[3] = ui.checkBox_defectType_concave->isChecked();
	
	//检测算法
	int accuracyLevel = 0; //匹配模式
	if (ui.checkBox_matchingAccuracy_high->isChecked()) accuracyLevel = 1;
	if (ui.checkBox_matchingAccuracy_low->isChecked()) accuracyLevel = 2;
	tempConfig.matchingAccuracyLevel = accuracyLevel; 

	tempConfig.concaveRateThresh = ui.lineEdit_concaveRateThresh->text().toInt();//缺失率阈值
	tempConfig.convexRateThresh = ui.lineEdit_convexRateThresh->text().toInt();//凸起率阈值

	//文件传输
	tempConfig.inetAddressOfRecheckPC = ui.lineEdit_inetAddressOfRecheckPC->text(); //复查设备IP
}


/****************** 系统参数登录界面 *******************/

//打开系统参数登陆界面
void SettingUI::on_pushButton_admin_clicked()
{
	//设置窗口始终置顶
	passWordUI.show();
	//设置按键
	this->setPushButtonsEnabled(false);
}

//关闭系统参数登陆界面
void SettingUI::on_closePassWordUI_pswdUI()
{
	//将系统参数按键设为可点击
	this->setPushButtonsEnabled(true);//设置按键
}

//显示系统参数设置界面
void SettingUI::do_showAdminSettingUI_pswdUI()
{
	adminSettingUI->setAdminConfig(adminConfig);
	adminSettingUI->refreshAdminSettingUI();
	pcb::delay(10);
	adminSettingUI->showFullScreen();
	pcb::delay(10);
	this->hide(); //隐藏参数设置界面
	this->setPushButtonsEnabled(true);//设置按键
}


/****************** 系统参数设置界面 *******************/

//由系统参数设置界面返回参数设置界面
void SettingUI::do_showSettingUI_adminUI()
{
	this->setCursorLocation(UserConfig::Index_None);
	this->showFullScreen();
	pcb::delay(10);
	adminSettingUI->hide();
}

//转发由系统设置界面发出的重置信号
void SettingUI::do_resetDetectSystem_adminUI(int code)
{
	emit resetDetectSystem_settingUI(code);
}

//转发由系统设置界面发出的系统状态检查信号
void SettingUI::do_checkSystemWorkingState_adminUI()
{
	if (adminConfig->isValid(true) && userConfig->isValid(adminConfig)
		&& runtimeParams->isValid(RuntimeParams::Index_All_SysInit, true, adminConfig))
	{
		emit checkSystemState_settingUI();
		pcb::delay(10);
	}
	else {
	}
}
