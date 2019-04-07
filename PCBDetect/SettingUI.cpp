#include "SettingUI.h"

using pcb::DetectConfig;
using pcb::Configurator;

SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//设置界面初始化
	this->setFocusPolicy(Qt::ClickFocus);
	initSettingUI();

	//参数下拉框的槽函数连接
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imgFormat()));

	//系统参数设置的登录界面
	passWordUI.setWindowFlags(passWordUI.windowFlags() | Qt::Dialog);
	passWordUI.setWindowModality(Qt::ApplicationModal);
	connect(&passWordUI, SIGNAL(showAdminSettingUI_pswdUI()), this, SLOT(do_showAdminSettingUI_pswdUI()));
	connect(&passWordUI, SIGNAL(closePassWordUI_pswdUI()), this, SLOT(on_closePassWordUI_pswdUI()));

	//系统参数设置界面
	adminSettingUI.setAdminConfig(adminConfig);
	connect(&adminSettingUI, SIGNAL(showSettingUI_adminUI()), this, SLOT(do_showSettingUI_adminUI()));
	connect(&adminSettingUI, SIGNAL(resetDetectSystem_adminUI()), this, SLOT(do_resetDetectSystem_adminUI()));
	connect(&adminSettingUI, SIGNAL(checkSystemWorkingState_adminUI()), this, SLOT(do_checkSystemWorkingState_adminUI()));
}

SettingUI::~SettingUI()
{
}


/************* 界面的设置、输入、更新 **************/

void SettingUI::initSettingUI()
{
	//第一次切换按键状态在显示上可能会出现延迟，故提前预热
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//限制参数的输入范围
	QIntValidator intValidator;
	ui.lineEdit_ActualProductSize_W->setValidator(&intValidator);
	ui.lineEdit_ActualProductSize_H->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInRow->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInCol->setValidator(&intValidator);
}

//更新界面
void SettingUI::refreshSettingUI()
{
	ui.lineEdit_SampleDirPath->setText(detectConfig->SampleDirPath);
	ui.lineEdit_TemplDirPath->setText(detectConfig->TemplDirPath);
	ui.lineEdit_OutputDirPath->setText(detectConfig->OutputDirPath);

	QString format = detectConfig->ImageFormat.toLower();
	if (format == ".bmp") ui.comboBox_ImageFormat->setCurrentText("    *.bmp");
	else if (format == ".jpg") ui.comboBox_ImageFormat->setCurrentText("    *.jpg");
	else if (format == ".png") ui.comboBox_ImageFormat->setCurrentText("    *.png");
	else if (format == ".tif" || format == ".tiff") ui.comboBox_ImageFormat->setCurrentText("    *.tif");

	ui.lineEdit_ActualProductSize_W->setText(QString::number(detectConfig->ActualProductSize_W));
	ui.lineEdit_ActualProductSize_H->setText(QString::number(detectConfig->ActualProductSize_H));
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(detectConfig->nBasicUnitInRow));
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(detectConfig->nBasicUnitInCol));
}

//设置光标的位置
void SettingUI::setCursorLocation(DetectConfig::ConfigIndex code)
{
	int textLen;
	switch (code)
	{
	case pcb::DetectConfig::Index_All:
		break;
	case pcb::DetectConfig::Index_None:
		ui.lineEdit_SampleDirPath->setFocus(); 
		ui.lineEdit_SampleDirPath->clearFocus();
		break;
	case pcb::DetectConfig::Index_SampleDirPath:
		textLen = ui.lineEdit_SampleDirPath->text().length();
		ui.lineEdit_SampleDirPath->setCursorPosition(textLen);
		ui.lineEdit_SampleDirPath->setFocus(); break;
	case pcb::DetectConfig::Index_TemplDirPath:
		ui.lineEdit_TemplDirPath->setFocus(); break;
	case pcb::DetectConfig::Index_OutputDirPath:
		ui.lineEdit_OutputDirPath->setFocus(); break;
	case pcb::DetectConfig::Index_ImageFormat:
		break;
	case pcb::DetectConfig::Index_ActualProductSize_W:
		ui.lineEdit_ActualProductSize_W->setFocus(); break;
	case pcb::DetectConfig::Index_ActualProductSize_H:
		ui.lineEdit_ActualProductSize_H->setFocus(); break;
	case pcb::DetectConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case pcb::DetectConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;
	}
}


