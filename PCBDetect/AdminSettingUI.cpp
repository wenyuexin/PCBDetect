#include "AdminSettingUI.h"

using pcb::Configurator;
using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//变量初始化
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
}

void AdminSettingUI::init()
{
	//多屏状态下选择在主屏还是副屏上显示
	this->setGeometry(runtimeParams->ScreenRect);

	//设置光标
	this->setCursorLocation(AdminConfig::Index_None);

	//第一次切换按键状态在显示上可能会出现延迟，故提前预热
	this->setPushButtonsEnabled(false);
	this->setPushButtonsEnabled(true);

	//限制参数的输入范围
	QIntValidator intValidator;
	QDoubleValidator doubleValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(NumberValidator);
	ui.lineEdit_PulseNumInUnitTime->setValidator(NumberValidator);
	ui.lineEdit_MaxCameraNum->setValidator(NumberValidator);
	ui.lineEdit_PixelsNumPerUnitLength->setValidator(NumberValidator);
	ui.lineEdit_ImageOverlappingRate_W->setValidator(&doubleValidator);
	ui.lineEdit_ImageOverlappingRate_H->setValidator(&doubleValidator);
	ui.lineEdit_ImageSize_W->setValidator(NumberValidator);
	ui.lineEdit_ImageSize_H->setValidator(NumberValidator);
}

AdminSettingUI::~AdminSettingUI()
{
	qDebug() << "~AdminSettingUI";
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
}


/***************** 更新界面 ****************/

//更新界面
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_PulseNumInUnitTime->setText(QString::number(adminConfig->PulseNumInUnitTime));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_PixelsNumPerUnitLength->setText(QString::number(adminConfig->PixelsNumPerUnitLength, 'f', 6));
	ui.lineEdit_ImageOverlappingRate_W->setText(QString::number(adminConfig->ImageOverlappingRate_W, 'f', 6));
	ui.lineEdit_ImageOverlappingRate_H->setText(QString::number(adminConfig->ImageOverlappingRate_H, 'f', 6));
	ui.lineEdit_ImageSize_W->setText(QString::number(adminConfig->ImageSize_W));
	ui.lineEdit_ImageSize_H->setText(QString::number(adminConfig->ImageSize_H));
}


/********************** 按键响应 **********************/

//确认
void AdminSettingUI::on_pushButton_confirm_clicked()
{
	//获取界面上的config参数
	this->getConfigFromAdminSettingUI();

	//检查界面上config的有效性
	AdminConfig::ErrorCode code = tempConfig.checkValidity(AdminConfig::Index_All);
	if (code != AdminConfig::ValidConfig) { //参数无效则报错
		tempConfig.showMessageBox(this);
		this->setPushButtonsEnabled(true);//将按键设为可点击
		AdminConfig::ConfigIndex index = AdminConfig::convertCodeToIndex(code);
		this->setCursorLocation(index); //将光标定位到无效参数的输入框上
		return;
	}

	//设置聚焦位置
	this->setCursorLocation(AdminConfig::Index_None);

	//判断参数是否变更
	if (adminConfig->unequals(tempConfig) != AdminConfig::Index_None) {
		//判断是否重置检测系统
		sysResetCode |= adminConfig->getSystemResetCode(tempConfig);
		//将临时配置拷贝到config中
		tempConfig.copyTo(adminConfig);
		//将参数保存到config文件中
		Configurator::saveConfigFile(configFileName, adminConfig);
		//更新运行参数


		if (userConfig->isValid(adminConfig, false)) {
			runtimeParams->copyTo(&tempParams);

			RuntimeParams::ErrorCode code;
			//计算单步前进距离
			code = tempParams.calcSingleMotionStroke(adminConfig);
			if (code != RuntimeParams::ValidValue) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			//计算相机个数、拍照次数
			code = tempParams.calcItemGridSize(adminConfig, userConfig);
			if (code != RuntimeParams::ValidValues) {
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
			emit resetDetectSystem_adminUI(sysResetCode);
		}
	}
	else if (adminConfig->getErrorCode() != tempConfig.getErrorCode()) {
		//将临时配置拷贝到系统参数对象中
		tempConfig.copyTo(adminConfig);
	}
	
	//将本界面上的按键设为可点击
	this->setPushButtonsEnabled(true);
}

//返回
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminUI();
	//如果界面上的系统参数无效，而adminConfig有效，则显示adminConfig
	if (!tempConfig.isValid(true) && adminConfig->isValid(true)) {
		this->refreshAdminSettingUI();
	}
}

//设置按键的可点击状态
void AdminSettingUI::setPushButtonsEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);//确认
	ui.pushButton_return->setEnabled(code);//返回
}


/**************** 获取界面上的参数 ****************/

//从界面获取config参数
void AdminSettingUI::getConfigFromAdminSettingUI()
{
	tempConfig.resetErrorCode();

	tempConfig.MaxMotionStroke = ui.lineEdit_MaxMotionStroke->text().toInt();
	tempConfig.PulseNumInUnitTime = ui.lineEdit_PulseNumInUnitTime->text().toInt();
	tempConfig.MaxCameraNum = ui.lineEdit_MaxCameraNum->text().toInt();
	tempConfig.PixelsNumPerUnitLength = ui.lineEdit_PixelsNumPerUnitLength->text().toDouble();
	tempConfig.ImageOverlappingRate_W = ui.lineEdit_ImageOverlappingRate_W->text().toDouble();
	tempConfig.ImageOverlappingRate_H = ui.lineEdit_ImageOverlappingRate_H->text().toDouble();
	tempConfig.ImageSize_W = ui.lineEdit_ImageSize_W->text().toInt();
	tempConfig.ImageSize_H = ui.lineEdit_ImageSize_H->text().toInt();

	AdminConfig::ErrorCode code = tempConfig.calcImageAspectRatio();
	if (code != AdminConfig::ValidConfig) 
		adminConfig->showMessageBox(this, code); 
}

//设置界面上光标的位置
void AdminSettingUI::setCursorLocation(AdminConfig::ConfigIndex code)
{
	switch (code)
	{
	case pcb::UserConfig::Index_All:
	case pcb::UserConfig::Index_None:
		ui.lineEdit_MaxMotionStroke->setFocus();
		ui.lineEdit_MaxMotionStroke->clearFocus(); break;
	case pcb::AdminConfig::Index_MaxMotionStroke:
		ui.lineEdit_MaxMotionStroke->setFocus(); break;
	case pcb::AdminConfig::Index_PulseNumInUnitTime:
		ui.lineEdit_PulseNumInUnitTime->setFocus(); break;
	case pcb::AdminConfig::Index_MaxCameraNum:
		ui.lineEdit_MaxCameraNum->setFocus(); break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength:
		ui.lineEdit_PixelsNumPerUnitLength->setFocus(); break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_W:
		ui.lineEdit_ImageOverlappingRate_W->setFocus(); break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_H:
		ui.lineEdit_ImageOverlappingRate_H->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_W:
		ui.lineEdit_ImageSize_W->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_H:
		ui.lineEdit_ImageSize_H->setFocus(); break;
	}
}
