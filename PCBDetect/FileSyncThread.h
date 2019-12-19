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


//�ļ�ͬ���߳�
//������豸�ϵļ����ͬ���������豸��
class FileSyncThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���

public:
	FileSyncThread(QObject *parent = Q_NULLPTR);
	~FileSyncThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

protected:
	void run();
};
