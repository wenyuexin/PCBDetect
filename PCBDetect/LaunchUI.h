#pragma once

#include <QWidget>
#include "ui_LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include <QDesktopWidget>
#include <QPixmap>
#include <QDir>
#include <QTime>

namespace Ui {
	class LaunchUI;
}

//��������
class LaunchUI : public QWidget
{
	Q_OBJECT

private:
	Ui::LaunchUI ui;
	Ui::DetectConfig *config; //��������
	SysInitThread *initThread; //ϵͳ��ʼ���߳�

public:
	LaunchUI(QWidget *parent = Q_NULLPTR, QPixmap *background = Q_NULLPTR);
	~LaunchUI();

	inline void setDetectConfig(Ui::DetectConfig* ptr = Q_NULLPTR) { config = ptr; }
	void runInitThread(); //����

Q_SIGNALS:
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString status);
	void on_configError_initThread(int ErrorCode);
	void on_sysInitFinished_initThread();
};
