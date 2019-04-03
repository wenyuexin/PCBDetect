#pragma once

#include <QThread>
#include "Configurator.h"
#include "CameraControler.h"
#include <windows.h>

using Ui::DetectConfig;

//��ʼ���߳�
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	Ui::DetectConfig *detectConfig; //�û���������
	Ui::AdminConfig *adminConfig; //ϵͳ��������
	CameraControler *cameraControler; //���������
	int bootStatus; //����״̬

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

