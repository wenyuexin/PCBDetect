#pragma once

#include <QWidget>
#include "ui_LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include <QDesktopWidget>
#include <QPixmap>
#include <QDir>
#include <QTime>


//启动界面
class LaunchUI : public QWidget
{
	Q_OBJECT

private:
	Ui::LaunchUI ui;
	SysInitThread *initThread; //系统初始化线程
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数
	MotionControler *motionControler; //运动控制器
	CameraControler *cameraControler; //相机控制器

public:
	LaunchUI(QWidget *parent = Q_NULLPTR);
	~LaunchUI();

	inline void setAdminConfig(pcb::AdminConfig* ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig* ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams* ptr) { detectParams = ptr; }
	inline void setMotionControler(MotionControler* ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler* ptr) { cameraControler = ptr; }
	void runInitThread(); //运行

Q_SIGNALS:
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString status);
	void on_adminConfigError_initThread();
	void on_detectConfigError_initThread();
	void on_detectParamsError_initThread();
	void on_motionError_initThread(int code = MotionControler::Default);
	void on_cameraError_initThread();
	void on_sysInitFinished_initThread();
};
