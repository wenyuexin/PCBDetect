#pragma once

#include "Configurator.h"
#include "RuntimeLibrary.h"
#include <QObject>
#include <QThread>
#include <QMutex> 
//#include "AMC98C.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


//运动控制器
class MotionControler : public QObject
{
	Q_OBJECT

public:
	//运动结构的错误代码
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x400,
		InitFailed = 0x401,
		MoveForwardFailed = 0x402,
		ReturnToZeroFailed = 0x403,
		ResetControlerFailed = 0x404,
		Default
	};

	const int MaxRuntime = 5000; //单位ms

private:
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //用户参数
	int callerOfResetControler; //复位的调用函数的标识
	bool running; //操作是否正在运行
	ErrorCode errorCode; //控制器的错误码
	QMutex mutex; //线程锁

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } 
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void initControler(); //初始化
	void moveForward(); //前进
	void returnToZero(); //归零
	void resetControler(int caller); //复位
	bool isRunning(); //判断当前是否有正在运行的操作

	inline bool isReady() { return errorCode == NoError; }
	inline ErrorCode getErrorCode() { return errorCode; } //获取当前的错误代码
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

private:
	void on_initControler_finished();
	void on_moveForward_finished();
	void on_returnToZero_finished();
	void on_resetControler_finished();

Q_SIGNALS:
	void initControlerFinished_motion();
	void moveForwardFinished_motion();
	void returnToZeroFinished_motion();
	void resetControlerFinished_motion(int);
};

