#pragma once

#include <QWidget>
#include "ui_AdminSettingUI.h"
#include "Configurator.h"
#include <QDesktopWidget>
#include <QIntValidator>

//系统参数设置界面
class AdminSettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::AdminSettingUI ui;
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig tempConfig; //临时的系统参数类
	const QString configFileName= ".admin.config";//配置文件的文件名

public:
	AdminSettingUI(QWidget *parent = Q_NULLPTR);
	~AdminSettingUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	void refreshAdminSettingUI(); //更新界面上的参数
	void setPushButtonsToEnabled(bool code); //按键设置

private:
	void initAdminSettingUI();
	void getConfigFromAdminSettingUI();
	void setCursorLocation(pcb::AdminConfig::ConfigIndex code);

Q_SIGNALS:
	void showSettingUI_adminUI();
	void resetDetectSystem_adminUI(int);
	void checkSystemWorkingState_adminUI();

private Q_SLOTS:
	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();

};
