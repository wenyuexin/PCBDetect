#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PCBDetect.h"
#include "LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "RuntimeLib.h"
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
	//检测系统的主要模块
	struct DetectSystemModules {
		LaunchUI *launcher; //启动界面
		SettingUI *settingUI; //参数设置界面
		TemplateUI *templateUI; //模板提取界面
		DetectUI *detectUI; //检测界面
		MotionControler *motionControler;//运动控制器
		CameraControler *cameraControler;//相机控制器
	};
#endif //STRUCT_DETECT_SYSTEM_UI


//检测系统的主界面
class PCBDetect : public QMainWindow
{
	Q_OBJECT

private:
	Ui::PCBDetectClass ui;
	LaunchUI *launcher; //启动界面
	SettingUI *settingUI; //参数设置界面
	TemplateUI *templateUI; //模板提取界面
	DetectUI *detectUI; //检测界面
	MotionControler *motionControler;//运动控制器
	CameraControler *cameraControler;//相机控制器
	pcb::AdminConfig adminConfig; //系统参数
	pcb::DetectConfig detectConfig; //用户参数
	pcb::DetectParams detectParams; //临时变量

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
	void setPushButtonsToEnabled(bool code, bool all = false);

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
