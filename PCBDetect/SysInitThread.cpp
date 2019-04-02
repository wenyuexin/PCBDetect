#include "SysInitThread.h"

using Ui::DetectConfig;


SysInitThread::SysInitThread()
{
	bootStatus = 0; //启动状态
}

SysInitThread::~SysInitThread()
{
}


/***************** 启动线程 *****************/

void SysInitThread::run()
{
	switch (bootStatus)
	{
	case 0: //初次执行初始化
	case 1: //读取参数配置文件，对config进行初始化
		if (!initDetectConfig()) { bootStatus = 1;  return; }
	case 2: //其他初始化操作
		Ui::delay(1000);
	default:
		break;
	}
	
	//初始化结束
	emit sysInitFinished_initThread();
}


/****************** 初始化 ********************/

//对config进行初始化
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("正在获取历史参数配置 ..."));
	Ui::delay(1200);

	if (!Configurator::loadConfigFile("/.config", config)) {
		emit configError_initThread(DetectConfig::ConfigFileMissing); return false;
	}
	else {
		//计算宽高比
		DetectConfig::ErrorCode code = config->calcImageAspectRatio();
		if (code != DetectConfig::ValidConfig) { 
			emit configError_initThread(code); return false; 
		}
		//参数有效性判断
		code = config->checkValidity(DetectConfig::Index_All);
		if (code != DetectConfig::ValidConfig) { 
			emit configError_initThread(code); return false; 
		}
	}

	emit sysInitStatus_initThread(QString::fromLocal8Bit("历史参数配置获取结束  "));
	return true;
}
