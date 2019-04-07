#pragma once
#include <QObject>
#include "Configurator.h"
//#include "AMC98C.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


namespace pcb {
	class ImageConverter;
}

//运动控制器
class MotionControler : public QObject
{
	Q_OBJECT

public:
	//运动结构的相关操作
	enum Operation {
		NoOperation,
		InitCameras,
		TakePhoto
	};

	//运动结构的错误代码
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x300,
		InitFailed = 0x301,
		moveForwardFailed = 0x302,
		returnToZeroFailed = 0x303,
		resetControler = 0x304
	};

private:
	pcb::DetectConfig *detectConfig;
	pcb::AdminConfig *adminConfig;
	int callerOfResetControler; //复位的调用函数的标识
	ErrorCode errorCode; //控制器的错误码
	Operation operation;//操作指令

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } 
	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 

	void initControler(); //初始化
	void moveForward(); //前进
	void returnToZero(); //归零
	void resetControler(int caller); //复位

	inline ErrorCode getErrorCode() { return errorCode; } //获取当前的错误代码
	bool showMessageBox(QWidget *parent); //弹窗警告

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

