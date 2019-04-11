#pragma once

#include <QWidget>
#include "ui_SettingUI.h"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "PassWordUI.h" 
#include "AdminSettingUI.h"
#include <QDesktopWidget>
#include <QRegExpValidator>


//参数设置界面
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	pcb::DetectParams *detectParams;//运行参数
	pcb::DetectParams tempParams;//临时的运行参数

	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectConfig tempConfig; //临时的用户参数类
	const QString configFileName = ".user.config";//配置文件的文件名

	pcb::AdminConfig *adminConfig; //系统参数
	PassWordUI passWordUI; //系统设置登录界面
	AdminSettingUI adminSettingUI; //系统设置界面
	int sysResetCode = 0b00000000;

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void doConnect(); //信号连接
	void refreshSettingUI(); //更新设置界面
	void setPushButtonsToEnabled(bool code); //按键设置

private:
	void initSettingUI();
	void getConfigFromSettingUI();
	void setCursorLocation(pcb::DetectConfig::ConfigIndex code);

Q_SIGNALS:
	void showDetectMainUI();
	void resetDetectSystem_settingUI(int);
	void enableButtonsOnMainUI_settingUI();
	void checkSystemState_settingUI();

private Q_SLOTS:
	void on_pushButton_SampleDirPath_clicked();
	void on_pushButton_TemplDirPath_clicked();
	void on_pushButton_OutputDirPath_clicked();
	void on_currentIndexChanged_imgFormat();

	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();
	void on_pushButton_admin_clicked();

	void on_closePassWordUI_pswdUI();
	void do_showAdminSettingUI_pswdUI();
	void do_showSettingUI_adminUI();
	void do_resetDetectSystem_adminUI(int);
	void do_checkSystemWorkingState_adminUI();
};
