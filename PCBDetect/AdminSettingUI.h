#pragma once

#include <QWidget>
#include "ui_AdminSettingUI.h"
#include "Configurator.h"
#include <QDesktopWidget>
#include <QIntValidator>

//ϵͳ�������ý���
class AdminSettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::AdminSettingUI ui;
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig tempConfig; //��ʱ��ϵͳ������
	const QString configFileName= ".admin.config";//�����ļ����ļ���

public:
	AdminSettingUI(QWidget *parent = Q_NULLPTR);
	~AdminSettingUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	void refreshAdminSettingUI(); //���½����ϵĲ���
	void setPushButtonsToEnabled(bool code); //��������

private:
	void initSettingUI();
	void getConfigFromAdminSettingUI();

Q_SIGNALS:
	void showSettingUI_adminSettingUI();
	void resetDetectSystem_settingUI(int);

private Q_SLOTS:
	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();

};