/***************** 按键响应 *****************/

//选择样本文件夹的路径
void SettingUI::on_pushButton_SampleDirPath_clicked()
{
	detectConfig->SampleDirPath = pcb::selectDirPath();
	ui.lineEdit_SampleDirPath->setText(detectConfig->SampleDirPath);
}

//选择模板文件夹的路径
void SettingUI::on_pushButton_TemplDirPath_clicked()
{
	detectConfig->TemplDirPath = pcb::selectDirPath();
	ui.lineEdit_TemplDirPath->setText(detectConfig->TemplDirPath);
}

//选择输出文件夹的路径
void SettingUI::on_pushButton_OutputDirPath_clicked()
{
	detectConfig->OutputDirPath = pcb::selectDirPath();
	ui.lineEdit_OutputDirPath->setText(detectConfig->OutputDirPath);
}

//确认键
void SettingUI::on_pushButton_confirm_clicked()
{
	//将参数设置界面的确认按键、返回按键设为不可点击
	this->setPushButtonsToEnabled(false);

	//获取界面上的config参数
	getConfigFromSettingUI();

	//检查界面上config的有效性
	DetectConfig::ErrorCode code = tempConfig.checkValidity(DetectConfig::Index_All);
	if (code != DetectConfig::ValidConfig) { //参数无效则报错
		tempConfig.showMessageBox(this); //弹窗警告
		this->setPushButtonsToEnabled(true);//将按键设为可点击
		DetectConfig::ConfigIndex index = DetectConfig::convertCodeToIndex(code);
		this->setCursorLocation(index);//将光标定位到无效参数的输入框上
		return;
	}

	//设置聚焦位置
	this->setCursorLocation(DetectConfig::Index_None);

	//判断是否重置检测系统
	int resetCode = detectConfig->getSystemResetCode(tempConfig); 

	//将临时配置拷贝到detectConfig中
	tempConfig.copyTo(detectConfig);

	//重置系统
	resetCode |= detectParams->updateGridSize(adminConfig, tempConfig);
	emit resetDetectSystem_settingUI(resetCode); //判断是否重置检测系统

	//将参数保存到配置文件中
	Configurator::saveConfigFile(configFileName, detectConfig);

	//向主界面发送消息
	emit checkSystemWorkingState_settingUI(); //检查系统的工作状态
	pcb::delay(100);
	
	//将本界面上的按键设为可点击
	this->setPushButtonsToEnabled(true);
}

//返回键
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}

//系统参数设置
void SettingUI::on_pushButton_admin_clicked()
{
	//设置窗口始终置顶
	passWordUI.show();
	//设置按键
	this->setPushButtonsToEnabled(false);
}

//设置按键的可点击状态
void SettingUI::setPushButtonsToEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);//确认
	ui.pushButton_return->setEnabled(code);//返回
	ui.pushButton_admin->setEnabled(code);//系统设置
}

/************** 获取界面上的参数 *****************/

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
}


/****************** 系统参数登录界面 *******************/

void SettingUI::on_closePassWordUI_pswdUI()
{
	//将系统参数按键设为可点击
	this->setPushButtonsToEnabled(true);//设置按键
}

//显示系统参数设置界面
void SettingUI::do_showAdminSettingUI_pswdUI()
{
	adminSettingUI.setAdminConfig(adminConfig);
	adminSettingUI.refreshAdminSettingUI();
	pcb::delay(10);
	adminSettingUI.showFullScreen();
	pcb::delay(10);
	this->hide(); //隐藏参数设置界面
	this->setPushButtonsToEnabled(true);//设置按键
}


/****************** 系统参数设置界面 *******************/

//由系统参数设置界面返回参数设置界面
void SettingUI::do_showSettingUI_adminSettingUI()
{
	this->showFullScreen();
	pcb::delay(10);
	adminSettingUI.hide();
}

//转发由系统设置界面发出的重置信号
void SettingUI::do_resetDetectSystem_adminUI(int code)
{
	emit resetDetectSystem_settingUI(code);
}

//转发由系统设置界面发出的系统状态检查信号
void SettingUI::do_checkSystemWorkingState_adminUI()
{
	emit checkSystemWorkingState_settingUI(code);
}
