#include "MotionControler.h"

MotionControler::MotionControler(QObject *parent)
	: QObject(parent)
{
	detectConfig = Q_NULLPTR;
	adminConfig = Q_NULLPTR;
	callerOfResetControler = 0; //复位的调用函数的标识
	errorCode = Uncheck; //控制器的错误码
	running = false; //操作是否正在运行
}

MotionControler::~MotionControler()
{
}


/******************* 运动控制 *********************/

//初始化，下载参数
void MotionControler::initControler()
{
	QMutexLocker locker(&mutex);
	if (running) return;
	else running = true;

	////0控制器类型
	//AMC98_KZQSet(0, 0, _T("4"));
	////1设置
	//AMC98_KZQSet(0, 1, _T("9600,N,8,1"));
	////2端口
	//CString str;
	//str.Format(_T("%d"), 4);//COM1就设置1
	//AMC98_KZQSet(0, 2, str);
	////3协议
	//str.Format(_T("%d"), xieyi_byte);
	//AMC98_KZQSet(0, 3, str);
	//
	////AMC98_Connect(NULL, 0);//放在最前面
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_initControler_finished;
	//AMC98_Connect((HWND)(&funcPtr), 0);

	//AMC98_AddParamPC2CNC(138, (int)(500 * 10000));//轴最大值
	//AMC98_AddParamPC2CNC(145, (int)(0 * 10000));//轴最小值
	//AMC98_AddParamPC2CNC(139, (int)(10 * 10000));//轴开始速度
	//AMC98_AddParamPC2CNC(140, (int)(50 * 10000));//轴结束数度
	//AMC98_AddParamPC2CNC(141, (int)(10 * 10000));//回原点慢速度
	//AMC98_AddParamPC2CNC(142, (int)(30 * 10000));//回原点快速度
	//AMC98_AddParamPC2CNC(143, (int)(5 * 10000));//原点位置
	//AMC98_AddParamPC2CNC(146, (int)(66 * 10000));//1单位脉冲数
	//AMC98_AddParamPC2CNC(144, 2);//回原点设置 2负方向复位 1正方向复位
	//AMC98_AddParamPC2CNC(147, 200);//加速时间
	//AMC98_AddParamPC2CNC(148, 200);//减速时间
	//AMC98_AddParamPC2CNC(243, 1);//需要复位的电机 1要X复位 3要XY复位 0不要复位
	//AMC98_AddParamPC2CNC(60000, 3);//保存数据

	//AMC98_AddParamPC2CNC(150, (int)(500 * 10000));//Y轴最大值
	//AMC98_AddParamPC2CNC(157, (int)(0 * 10000));//轴最小值
	//AMC98_AddParamPC2CNC(151, (int)(10 * 10000));//轴开始速度
	//AMC98_AddParamPC2CNC(152, (int)(50 * 10000));//轴结束数度
	//AMC98_AddParamPC2CNC(153, (int)(10 * 10000));//回原点慢速度
	//AMC98_AddParamPC2CNC(154, (int)(30 * 10000));//回原点快速度
	//AMC98_AddParamPC2CNC(155, (int)(5 * 10000));//原点位置
	//AMC98_AddParamPC2CNC(158, (int)(66 * 10000));//1单位脉冲数
	//AMC98_AddParamPC2CNC(156, 2);//回原点设置 2负方向复位 1正方向复位
	//AMC98_AddParamPC2CNC(159, 200);//加速时间
	//AMC98_AddParamPC2CNC(160, 200);//减速时间
	//AMC98_AddParamPC2CNC(243, 1);//需要复位的电机 1要X复位 3要XY复位 0不要复位
	//AMC98_AddParamPC2CNC(60000, 3);//保存数据
	//AMC98_ParamPC2CNC();//放在最后面

	//用于临时测试
	pcb::delay(1000);
	//running = false;
	//emit initControlerFinished_motion();

	on_initControler_finished();
}

//运动结构前进
void MotionControler::moveForward()
{
	QMutexLocker locker(&mutex);
	if (running) return;
	running = true;

	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_moveForward_finished;
	//AMC98_Connect((HWND)(&funcPtr), 0);

	////AMC98_Connect(NULL, 0);
	//AMC98_start_sr_move(2, 0, 80, WeizhiType_XD, 10, 50, 100, 100, 0, 0);
}

//运动结构归零
void MotionControler::returnToZero()
{
	QMutexLocker locker(&mutex);
	if (running) return;
	running = true;

	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_returnToZero_finished;
	//AMC98_Connect((HWND)(&funcPtr), 0);

	////AMC98_Connect(NULL, 0);
	//AMC98_start_sr_move(2, 0, 0, WeizhiType_JD, 10, 100, 200, 200, 0, 0);
}

/**
 * 功能：运动结构复位
 * 参数：caller 调用者的标识
 */
void MotionControler::resetControler(int caller)
{
	QMutexLocker locker(&mutex);
	if (running) return;
	running = true;

	callerOfResetControler  = caller;

	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_resetControler_finished;
	//AMC98_Connect((HWND)(&funcPtr), 0);

	////AMC98_Connect(NULL, 0);
	//AMC98_Comand(1, NULL);
}


/************ 判断操作状态、发送结束信号 *************/

//判断当前是否有正在运行的操作
bool MotionControler::isRunning()
{ 
	QMutexLocker locker(&mutex);
	return running; 
}

//发送初始化结束的信号
void MotionControler::on_initControler_finished()
{
	//QMutexLocker locker(&mutex);
	errorCode = NoError;
	running = false;
	emit initControlerFinished_motion();
}

//发送运动结构前进结束的信号
void MotionControler::on_moveForward_finished()
{
	//QMutexLocker locker(&mutex);
	running = false;
	emit moveForwardFinished_motion();
}

//发送运动结构归零结束的信号
void MotionControler::on_returnToZero_finished()
{
	//QMutexLocker locker(&mutex);
	running = false;
	emit returnToZeroFinished_motion();
}

//发送运动结构复位结束的信号
void MotionControler::on_resetControler_finished()
{
	//QMutexLocker locker(&mutex);
	running = false;
	emit resetControlerFinished_motion(callerOfResetControler);
}


/******************* 其他 ******************/

//弹窗警告
bool MotionControler::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == MotionControler::NoError) return false;

	QString warningMessage;
	switch (tempCode)
	{
	case MotionControler::Uncheck:
		warningMessage = pcb::chinese("未确认运动结构的工作状态！"); break;
	case MotionControler::InitFailed:
		warningMessage = pcb::chinese("运动结构初始化失败！  "); break;
	case MotionControler::MoveForwardFailed:
		warningMessage = pcb::chinese("运动结构前进失败！    "); break;
	case MotionControler::ReturnToZeroFailed:
		warningMessage = pcb::chinese("运动机构归零失败！    "); break;
	case MotionControler::ResetControlerFailed:
		warningMessage = pcb::chinese("运动机构复位失败！    "); break;
	default:
		warningMessage = pcb::chinese("未知错误！"); break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		warningMessage + "    \n" +
		"MotionControler: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}
