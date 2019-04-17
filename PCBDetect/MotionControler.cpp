#include "MotionControler.h"
//#include "WinNT.h"

//PortIndex 控制器编号0,1,2,3,4,
typedef int (*Uart_AMC98_GetNewDataType)(int,unsigned char *,int);  //函数指针类型
int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth);
int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth)
{
	//theApp.GetNewData(PortIndex);
	return 0;
}

MotionControler::MotionControler(QObject *parent)
	: QObject(parent)
{
	adminConfig = Q_NULLPTR; //系统参数
	detectConfig = Q_NULLPTR; //用户参数
	detectParams = Q_NULLPTR; //运行参数
	errorCode = Uncheck; //控制器的错误码
	running = false; //操作是否正在运行
}

MotionControler::~MotionControler()
{
}


/******************* 运动控制 *********************/

//初始化，下载参数
bool MotionControler::initControler()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck; //控制器的错误码

	Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//0控制器类型
	if (AMC98_KZQSet_v2(0, 0, "4") != 0) {
		markInitFailed(); return false;
	}
	//1设置
	if (AMC98_KZQSet_v2(0, 1, "9600,N,8,1") != 0) {
		markInitFailed(); return false;
	}
	//2端口
	if (AMC98_KZQSet_v2(0, 2, "4") != 0) {
		markInitFailed(); return false;
	}
	//3协议
	//str.Format(_T("%d"), xieyi_byte);
	if (AMC98_KZQSet_v2(0, 3, "2") != 0) {
		markInitFailed(); return false;
	}

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}

	//连接控制器 - 方式2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_initControler_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//进一步检查运动结构的最大行程
	int maxDist = adminConfig->MaxMotionStroke;
	if (maxDist > 500) {
		qDebug() << "invalid maxDist"; return false;
	}

	//设置控制指令 - X轴
	if (!_AMC98_AddParamPC2CNC(138, maxDist * 10000)) return false; //轴最大值
	if (!_AMC98_AddParamPC2CNC(145, 0 * 10000)) return false;//轴最小值
	if (!_AMC98_AddParamPC2CNC(139, 10 * 10000)) return false;//轴开始速度
	if (!_AMC98_AddParamPC2CNC(140, 50 * 10000)) return false;//轴结束数度
	if (!_AMC98_AddParamPC2CNC(141, 10 * 10000)) return false;//回原点慢速度
	if (!_AMC98_AddParamPC2CNC(142, 30 * 10000)) return false;//回原点快速度
	if (!_AMC98_AddParamPC2CNC(143, 5 * 10000)) return false;//原点位置
	if (!_AMC98_AddParamPC2CNC(146, 85 * 10000)) return false;//1单位脉冲数
	if (!_AMC98_AddParamPC2CNC(144, 2)) return false;//回原点设置 2负方向复位 1正方向复位
	if (!_AMC98_AddParamPC2CNC(147, 200)) return false;//加速时间
	if (!_AMC98_AddParamPC2CNC(148, 200)) return false;//减速时间
	if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//需要复位的电机 1要X复位 3要XY复位 0不要复位
	if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//保存数据


	//设置控制指令 - Y轴
	if (!_AMC98_AddParamPC2CNC(150, maxDist * 10000)) return false;//Y轴最大值
	if (!_AMC98_AddParamPC2CNC(157, 0 * 10000)) return false;//轴最小值
	if (!_AMC98_AddParamPC2CNC(151, 10 * 10000)) return false;//轴开始速度
	if (!_AMC98_AddParamPC2CNC(152, 50 * 10000)) return false;//轴结束数度
	if (!_AMC98_AddParamPC2CNC(153, 10 * 10000)) return false;//回原点慢速度
	if (!_AMC98_AddParamPC2CNC(154, 30 * 10000)) return false;//回原点快速度
	if (!_AMC98_AddParamPC2CNC(155, 5 * 10000)) return false;//原点位置
	if (!_AMC98_AddParamPC2CNC(158, 85 * 10000)) return false;//1单位脉冲数
	if (!_AMC98_AddParamPC2CNC(156, 2)) return false;//回原点设置 2负方向复位 1正方向复位
	if (!_AMC98_AddParamPC2CNC(159, 200)) return false;//加速时间
	if (!_AMC98_AddParamPC2CNC(160, 200)) return false;//减速时间
	if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//需要复位的电机 1要X复位 3要XY复位 0不要复位
	if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//保存数据
	if (AMC98_ParamPC2CNC() != 0) return false;//放在最后面

	//初始化结束
	errorCode = ErrorCode::NoError;
	running = false;
	emit initControlerFinished_motion();
	return true;
}

