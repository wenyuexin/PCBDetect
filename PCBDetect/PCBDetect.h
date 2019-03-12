#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PCBDetect.h"
#include "LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "DetectUI.h"
#include "SettingUI.h"
#include "TemplateUI.h"
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QTime>



namespace Ui {
	class PCBRecheck;
}

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
	QString IconFolder; //图标文件夹
	Ui::DetectConfig config; //参数配置
	Ui::DetectParams params; //临时变量

public:
	PCBDetect(QWidget *parent = Q_NULLPTR);
	~PCBDetect();

private:
	void showSettingUI();
	void showDetectUI();
	void showTemplateUI();
	void eixtDetectSystem();

private Q_SLOTS:
	void on_pushButton_set_clicked();
	void on_pushButton_set2_clicked();
	void on_pushButton_getTempl_clicked();
	void on_pushButton_getTempl2_clicked();
	void on_pushButton_detect_clicked();
	void on_pushButton_detect2_clicked();
	void on_pushButton_exit_clicked();
	void on_pushButton_exit2_clicked();

	void on_launchFinished_launchUI();
	void do_showDetectMainUI_settingUI();
	void do_showDetectMainUI_detectUI();
	void do_showDetectMainUI_templateUI();
};
