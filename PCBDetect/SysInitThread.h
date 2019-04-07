#pragma once

#include <QThread>
#include "Configurator.h"
#include "CameraControler.h"
#include <windows.h>


namespace pcb {
	class SysInitThread;
}

//初始化线程
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	pcb::DetectConfig *detectConfig; //用户参数配置
	pcb::AdminConfig *adminConfig; //系统参数配置
	CameraControler *cameraControler; //相机控制器
	int bootStatus; //启动状态

public:
	SysInitThread();
	~SysInitThread();

	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setCameraControler(CameraControler *ptr) { cameraControler = ptr; }

protected:
	void run();

private:
	bool initDetectConfig();
	bool initAdminConfig();
	bool initCameraControler();

Q_SIGNALS:
	void sysInitStatus_initThread(QString status);
	void detectConfigError_initThread();
	void adminConfigError_initThread();
	void cameraError_initThread();
	void sysInitFinished_initThread();
};

