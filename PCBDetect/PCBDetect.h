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
	QString IconFolder; //ͼ���ļ���
	Ui::DetectConfig config; //��������
	Ui::DetectParams params; //��ʱ����

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
