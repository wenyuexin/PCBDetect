#pragma once

#include <QThread>
#include "Configurator.h"
#include "CameraControler.h"
#include <windows.h>

using Ui::DetectConfig;

//初始化线程
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	Ui::DetectConfig *detectConfig; //用户参数配置
	Ui::AdminConfig *adminConfig; //系统参数配置
	CameraControler *cameraControler; //相机控制器
	int bootStatus; //启动状态

public:
	SysInitThread();
	~SysInitThread();

	inline void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setAdminConfig(Ui::AdminConfig *ptr = Q_NULLPTR) { adminConfig = ptr; }
	inline void setCameraControler(CameraControler *ptr = Q_NULLPTR) { cameraControler = ptr; }

protected:
	void run();

private:
	bool initDetectConfig();
	bool initCameraControler();

Q_SIGNALS:
	void sysInitStatus_initThread(QString status);
	void configError_initThread(int ErrorCode);
	void cameraError_initThread();
	void sysInitFinished_initThread();
};

