#pragma once

#include <QThread>
#include "Configurator.h"
#include "RuntimeLib.h"
#include "MotionControler.h"
#include "CameraControler.h"
//#include <windows.h>


//��ʼ���߳�
class SysInitThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ��������
	pcb::UserConfig *userConfig; //�û���������
	pcb::RuntimeParams *runtimeParams; //�û���������
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������
	int bootStatus; //����״̬

public:
	SysInitThread();
	~SysInitThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setMotionControler(MotionControler *ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler *ptr) { cameraControler = ptr; }

protected:
	void run();

private:
	bool initAdminConfig();
	bool initUserConfig();
	bool initRuntimeParams();
	bool initMotionControler();
	bool initCameraControler();

Q_SIGNALS:
	void sysInitStatus_initThread(QString);
	void adminConfigError_initThread();
	void userConfigError_initThread();
	void runtimeParamsError_initThread();
	void initGraphicsView_initThread(int);
	void motionError_initThread(int);
	void cameraError_initThread();
	void sysInitFinished_initThread();
};

