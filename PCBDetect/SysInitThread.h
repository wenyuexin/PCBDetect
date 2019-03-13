#pragma once

#include <QThread>
#include "Configurator.h"
#include <windows.h>

using Ui::DetectConfig;

//初始化线程
class SysInitThread :
	public QThread
{
	Q_OBJECT

private:
	Ui::DetectConfig *config;

public:
	SysInitThread();
	~SysInitThread();

	inline void setDetectConfig(Ui::DetectConfig* ptr = Q_NULLPTR) { config = ptr; }

protected:
	void run();

private:
	int initDetectConfig();

Q_SIGNALS:
	void initializeStatus_initThread(QString status);
	void configError_initThread(int code);
	void initializeFinished_initThread();
};

