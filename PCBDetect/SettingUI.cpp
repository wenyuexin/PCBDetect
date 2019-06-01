#include "SettingUI.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::Configurator;


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

	//将匹配精度的确认框设为一组
	QPushButton button;
	matchingCheckBoxGroup.addButton(&button);
	matchingCheckBoxGroup.addButton(ui.checkBox_matchingAccuracy_high, 1);
	matchingCheckBoxGroup.addButton(ui.checkBox_matchingAccuracy_low, 2);

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
	//参数设置
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

	if (userConfig->matchingAccuracyLevel == 1) //匹配精度等级
		ui.checkBox_matchingAccuracy_high->setChecked(true);
	else if (userConfig->matchingAccuracyLevel == 2) {
		ui.checkBox_matchingAccuracy_low->setChecked(true);
	}

	ui.lineEdit_concaveRateThresh->setText(QString::number(userConfig->concaveRateThresh)); //缺失率阈值
	ui.lineEdit_convexRateThresh->setText(QString::number(userConfig->convexRateThresh)); //凸起率阈值

	//运动控制
	ui.pushButton_reset_motion->setEnabled(true);
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
	case pcb::UserConfig::Index_clusterComPort:
		ui.comboBox_clusterComPort->setFocus(); break;

	case pcb::UserConfig::Index_ActualProductSize_W:
		ui.lineEdit_ActualProductSize_W->setFocus(); break;
	case pcb::UserConfig::Index_ActualProductSize_H:
		ui.lineEdit_ActualProductSize_H->setFocus(); break;
	case pcb::UserConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case pcb::UserConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;

	case pcb::UserConfig::Index_matchingAccuracyLevel:
		ui.checkBox_matchingAccuracy_high->setFocus(); break;
	case pcb::UserConfig::Index_concaveRateThresh:
		ui.lineEdit_concaveRateThresh->setFocus(); break;
	case pcb::UserConfig::Index_convexRateThresh:
		ui.lineEdit_convexRateThresh->setFocus(); break;
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

	//重置控制器
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
		this->setPushButtonsEnabled(true);//将按键设为可点击
		UserConfig::ConfigIndex index = UserConfig::convertCodeToIndex(code);
		this->setCursorLocation(index);//将光标定位到无效参数的输入框上
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
			runtimeParams->copyTo(&tempParams);

			RuntimeParams::ErrorCode code;
			//计算单步运动距离
			code = tempParams.calcSingleMotionStroke(adminConfig);
			if (code != RuntimeParams::ValidValue) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			//计算相机个数和拍照次数
			code = tempParams.calcItemGridSize(adminConfig, userConfig);
			if (code != RuntimeParams::ValidValue) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			//计算初始拍照位置
			code = tempParams.calcInitialPhotoPos(adminConfig);
			if (code != RuntimeParams::ValidValue) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}

			if (!tempParams.isValid(RuntimeParams::Index_All_SysInit, true, adminConfig)) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			sysResetCode |= runtimeParams->getSystemResetCode(tempParams);
			tempParams.copyTo(runtimeParams);
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

	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //样本路径
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //模板路径
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//输出路径

	tempConfig.ActualProductSize_W = ui.lineEdit_ActualProductSize_W->text().toInt();
	tempConfig.ActualProductSize_H = ui.lineEdit_ActualProductSize_H->text().toInt();
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//每一行中的基本单元数
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//每一列中的基本单元数

	int accuracyLevel = 0;
	if (ui.checkBox_matchingAccuracy_high->isChecked()) accuracyLevel = 1;
	if (ui.checkBox_matchingAccuracy_low->isChecked()) accuracyLevel = 2;
	tempConfig.matchingAccuracyLevel = accuracyLevel; //匹配模式

	tempConfig.concaveRateThresh = ui.lineEdit_concaveRateThresh->text().toInt();//缺失率阈值
	tempConfig.convexRateThresh = ui.lineEdit_convexRateThresh->text().toInt();//凸起率阈值
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