//设置参数
bool MotionControler::_AMC98_AddParamPC2CNC(int paramNum, int data)
{
	if (AMC98_AddParamPC2CNC(paramNum, data) != 0) { //轴最大值
		markInitFailed(); return false;
	}
	return true;
}

//标记初始化失败
void MotionControler::markInitFailed()
{
	errorCode = ErrorCode::InitFailed;
	running = false;
	emit initControlerFinished_motion();
}


//运动结构前进
bool MotionControler::moveForward()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}

	//连接控制器 - 方式2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_moveForward_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//发送控制指令
	double dist = -detectParams->singleMotionStroke;

	clock_t t1 = clock();
	if (AMC98_start_sr_move(2, 0, dist, WeizhiType_XD, 10, 50, 100, 100, 0, 0) != 0) {
		errorCode = ErrorCode::MoveForwardFailed;
		running = false; 
		emit moveForwardFinished_motion();
		return false;
	}
	clock_t t2 = clock();
	qDebug() << "AMC98_start_sr_move" << t2 - t1;

	pcb::delay(8000);

	//前进结束
	errorCode = ErrorCode::NoError;
	running = false;
	emit moveForwardFinished_motion();
	return true;
}


//运动结构归零
bool MotionControler::returnToZero()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//连接控制器 - 方式1
	int counter = RETRY_NUM; //计数器
	while (counter > 0) {
		if (!AMC98_Connect(Q_NULLPTR, 0)) break;
		counter -= 1; //更新计数器
		if (counter == 0) { markInitFailed(); return false; }
	}

	//连接控制器 - 方式2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_returnToZero_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//发送控制指令
	if (AMC98_start_sr_move(2, 0, 0, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MoveForwardFailed;
		running = false;
		emit returnToZeroFinished_motion();
		return false;
	}

	//归零结束
	errorCode = ErrorCode::NoError;
	running = false;
	emit returnToZeroFinished_motion();
	return true;
}

//移动到初始拍照位置
//到位后即可调用相机执行第一行图像的拍摄
bool MotionControler::moveToInitialPos()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}

	//连接控制器 - 方式2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_returnToZero_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//发送控制指令
	if (AMC98_start_sr_move(2, 0, 245, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MoveToInitialPosFailed;
		running = false;
		emit moveToInitialPosFinished_motion();
		return false;
	}

	pcb::delay(15000);

	//移动结束
	errorCode = ErrorCode::NoError;
	running = false;
	emit moveToInitialPosFinished_motion();
	return true;
}

/**
 * 功能：运动结构复位
 * 参数：caller 调用者的标识
 */
bool MotionControler::resetControler()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}
	
	//连接控制器 - 方式2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_resetControler_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//发送控制指令
	//if (AMC98_Comand(1, NULL) != 0) { return false; }

	//移动到合适的位置，此时可以放置新的PCB板
	if (AMC98_start_sr_move(2, 0, 450, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::ResetControlerFailed;
		running = false;
		emit resetControlerFinished_motion();
		return false;
	}

	//复位结束
	errorCode = ErrorCode::NoError;
	running = false;
	emit resetControlerFinished_motion();
	return true;
}


/********* 判断操作状态、发送结束信号 - 暂时没用 ***********/

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
	emit resetControlerFinished_motion();
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
		warningMessage = pcb::chinese("运动结构归零失败！    "); break;
	case MotionControler::MoveToInitialPosFailed:
		warningMessage = pcb::chinese("运动至初始拍照位置失败！    "); break;
	case MotionControler::ResetControlerFailed:
		warningMessage = pcb::chinese("运动结构复位失败！    "); break;
	case MotionControler::Default:
		warningMessage = pcb::chinese("未知错误！"); break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		warningMessage + "    \n" +
		"MotionControler: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}
