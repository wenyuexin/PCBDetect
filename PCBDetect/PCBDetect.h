#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PCBDetect.h"
#include "LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "DetectUI.h"
#include "SettingUI.h"
#include "TemplateUI.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QTime>


#ifndef STRUCT_DETECT_SYSTEM_UI
#define STRUCT_DETECT_SYSTEM_UI 
	//���ϵͳ����Ҫģ��
	struct DetectSystemModules {
		LaunchUI *launcher; //��������
		SettingUI *settingUI; //�������ý���
		TemplateUI *templateUI; //ģ����ȡ����
		DetectUI *detectUI; //������
		MotionControler *motionControler;//�˶�������
		CameraControler *cameraControler;//���������
	};
#endif //STRUCT_DETECT_SYSTEM_UI


//���ϵͳ��������
class PCBDetect : public QMainWindow
{
	Q_OBJECT

private:
	Ui::PCBDetectClass ui;
	LaunchUI *launcher; //��������
	SettingUI *settingUI; //�������ý���
	TemplateUI *templateUI; //ģ����ȡ����
	DetectUI *detectUI; //������
	MotionControler *motionControler;//�˶�������
	CameraControler *cameraControler;//���������
	pcb::AdminConfig adminConfig; //ϵͳ����
	pcb::DetectConfig detectConfig; //�û�����
	pcb::DetectParams detectParams; //��ʱ����

public:
	PCBDetect(QWidget *parent = Q_NULLPTR);
	~PCBDetect();

private:
	void showSettingUI();
	void showDetectUI();
	void showTemplateUI();
	void eixtDetectSystem();

Q_SIGNALS:
	void resetDetectSystemFinished_mainUI();

private Q_SLOTS:
	void keyPressEvent(QKeyEvent *event);
	void setPushButtonsToEnabled(bool code);

	void on_pushButton_set_clicked();
	void on_pushButton_set2_clicked();
	void on_pushButton_getTempl_clicked();
	void on_pushButton_getTempl2_clicked();
	void on_pushButton_detect_clicked();
	void on_pushButton_detect2_clicked();
	void on_pushButton_exit_clicked();
	void on_pushButton_exit2_clicked();

	void on_initGraphicsView_launchUI(int launchCode);
	void on_launchFinished_launchUI(int launchCode);
	void do_showDetectMainUI_settingUI();
	void do_resetDetectSystem_settingUI(int);
	void do_enableButtonsOnMainUI_settingUI();
	void do_checkSystemState_settingUI();
	void do_showDetectMainUI_detectUI();
	void do_showDetectMainUI_templateUI();
};
