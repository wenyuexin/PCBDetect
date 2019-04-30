#pragma once

#include <QWidget>
#include "ui_LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "RuntimeLib.h"
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
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	MotionControler *motionControler; //运动控制器
	CameraControler *cameraControler; //相机控制器

public:
	LaunchUI(QWidget *parent = Q_NULLPTR, QRect &screenRect = QRect());
	~LaunchUI();

	inline void setAdminConfig(pcb::AdminConfig* ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig* ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams* ptr) { runtimeParams = ptr; }
	inline void setMotionControler(MotionControler* ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler* ptr) { cameraControler = ptr; }
	void runInitThread(); //运行

Q_SIGNALS:
	void initGraphicsView_launchUI(int LaunchCode);
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString);
	void on_adminConfigError_initThread();
	void on_userConfigError_initThread();
	void on_runtimeParamsError_initThread();
	void on_initGraphicsView_initThread(int);

	void on_motionError_initThread(int code = MotionControler::Default);
	void on_cameraError_initThread();
	void on_sysInitFinished_initThread();
};
