#include "AdminSettingUI.h"


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//一些初始化操作
	this->initSettingUI();
}

AdminSettingUI::~AdminSettingUI()
{
}

void AdminSettingUI::initSettingUI()
{
	//第一次切换按键状态在显示上可能会出现延迟，故提前预热
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//限制参数的输入范围
	QIntValidator intValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(&intValidator);
	ui.lineEdit_MaxCameraNum->setValidator(&intValidator);
	ui.lineEdit_MaxPhotographingNum->setValidator(&intValidator);
}

//更新界面
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_MaxPhotographingNum->setText(QString::number(adminConfig->MaxPhotographingNum));
}


/********************** 按键响应 **********************/

//确认
void AdminSettingUI::on_pushButton_confirm_clicked()
{
	//获取界面上的config参数
	getConfigFromAdminSettingUI();

	//检查界面上config的有效性
	AdminConfig::ErrorCode code = tempConfig.checkValidity(AdminConfig::Index_All);
	if (code != AdminConfig::ValidConfig) { //参数无效则报错
		tempConfig.showMessageBox(this);
		this->setPushButtonsToEnabled(true);//将按键设为可点击
		AdminConfig::ConfigIndex index = AdminConfig::convertCodeToIndex(code);
		this->setCursorLocation(index);//将光标定位到无效参数的输入框上
		return;
	}

	//设置聚焦位置
	this->setCursorLocation(AdminConfig::Index_None);

	//判断是否重置检测系统
	int resetCode = detectConfig->getSystemResetCode(tempConfig);

	//将临时配置拷贝到config中
	tempConfig.copyTo(adminConfig);

	//重置系统
	emit resetDetectSystem_settingUI(resetCode); //判断是否重置检测系统

	//将参数保存到config文件中
	pcb::Configurator::saveConfigFile(configFileName, adminConfig);

	//向主界面发送消息
	emit checkSystemWorkingState_settingUI(); //检查系统的工作状态

	//将本界面上的按键设为可点击
	this->setPushButtonsToEnabled(true);

	/*******/
	bool k1, k2, k3;
	double MaxS =( ui.lineEdit_MaxMotionStroke->text()).toDouble(&k1);
	int CamerN = (ui.lineEdit_MaxCameraNum->text()).toInt(&k2);
	int PhotoN =( ui.lineEdit_MaxPhotographingNum->text()).toInt(&k3);

	if(MaxS <= 0 || CamerN <= 0 || PhotoN <= 0)
		QMessageBox::information(this, QString::fromLocal8Bit("警告"), 
			QString::fromLocal8Bit("数据必须为正数，请重新输入！"),
			QString::fromLocal8Bit("确定"));
	else {
		adminConfig->MaxCameraNum = CamerN;
		adminConfig->MaxMotionStroke = MaxS;
		adminConfig->MaxPhotographingNum = PhotoN;

		//Configurator::saveConfigFile_A(fileName, config_A);
	}
}

//返回
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminSettingUI();
}

//设置按键的可点击状态
void AdminSettingUI::setPushButtonsToEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);//确认
	ui.pushButton_return->setEnabled(code);//返回
}


/**************** 获取界面上的参数 ****************/

void AdminSettingUI::getConfigFromAdminSettingUI()
{
	tempConfig.MaxMotionStroke = ui.lineEdit_MaxMotionStroke->text().toInt();
	tempConfig.MaxCameraNum = ui.lineEdit_MaxCameraNum->text().toInt();
}