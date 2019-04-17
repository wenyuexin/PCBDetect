#include "SysInitThread.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::Configurator;


SysInitThread::SysInitThread()
{
	adminConfig = Q_NULLPTR; //系统参数配置
	detectConfig = Q_NULLPTR; //用户参数配置
	detectParams = Q_NULLPTR; //用户参数配置
	motionControler = Q_NULLPTR; //运动控制器
	cameraControler = Q_NULLPTR; //相机控制器
	bootStatus = 0x0000; //启动状态
}

SysInitThread::~SysInitThread()
{
}


/***************** 启动线程 *****************/

void SysInitThread::run()
{
	//初次执行初始化
	if (bootStatus == 0x0000) {  
		//参数类的初始化
		if (!initAdminConfig()) { bootStatus |= 0x0100; return; }
		if (!initDetectConfig()) { bootStatus |= 0x0100; return; }
		emit initGraphicsView_initThread(-1);
		qApp->processEvents();

		//更新运行参数
		if (!initDetectParams()) { bootStatus |= 0x1100; return; }
		emit initGraphicsView_initThread(0);
		qApp->processEvents();

		//初始化运动结构
		if (!initMotionControler()) { bootStatus |= 0x0010; return; }

		//初始化相机
		if (!initCameraControler()) { bootStatus |= 0x0001; return; }
	}
	
	////系统参数adminConfig初始化异常
	//if (!((bootStatus & 0xF000) >> 12)) { 
	//	if (!initAdminConfig()) { bootStatus |= 0x1000; return; }
	//	else { bootStatus &= 0x0FFF; }
	//}
	////用户参数detectConfig初始化异常
	//if (!((bootStatus & 0x0F00) >> 8)) { 
	//	if (!initDetectConfig()) { bootStatus |= 0x0100; return; }
	//	else                     { bootStatus &= 0xF0FF; }
	//}
	////运动结构初始化异常
	//if (!((bootStatus & 0x00F0) >> 4)) { 
	//	if (!initMotionControler()) { bootStatus |= 0x0010; return; }
	//	else                        { bootStatus &= 0xFF0F; }
	//}
	////相机初始化异常
	//if (!(bootStatus & 0x000F)) { 
	//	if (!initCameraControler()) { bootStatus |= 0x0001; return; }
	//	else                        { bootStatus &= 0xFFF0; }
	//}
	
	//初始化结束
	emit sysInitFinished_initThread();
}


/****************** 初始化 ********************/

//对adminConfig进行初始化
bool SysInitThread::initAdminConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("正在获取系统参数 ..."));
	qApp->processEvents();
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.admin.config", adminConfig)) {
		emit adminConfigError_initThread(); return false;
	}
	else {
		AdminConfig::ErrorCode code;//错误代码
		//计算宽高比
		code = adminConfig->calcImageAspectRatio();
		if (code != AdminConfig::ValidValue) {
			emit adminConfigError_initThread(); return false; 
		}
		//参数有效性判断
		code = adminConfig->checkValidity(AdminConfig::Index_All);
		if (code != AdminConfig::ValidConfig) { 
			emit adminConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("系统参数获取结束   "));
	pcb::delay(800);
	return true;
}

//对DetectConfig进行初始化
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("正在获取用户参数 ..."));
	qApp->processEvents();
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", detectConfig)) {
		emit detectConfigError_initThread(); return false;
	}
	else {
		DetectConfig::ErrorCode code;//错误代码
		//参数有效性判断
		code = detectConfig->checkValidity(DetectConfig::Index_All, adminConfig);
		if (code != DetectConfig::ValidConfig) { 
			emit detectConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("用户参数获取结束   "));
	pcb::delay(800);
	return true;
}

//对DetectParams进行初始化
bool SysInitThread::initDetectParams()
{
	emit sysInitStatus_initThread(pcb::chinese("正在更新运行参数 ..."));
	qApp->processEvents();
	pcb::delay(1000);
	
	DetectParams::ErrorCode code, code2;
	code = detectParams->calcSingleMotionStroke(adminConfig);
	if (code != DetectParams::ValidValue) {
		emit detectParamsError_initThread(); return false;
	}
	pcb::delay(10);

	code2 = detectParams->calcItemGridSize(adminConfig, detectConfig);
	if (code2 != DetectParams::ValidValue) {
		emit detectParamsError_initThread(); return false;
	}

	emit sysInitStatus_initThread(pcb::chinese("运行参数更新结束    "));
	pcb::delay(800);
	return true;
}

//初始化运动控制器
bool SysInitThread::initMotionControler()
{
	emit sysInitStatus_initThread(pcb::chinese("正在初始化运动结构 ..."));
	pcb::delay(800);
	
	if (!motionControler->initControler()) {
		emit motionError_initThread(MotionControler::InitFailed);
		return false; //初始化控制器
	}

	if (!motionControler->isReady()) {
		MotionControler::ErrorCode code;
		code = motionControler->getErrorCode();
		emit motionError_initThread(code);
		return false;
	}

	emit sysInitStatus_initThread(pcb::chinese("运动结构初始化结束    "));
	pcb::delay(600);
	return true;
}

//初始化相机控制器
bool SysInitThread::initCameraControler()
{
	emit sysInitStatus_initThread(pcb::chinese("正在初始化相机 ..."));
	pcb::delay(800);
	cameraControler->setOperation(CameraControler::InitCameras);
	cameraControler->start(); //启动线程
	cameraControler->wait(); //线程等待
	if (cameraControler->getErrorCode() != CameraControler::NoError) { 
		emit cameraError_initThread(); return false; 
	}
	emit sysInitStatus_initThread(pcb::chinese("相机初始化结束    "));
	pcb::delay(600);
	return true;
}
