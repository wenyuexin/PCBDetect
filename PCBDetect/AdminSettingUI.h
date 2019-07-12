#pragma once

#include <QWidget>
#include "ui_AdminSettingUI.h"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include <QDesktopWidget>
#include <QIntValidator>

//系统参数设置界面
class AdminSettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::AdminSettingUI ui;
	QRegExpValidator *NumberValidator;

	pcb::AdminConfig *adminConfig; //系统参数
	pcb::AdminConfig tempConfig; //临时的系统参数类，暂存界面上的值
	const QString configFileName= ".admin.config";//配置文件的文件名

	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	pcb::RuntimeParams tempParams;//临时的运行参数
	int sysResetCode = 0b000000000; //系统重置代码

public:
	AdminSettingUI(QWidget *parent = Q_NULLPTR);
	~AdminSettingUI();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void refreshAdminSettingUI(); //更新界面上的参数
	void setPushButtonsEnabled(bool code); //按键设置

private:
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
