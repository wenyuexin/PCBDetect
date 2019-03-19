#pragma once
#include <QObject>
#include "Configurator.h"
#include "AMC98C.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//运动控制器
class MotionControler : public QObject
{
	Q_OBJECT

private:
	Ui::DetectConfig *config;

public:
	MotionControler(QObject *parent);
	~MotionControler();

	inline void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }

	void initControler(); //初始化
	void moveForward(); //前进
	void returnToZero(); //归零
	void resetControler(); //复位

private:
	void on_initControler_finished();
	void on_moveForward_finished();
	void on_returnToZero_finished();
	void on_resetControler_finished();

Q_SIGNALS:
	void signal_moveForward_finished();
};

