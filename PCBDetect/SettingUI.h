#pragma once

#include <QWidget>
#include "ui_SettingUI.h"
#include "Configurator.h"
#include "RuntimeLib.h"
#include "PassWordUI.h" 
#include "AdminSettingUI.h"
#include <QDesktopWidget>
#include <QRegExpValidator>
#include <QButtonGroup>


//参数设置界面
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	QRect screenRect; //屏幕显示区域
	QRegExpValidator *NumberValidator;
	QButtonGroup matchingCheckBoxGroup;

	pcb::AdminConfig *adminConfig; //系统参数
	PassWordUI passWordUI; //系统设置登录界面
	AdminSettingUI *adminSettingUI; //系统设置界面

	pcb::UserConfig *userConfig; //用户参数
	pcb::UserConfig tempConfig; //临时的用户参数类
	const QString configFileName = ".user.config";//配置文件的文件名

	pcb::RuntimeParams *runtimeParams;//运行参数
	pcb::RuntimeParams tempParams;//临时的运行参数

	int sysResetCode; //系统重置代码

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void refreshSettingUI(); //更新设置界面
	void setPushButtonsEnabled(bool code); //按键设置

private:
	void getConfigFromSettingUI();
	void setCursorLocation(pcb::UserConfig::ConfigIndex code);

Q_SIGNALS:
	void showDetectMainUI();
	void resetDetectSystem_settingUI(int);
	void enableButtonsOnMainUI_settingUI();
	void checkSystemState_settingUI();

private Q_SLOTS:
	void on_pushButton_SampleDirPath_clicked();
	void on_pushButton_TemplDirPath_clicked();
	void on_pushButton_OutputDirPath_clicked();
	void on_currentIndexChanged_comPort();
	void on_currentIndexChanged_imgFormat();

	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();
	void on_pushButton_admin_clicked();

	void on_closePassWordUI_pswdUI();
	void do_showAdminSettingUI_pswdUI();
	void do_showSettingUI_adminUI();
	void do_resetDetectSystem_adminUI(int);
	void do_checkSystemWorkingState_adminUI();

	//void on_pushButton_toZero2_motion();
	//void on_pushButton_toZero_motion();
};
