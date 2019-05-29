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


//�������ý���
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	QRect screenRect; //��Ļ��ʾ����
	QRegExpValidator *NumberValidator;
	QButtonGroup matchingCheckBoxGroup;

	pcb::AdminConfig *adminConfig; //ϵͳ����
	PassWordUI passWordUI; //ϵͳ���õ�¼����
	AdminSettingUI *adminSettingUI; //ϵͳ���ý���

	pcb::UserConfig *userConfig; //�û�����
	pcb::UserConfig tempConfig; //��ʱ���û�������
	const QString configFileName = ".user.config";//�����ļ����ļ���

	pcb::RuntimeParams *runtimeParams;//���в���
	pcb::RuntimeParams tempParams;//��ʱ�����в���

	int sysResetCode; //ϵͳ���ô���

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void refreshSettingUI(); //�������ý���
	void setPushButtonsEnabled(bool code); //��������

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
