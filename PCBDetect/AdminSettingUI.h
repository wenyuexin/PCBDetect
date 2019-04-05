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
	pcb::AdminConfig *adminConfig;
	const QString configFileName= ".admin.config";

public:
	AdminSettingUI(QWidget *parent = Q_NULLPTR);
	~AdminSettingUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	void refreshAdminSettingUI(); //更新界面上的参数

private:
	void initSettingUI();

Q_SIGNALS:
	void showSettingUI_adminSettingUI();

private Q_SLOTS:
	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();

};
