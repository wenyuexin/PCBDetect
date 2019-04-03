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

namespace Ui {
	class LaunchUI;
}

//启动界面
class LaunchUI : public QWidget
{
	Q_OBJECT

private:
	Ui::LaunchUI ui;
	Ui::DetectConfig *detectConfig; //用户参数配置
	Ui::AdminConfig *adminConfig; //系统参数配置
	CameraControler *cameraControler; //相机控制器
	SysInitThread *initThread; //系统初始化线程

public:
	LaunchUI(QWidget *parent = Q_NULLPTR, QPixmap *background = Q_NULLPTR);
	~LaunchUI();

	inline void setDetectConfig(Ui::DetectConfig* ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setAdminConfig(Ui::AdminConfig* ptr = Q_NULLPTR) { adminConfig = ptr; }
	inline void setCameraControler(CameraControler* ptr = Q_NULLPTR) { cameraControler = ptr; }
	void runInitThread(); //运行

Q_SIGNALS:
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString status);
	void on_configError_initThread(int ErrorCode);
	void on_cameraError_initThread();
	void on_sysInitFinished_initThread();
};
