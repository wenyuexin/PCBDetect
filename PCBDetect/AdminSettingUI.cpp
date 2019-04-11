#include "AdminSettingUI.h"

using pcb::Configurator;
using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//变量初始化
	adminConfig = Q_NULLPTR; //系统参数
	detectConfig = Q_NULLPTR; //用户参数
	detectParams = Q_NULLPTR; //运行参数

	//一些初始化操作
	this->initAdminSettingUI();
}

AdminSettingUI::~AdminSettingUI()
{
}

void AdminSettingUI::initAdminSettingUI()
{
	//设置光标
	this->setCursorLocation(AdminConfig::Index_None);

	//第一次切换按键状态在显示上可能会出现延迟，故提前预热
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//限制参数的输入范围
	QIntValidator intValidator;
	QDoubleValidator doubleValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(&intValidator);
	ui.lineEdit_MaxCameraNum->setValidator(&intValidator);
	ui.lineEdit_PixelsNumPerUnitLength->setValidator(&intValidator);
	ui.lineEdit_ImageOverlappingRate->setValidator(&doubleValidator);
	ui.lineEdit_ImageSize_W->setValidator(&intValidator);
	ui.lineEdit_ImageSize_H->setValidator(&intValidator);
}

//更新界面
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_PixelsNumPerUnitLength->setText(QString::number(adminConfig->PixelsNumPerUnitLength));
	ui.lineEdit_ImageOverlappingRate->setText(QString::number(adminConfig->ImageOverlappingRate));
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
		this->setPushButtonsToEnabled(true);//将按键设为可点击
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
		sysResetCode |= detectParams->calcItemGridSize(adminConfig, detectConfig);
		if (!detectParams->isValid()) detectParams->showMessageBox(this);
		//发送检测系统重置信号
		emit resetDetectSystem_adminUI(sysResetCode);
	}
	if (adminConfig->getErrorCode() != tempConfig.getErrorCode()) {
		//将临时配置拷贝到系统参数对象中
		tempConfig.copyTo(adminConfig);
	}
	
	//将本界面上的按键设为可点击
	this->setPushButtonsToEnabled(true);
}

//返回
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminUI();
	//如果界面上的系统参数无效，而adminConfig有效，则显示adminConfig
	//注：初始条件下，没有按确定键，则tempConfig为空，tempConfig无效
	if (!tempConfig.isValid() && adminConfig->isValid()) {
		this->refreshAdminSettingUI();
	}
}

//设置按键的可点击状态
void AdminSettingUI::setPushButtonsToEnabled(bool code)
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
	tempConfig.MaxCameraNum = ui.lineEdit_MaxCameraNum->text().toInt();
	tempConfig.PixelsNumPerUnitLength = ui.lineEdit_PixelsNumPerUnitLength->text().toInt();
	tempConfig.ImageOverlappingRate = ui.lineEdit_ImageOverlappingRate->text().toDouble();
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
	case pcb::DetectConfig::Index_All:
	case pcb::DetectConfig::Index_None:
		ui.lineEdit_MaxMotionStroke->setFocus();
		ui.lineEdit_MaxMotionStroke->clearFocus(); break;
	case pcb::AdminConfig::Index_MaxMotionStroke:
		ui.lineEdit_MaxMotionStroke->setFocus(); break;
	case pcb::AdminConfig::Index_MaxCameraNum:
		ui.lineEdit_MaxCameraNum->setFocus(); break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength:
		ui.lineEdit_PixelsNumPerUnitLength->setFocus(); break;
	case pcb::AdminConfig::Index_ImageOverlappingRate:
		ui.lineEdit_ImageOverlappingRate->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_W:
		ui.lineEdit_ImageSize_W->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_H:
		ui.lineEdit_ImageSize_H->setFocus(); break;
	}
}