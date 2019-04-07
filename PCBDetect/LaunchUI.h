#pragma once

#include <QWidget>
#include "ui_LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "CameraControler.h"
#include <QDesktopWidget>
#include <QPixmap>
#include <QDir>
#include <QTime>

namespace pcb {
	class LaunchUI;
}

//启动界面
class LaunchUI : public QWidget
{
	Q_OBJECT

private:
	Ui::LaunchUI ui;
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数
	CameraControler *cameraControler; //相机控制器
	SysInitThread *initThread; //系统初始化线程

public:
	LaunchUI(QWidget *parent = Q_NULLPTR);
	~LaunchUI();

	inline void setDetectConfig(pcb::DetectConfig* ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setAdminConfig(pcb::AdminConfig* ptr = Q_NULLPTR) { adminConfig = ptr; }
	inline void setCameraControler(CameraControler* ptr = Q_NULLPTR) { cameraControler = ptr; }
	void runInitThread(); //运行

Q_SIGNALS:
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString status);
	void on_detectConfigError_initThread();
	void on_adminConfigError_initThread();
	void on_cameraError_initThread();
	void on_sysInitFinished_initThread();
};
