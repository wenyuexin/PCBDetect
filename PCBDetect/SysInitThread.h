#pragma once

#include <QThread>
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include <windows.h>


//��ʼ���߳�
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ��������
	pcb::DetectConfig *detectConfig; //�û���������
	pcb::DetectParams *detectParams; //�û���������
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������
	int bootStatus; //����״̬

public:
	SysInitThread();
	~SysInitThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }
	inline void setMotionControler(MotionControler *ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler *ptr) { cameraControler = ptr; }

protected:
	void run();

private:
	bool initAdminConfig();
	bool initDetectConfig();
	bool initDetectParams();
	bool initMotionControler();
	bool initCameraControler();

Q_SIGNALS:
	void sysInitStatus_initThread(QString status);
	void adminConfigError_initThread();
	void detectConfigError_initThread();
	void detectParamsError_initThread();
	void motionError_initThread(int);
	void cameraError_initThread();
	void sysInitFinished_initThread();
};

