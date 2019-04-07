#include "SysInitThread.h"

using pcb::DetectConfig;
using pcb::AdminConfig;
using pcb::Configurator;


SysInitThread::SysInitThread()
{
	detectConfig = Q_NULLPTR; //用户参数配置
	adminConfig = Q_NULLPTR; //系统参数配置
	cameraControler = Q_NULLPTR; //相机控制器
	bootStatus = 0x0000; //启动状态
}

SysInitThread::~SysInitThread()
{
}


/***************** 启动线程 *****************/

void SysInitThread::run()
{
	if (bootStatus == 0x0000) {  //初次执行初始化
		if (!initDetectConfig()) { bootStatus |= 0x1000;  return; }
		if (!initAdminConfig()) { bootStatus |= 0x0100;  return; }
		if (!initCameraControler()) { bootStatus |= 0x0001;  return; }
	}
	else if (!((bootStatus & 0xF000) >> 12)) { //detectConfig初始化异常
		if (!initDetectConfig()) { bootStatus |= 0x1000; return; }
		else                     { bootStatus &= 0x0FFF; }
	}
	else if (!((bootStatus & 0x0F00) >> 8)) { //adminConfig初始化异常
		if (!initAdminConfig()) { bootStatus |= 0x0100; return; }
		else                     { bootStatus &= 0xF0FF; }
	}
	else if (!(bootStatus & 0x000F) { //相机初始化异常
		if (!initCameraControler()) { bootStatus |= 0x0001; return; }
		else                        { bootStatus &= 0xFFF0; }
	}
	
	//初始化结束
	emit sysInitFinished_initThread();
}


/****************** 初始化 ********************/

//对DetectConfig进行初始化
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("正在获取用户参数 ..."));
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", detectConfig)) {
		emit detectConfigError_initThread(); return false;
	}
	else {
		DetectConfig::ErrorCode code;
		//计算宽高比
		code = detectConfig->calcImageAspectRatio();
		if (code != DetectConfig::ValidValue) {
			emit detectConfigError_initThread(); return false; 
		}
		//参数有效性判断
		code = detectConfig->checkValidity(DetectConfig::Index_All);
		if (code != DetectConfig::ValidConfig) { 
			emit detectConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("用户参数获取结束   "));
	pcb::delay(500);
	return true;
}

//对adminConfig进行初始化
bool SysInitThread::initAdminConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("正在获取系统参数 ..."));
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.admin.config", adminConfig)) {
		emit adminConfigError_initThread(); return false;
	}
	else {
		//参数有效性判断
		AdminConfig::ErrorCode code;//错误代码
		code = adminConfig->checkValidity(AdminConfig::Index_All);
		if (code != AdminConfig::ValidConfig) { 
			emit adminConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("系统参数获取结束   "));
	pcb::delay(500);
	return true;
}

//初始化相机控制器
bool SysInitThread::initCameraControler()
{
	emit sysInitStatus_initThread(pcb::chinese("正在初始化相机 ..."));
	pcb::delay(1000);
	cameraControler->setOperation(CameraControler::InitCameras);
	cameraControler->start(); //启动线程
	cameraControler->wait(); //线程等待
	if (cameraControler->getErrorCode() != CameraControler::NoError) { 
		emit cameraError_initThread(); return false; 
	}
	pcb::delay(600);
	emit sysInitStatus_initThread(pcb::chinese("相机初始化结束    "));
	return true;
}
