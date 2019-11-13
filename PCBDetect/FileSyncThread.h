#pragma once
#ifndef WIN32

#define WIN32

#endif
#include <QThread>
#include <QDebug>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "FileSender.h"


//文件同步线程
//将检测设备上的检测结果同步到复查设备中
class FileSyncThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数

public:
	FileSyncThread(QObject *parent = Q_NULLPTR);
	~FileSyncThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

protected:
	void run();
};
