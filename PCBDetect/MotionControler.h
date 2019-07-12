#pragma once
#pragma comment(lib, "NETDLL.lib")
#pragma comment(lib, "MOTIONDLL.lib")

#include "AMC98C.h"
#include "MOTION_DLL.h"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include <QObject>
#include <QThread>
#include <QMutex> 

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


//运动控制器
class MotionControler : public QThread
{
	Q_OBJECT

public:
	//相机的相关操作
	enum Operation {
		NoOperation,
		InitControler, //初始化
		MoveForward, //运动前进
		ReturnToZero, //运动归零
		MoveToInitialPos, //运动至初始位置
		MotionReset, //运动复位
		ResetControler //重置控制器
	};

	//运动结构的错误代码
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x400,
		InitFailed = 0x401,
		MoveForwardFailed = 0x402,
		ReturnToZeroFailed = 0x403,
		MoveToInitialPosFailed = 0x404,
		MotionResetFailed = 0x405,
		ResetControlerFailed = 0x406,
		Default = 0x4FF
	};

private:
	int caller; //模块的调用者
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数

	int xMotionPos; //X轴的位置

	struct MotionInfo //电机信息
	{
		int xPos;//X轴当前位置
		int yPos;//X轴当前位置
		int inputStatus;//输入状态
		int outputStauts;//输出状态 
		int motorStatus;//电机状态
	} motionInfo;

	QMutex mutex; //线程锁
	
	ErrorCode errorCode; //控制器的错误码
	Operation operation; //操作指令

public:
	MotionControler(QThread *parent = Q_NULLPTR);
	~MotionControler();

	inline void setCaller(int c) { caller = c; };
	inline int getCaller() { return caller; }

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } 
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	bool initControler(); //初始化
	bool moveForward(); //前进
	bool returnToZero(); //归零
	bool returnToZero2(); //归零
	bool moveToInitialPos();//移动到初始位置，到位即可拍照
	bool motionReset(); //运动复位
	bool resetControler(); //重置控制器


	inline void setOperation(Operation op) { operation = op; }
	inline bool isReady() { return (this != Q_NULLPTR) && (errorCode == NoError); }
	inline ErrorCode getErrorCode() { return errorCode; } //获取当前的错误代码
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

private:
	void markInitFailed();
	bool _AMC98_AddParamPC2CNC(int paramNum, int data);
	void getMotionData(int portIndex);

protected:
	void run();

Q_SIGNALS:
	void UartNetGetNewData_motion();

	void initControlerFinished_motion(int);
	void motionResetFinished_motion(int);
	void moveToInitialPosFinished_motion(int);
	void returnToZeroFinished_motion(int);
	void moveForwardFinished_motion(int);
};

