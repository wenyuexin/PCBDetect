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

private:
	pcb::DetectConfig *config;
	int callerOfResetControler; //复位的调用函数的标识

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	//inline void setDetectConfig(pcb::DetectConfig *ptr = Q_NULLPTR) { config = ptr; } 

	void initControler(); //初始化
	void moveForward(); //前进
	void returnToZero(); //归零
	void resetControler(int caller); //复位

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

