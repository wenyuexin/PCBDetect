#pragma once

#include "ui_PassWordUI.h"
#include <QWidget>
#include <QMessageBox>
#include <qDebug>


//系统设置登录界面
class PassWordUI : public QWidget
{
	Q_OBJECT

public:
	PassWordUI(QWidget *parent = Q_NULLPTR);
	~PassWordUI();

private:
	Ui::PassWordUI ui;
	const QString PASS_WORD = "hongdian123456";

Q_SIGNALS:
	void showAdminSettingUI_pswdUI();
	void closePassWordUI_pswdUI();

private Q_SLOTS:
	void on_pushButton_confirm_clicked();
	void on_pushButton_cancel_clicked();
	void closeEvent(QCloseEvent * event);
};
