#pragma once

#include <QWidget>
#include "ui_SettingUI.h"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "PassWordUI.h" 
#include "AdminSettingUI.h"
#include <QDesktopWidget>
#include <QRegExpValidator>


//�������ý���
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	pcb::DetectParams *detectParams;//���в���
	pcb::DetectParams tempParams;//��ʱ�����в���

	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectConfig tempConfig; //��ʱ���û�������
	const QString configFileName = ".user.config";//�����ļ����ļ���

	pcb::AdminConfig *adminConfig; //ϵͳ����
	PassWordUI passWordUI; //ϵͳ���õ�¼����
	AdminSettingUI adminSettingUI; //ϵͳ���ý���
	int sysResetCode = 0b00000000;

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void doConnect(); //�ź�����
	void refreshSettingUI(); //�������ý���
	void setPushButtonsToEnabled(bool code); //��������

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
