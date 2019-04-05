#pragma once

#include <QWidget>
#include "ui_SettingUI.h"
#include "Configurator.h"
#include "PassWordUI.h" 
#include "AdminSettingUI.h"
#include <QDesktopWidget>
#include <QRegExpValidator>

namespace Ui {
	class SettingUI;
}

//参数设置界面
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectConfig tempConfig; //临时的用户参数类
	const QString configFileName = ".user.config";

	PassWordUI passWordUI;
	AdminSettingUI adminSettingUI;
	pcb::AdminConfig *adminConfig; //系统参数

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();

	inline void setDetectConfig(pcb::DetectConfig *ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setAdminConfig(pcb::AdminConfig *ptr = Q_NULLPTR) { adminConfig = ptr; }

	void refreshSettingUI();
	void setPushButtonsToEnabled(bool code);

private:
	void initSettingUI();
	void selectDirPath(QString &path);
	void getConfigFromSettingUI();
	void setCursorLocation(pcb::DetectConfig::ConfigIndex code);

Q_SIGNALS:
	void showDetectMainUI();
	void resetDetectSystem_settingUI(int);
	void enableButtonsOnDetectMainUI_settingUI();
	void checkSystemWorkingState_settingUI();

private Q_SLOTS:
	void on_pushButton_SampleDirPath_clicked();
	void on_pushButton_TemplDirPath_clicked();
	void on_pushButton_OutputDirPath_clicked();
	void on_currentIndexChanged_imageFormat();

	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();
	void on_pushButton_admin_clicked();

	void on_closePassWordUI_pswdUI();
	void do_showAdminSettingUI_pswdUI();
	void do_showSettingUI_adminSettingUI();
};
