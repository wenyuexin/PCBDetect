#pragma once

#include <QThread>
#include "Configurator.h"
#include <windows.h>

using Ui::DetectConfig;

//��ʼ���߳�
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	Ui::DetectConfig *config;
	int bootStatus; //����״̬

public:
	SysInitThread();
	~SysInitThread();

	inline void setDetectConfig(Ui::DetectConfig* ptr = Q_NULLPTR) { config = ptr; }

protected:
	void run();

private:
	bool initDetectConfig();

Q_SIGNALS:
	void sysInitStatus_initThread(QString status);
	void configError_initThread(int code);
	void sysInitFinished_initThread();
};

