#pragma once

#include <QThread>
#include "Configurator.h"
#include "CameraControler.h"
#include <windows.h>


namespace pcb {
	class SysInitThread;
}

//��ʼ���߳�
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	pcb::DetectConfig *detectConfig; //�û���������
	pcb::AdminConfig *adminConfig; //ϵͳ��������
	CameraControler *cameraControler; //���������
	int bootStatus; //����״̬

public:
	SysInitThread();
	~SysInitThread();

	inline void setDetectConfig(pcb::DetectConfig *ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setAdminConfig(pcb::AdminConfig *ptr = Q_NULLPTR) { adminConfig = ptr; }
	inline void setCameraControler(CameraControler *ptr = Q_NULLPTR) { cameraControler = ptr; }

protected:
	void run();

private:
	bool initDetectConfig();
	bool initCameraControler();

Q_SIGNALS:
	void sysInitStatus_initThread(QString status);
	void configError_initThread();
	void cameraError_initThread();
	void sysInitFinished_initThread();
};

