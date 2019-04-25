#pragma once

#include <QWidget>
#include "ui_AdminSettingUI.h"
#include "Configurator.h"
#include "RuntimeLib.h"
#include <QDesktopWidget>
#include <QIntValidator>

//ϵͳ�������ý���
class AdminSettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::AdminSettingUI ui;
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::AdminConfig tempConfig; //��ʱ��ϵͳ�����࣬�ݴ�����ϵ�ֵ
	const QString configFileName= ".admin.config";//�����ļ����ļ���

	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���
	pcb::DetectParams tempParams;//��ʱ�����в���
	int sysResetCode = 0b000000000; //ϵͳ���ô���

public:
	AdminSettingUI(QWidget *parent = Q_NULLPTR, QRect &screenRect = QRect());
	~AdminSettingUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void refreshAdminSettingUI(); //���½����ϵĲ���
	void setPushButtonsEnabled(bool code); //��������

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
