#pragma once

#include <QWidget>
#include "ui_AdminSettingUI.h"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include <QDesktopWidget>
#include <QIntValidator>

//ϵͳ�������ý���
class AdminSettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::AdminSettingUI ui;
	QRegExpValidator *NumberValidator;

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::AdminConfig tempConfig; //��ʱ��ϵͳ�����࣬�ݴ�����ϵ�ֵ
	const QString configFileName= ".admin.config";//�����ļ����ļ���

	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	pcb::RuntimeParams tempParams;//��ʱ�����в���
	int sysResetCode = 0b000000000; //ϵͳ���ô���

public:
	AdminSettingUI(QWidget *parent = Q_NULLPTR);
	~AdminSettingUI();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void refreshAdminSettingUI(); //���½����ϵĲ���
	void setPushButtonsEnabled(bool code); //��������

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
