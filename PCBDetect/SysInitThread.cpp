#include "SysInitThread.h"

using Ui::DetectConfig;


SysInitThread::SysInitThread()
{
	bootStatus = 0x00; //启动状态
}

SysInitThread::~SysInitThread()
{
}


/***************** 启动线程 *****************/

void SysInitThread::run()
{
	if (bootStatus == 0x00) {  //初次执行初始化
		if (!initDetectConfig()) { bootStatus |= 0x10;  return; }
		if (!initCameraControler()) { bootStatus |= 0x01;  return; }
	}
	else if ((bootStatus & 0x10) >> 4 == 0x1) { //DetectConfig初始化异常
		if (!initDetectConfig()) { bootStatus |= 0x10; return; }
		else                     { bootStatus &= 0x01; }
	}
	else if ((bootStatus & 0x01) == 0x1) { //相机初始化异常
		if (!initCameraControler()) { bootStatus |= 0x01; return; }
		else                        { bootStatus &= 0x10; }
	}
	
	//初始化结束
	emit sysInitFinished_initThread();
}


/****************** 初始化 ********************/

//对config进行初始化
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("正在获取历史参数配置 ..."));
	Ui::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", detectConfig)) {
		emit configError_initThread(DetectConfig::ConfigFileMissing); return false;
	}
	else {
		//计算宽高比
		DetectConfig::ErrorCode code = detectConfig->calcImageAspectRatio();
		if (code != DetectConfig::ValidValue) {
			emit configError_initThread(code); return false; 
		}
		//参数有效性判断
		code = detectConfig->checkValidity(DetectConfig::Index_All);
		if (code != DetectConfig::ValidConfig) { 
			emit configError_initThread(code); return false; 
		}
	}

	emit sysInitStatus_initThread(QString::fromLocal8Bit("历史参数配置获取结束   "));
	Ui::delay(500);
	return true;
}

//初始化相机控制器
bool SysInitThread::initCameraControler()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("正在初始化相机 ..."));
	Ui::delay(1000);
	cameraControler->setOperation(CameraControler::InitCameras);
	cameraControler->start(); //启动线程
	cameraControler->wait(); //线程等待
	if (cameraControler->getErrorCode() != CameraControler::NoError) { 
		emit cameraError_initThread(); return false; 
	}
	Ui::delay(600);
	emit sysInitStatus_initThread(QString::fromLocal8Bit("相机初始化结束    "));
	return true;
}
