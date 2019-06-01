#include "MotionControler.h"
//#include "WinNT.h"

//PortIndex 控制器编号0,1,2,3,4
//typedef int(*Uart_AMC98_GetNewDataType)(int, unsigned char *, int);
//int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth);
//int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth)
//{
//	//getMotionData(PortIndex);
//	//emit UartNetGetNewData_motion();
//	return 0;
//}


//控制器的构造函数
MotionControler::MotionControler(QThread *parent)
	: QThread(parent)
{
	caller = -1; //模块的调用者
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	xMotionPos = 0; //X轴的位置
	errorCode = Unchecked; //控制器的错误码
	operation = NoOperation;//操作指令
}

MotionControler::~MotionControler()
{
	qDebug() << "~MotionControler";
}


/************** 启动线程 **************/

void MotionControler::run()
{
	switch (operation)
	{
	case MotionControler::NoOperation:
		break;
	case MotionControler::InitControler: //初始化
		if (!initControler()) {
			errorCode = ErrorCode::InitFailed; 
		}
		break;
	case MotionControler::MoveForward: //前进
		if (!moveForward()) {
			errorCode = ErrorCode::MoveForwardFailed;
		}
		break;
	case MotionControler::ReturnToZero: //归零
		if (!returnToZero()) {
			errorCode = ErrorCode::ReturnToZeroFailed;
		}
		break;
	case MotionControler::MoveToInitialPos: //运动到初始拍照位置
		if (!moveToInitialPos()) {
			errorCode = ErrorCode::MoveToInitialPosFailed;
		}
		break;
	case MotionControler::MotionReset: //复位
		if (!motionReset()) {
			errorCode = ErrorCode::MotionResetFailed;
		}
		break;
	case MotionControler::ResetControler:
		if (!resetControler()) {
			errorCode = ErrorCode::ResetControlerFailed;
		}
	}
}

//获取控制器状态信息 - 没有用
//portIndex 控制器编号0,1,2,3,4
void MotionControler::getMotionData(int portIndex)
{
	int *d = AMC98_GetCNCDataBuffer();
	motionInfo.xPos = d[137];//X轴当前位置
	motionInfo.yPos = d[149];//Y轴当前位置
	motionInfo.outputStauts = d[1];//输出状态
	motionInfo.inputStatus = d[4];//输入状态
	motionInfo.motorStatus = d[218];//电机状态
	int ts = d[226];
	CString strts = AMC98_GetInfString(ts);//提示信息
	int cw = d[227];
	CString strerr = AMC98_GetErrString(cw);//错误信息
}


/******************* 初始化 *********************/

//初始化，下载参数
bool MotionControler::initControler()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked; //控制器的错误码

	//设置端口数据更新的回调函数
	//Uart_AMC98_GetNewData(CallBack_UartNetGetNewData);

	//0控制器类型
	if (AMC98_KZQSet_v2(0, 0, "4")) { markInitFailed(); return false; }
	pcb::delay(10);
	//1设置
	if (AMC98_KZQSet_v2(0, 1, "9600,N,8,1")) { markInitFailed(); return false; }
	pcb::delay(10);
	//2端口
	QString _port = "";
	if (userConfig->clusterComPort.size() == 4) 
		_port = userConfig->clusterComPort.at(3);
	std::string port = _port.toStdString();
	if (AMC98_KZQSet_v2(0, 2, port.c_str())) { markInitFailed(); return false; }
	pcb::delay(10);
	//3协议
	//str.Format(_T("%d"), xieyi_byte);
	if (AMC98_KZQSet_v2(0, 3, "2")) { markInitFailed(); return false; }
	pcb::delay(10);

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) { markInitFailed(); return false; }
	pcb::delay(100);

	//连接控制器 - 方式2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_initControler_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//进一步检查运动结构的最大行程
	double maxDist = adminConfig->MaxMotionStroke;
	int PulseNumInUnitTime = adminConfig->PulseNumInUnitTime;

	//设置控制指令 - X轴
	if (!_AMC98_AddParamPC2CNC(138, int(maxDist * 10000))) return false; //轴最大值
	if (!_AMC98_AddParamPC2CNC(145, 0 * 10000)) return false;//轴最小值
	if (!_AMC98_AddParamPC2CNC(139, 10 * 10000)) return false;//轴开始速度
	if (!_AMC98_AddParamPC2CNC(140, 50 * 10000)) return false;//轴结束数度
	if (!_AMC98_AddParamPC2CNC(141, 10 * 10000)) return false;//回原点慢速度
	if (!_AMC98_AddParamPC2CNC(142, 30 * 10000)) return false;//回原点快速度
	if (!_AMC98_AddParamPC2CNC(143, 5 * 10000)) return false;//原点位置
	if (!_AMC98_AddParamPC2CNC(146, int(PulseNumInUnitTime * 10000))) return false;//1单位脉冲数
	if (!_AMC98_AddParamPC2CNC(144, 2)) return false;//回原点设置 2负方向复位 1正方向复位
	if (!_AMC98_AddParamPC2CNC(147, 200)) return false;//加速时间
	if (!_AMC98_AddParamPC2CNC(148, 200)) return false;//减速时间
	if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//需要复位的电机 1要X复位 3要XY复位 0不要复位
	if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//保存数据
	pcb::delay(500);

	//设置控制指令 - Y轴
	//if (!_AMC98_AddParamPC2CNC(150, maxDist * 10000)) return false;//Y轴最大值
	//if (!_AMC98_AddParamPC2CNC(157, 0 * 10000)) return false;//轴最小值
	//if (!_AMC98_AddParamPC2CNC(151, 10 * 10000)) return false;//轴开始速度
	//if (!_AMC98_AddParamPC2CNC(152, 50 * 10000)) return false;//轴结束数度
	//if (!_AMC98_AddParamPC2CNC(153, 10 * 10000)) return false;//回原点慢速度
	//if (!_AMC98_AddParamPC2CNC(154, 30 * 10000)) return false;//回原点快速度
	//if (!_AMC98_AddParamPC2CNC(155, 5 * 10000)) return false;//原点位置
	//if (!_AMC98_AddParamPC2CNC(158, 85 * 10000)) return false;//1单位脉冲数
	//if (!_AMC98_AddParamPC2CNC(156, 2)) return false;//回原点设置 2负方向复位 1正方向复位
	//if (!_AMC98_AddParamPC2CNC(159, 200)) return false;//加速时间
	//if (!_AMC98_AddParamPC2CNC(160, 200)) return false;//减速时间
	//if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//需要复位的电机 1要X复位 3要XY复位 0不要复位
	//if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//保存数据
	
	if (AMC98_ParamPC2CNC() != 0) return false;//放在最后面
	pcb::delay(500);

	//初始化结束
	errorCode = ErrorCode::NoError;
	emit initControlerFinished_motion(errorCode);
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
	emit initControlerFinished_motion(errorCode);
}


/*********** 运动至初始位置、前进 ************/

//移动到初始拍照位置
//到位后即可调用相机执行第一行图像的拍摄
bool MotionControler::moveToInitialPos()
{
	QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) {
		errorCode = ErrorCode::MoveToInitialPosFailed;
		emit moveToInitialPosFinished_motion(errorCode);
		return false;
	}
	pcb::delay(50);

	//发送控制指令
	int endingPos = runtimeParams->initialPhotoPos; //初始拍照位置
	if (AMC98_start_sr_move(2, 0, endingPos, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MoveToInitialPosFailed;
		emit moveToInitialPosFinished_motion(errorCode);
		return false;
	}

	//等待运动到位
	//int motionTime = 0; //运动累计时间
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos == endingPos) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //延时
	//	motionTime += 500; //更新累计时间
	//	if (motionTime > 30000) { //超时 30s
	//		errorCode = ErrorCode::MoveToInitialPosFailed;
	//		emit moveToInitialPosFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(8000);

	//到达初始拍照位置
	errorCode = ErrorCode::NoError;
	emit moveToInitialPosFinished_motion(errorCode);
	return true;
}


//前进
bool MotionControler::moveForward()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) { 
		errorCode = ErrorCode::MoveForwardFailed;
		emit moveForwardFinished_motion(errorCode);
		return false; 
	}
	pcb::delay(50);

	//发送控制指令
	double dist = -runtimeParams->singleMotionStroke;
	if (AMC98_start_sr_move(2, 0, dist, WeizhiType_XD, 10, 50, 100, 100, 0, 0) != 0) {
		errorCode = ErrorCode::MoveForwardFailed;
		emit moveForwardFinished_motion(errorCode);
		return false;
	}

	//等待运动到位
	//int motionTime = 0; //运动累计时间
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos = xMotionPos + dist) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //延时
	//	motionTime += 500; //更新累计时间
	//	if (motionTime > 20000) { //超时 20s
	//		errorCode = ErrorCode::MoveForwardFailed;
	//		emit moveForwardFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(3000);

	//前进结束
	errorCode = ErrorCode::NoError;
	emit moveForwardFinished_motion(errorCode);
	return true;
}


/******************* 归零 ******************/

//运动结构归零
bool MotionControler::returnToZero()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//连接控制器
	if (AMC98_Connect(Q_NULLPTR, 0) != 0) { 
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false; 
	}
	pcb::delay(100);

	//发送控制指令
	int zeroPos = 0;
	if (AMC98_Comand(1, NULL) != 0) {
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false;
	}

	//等待运动到位
	//int motionTime = 0; //运动累计时间
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos == zeroPos) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //延时
	//	motionTime += 500; //更新累计时间
	//	if (motionTime > 30000) { //超时 30s
	//		errorCode = ErrorCode::ReturnToZeroFailed;
	//		emit returnToZeroFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(10000);

	//归零结束
	errorCode = ErrorCode::NoError;
	emit returnToZeroFinished_motion(errorCode);
	return true;
}

bool MotionControler::returnToZero2()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//连接控制器
	if (AMC98_Connect(Q_NULLPTR, 0) != 0) {
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false;
	}
	pcb::delay(100);

	//发送控制指令
	int zeroPos = 0;
	if (AMC98_start_sr_move(2, 0, zeroPos, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false;
	}

	//等待运动到位
	//int motionTime = 0; //运动累计时间
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos == zeroPos) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //延时
	//	motionTime += 500; //更新累计时间
	//	if (motionTime > 30000) { //超时 30s
	//		errorCode = ErrorCode::ReturnToZeroFailed;
	//		emit returnToZeroFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(9000);

	//归零结束
	errorCode = ErrorCode::NoError;
	emit returnToZeroFinished_motion(errorCode);
	return true;
}



/******************* 运动复位 ******************/

//运动结构复位
bool MotionControler::motionReset()
{
	QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//连接控制器 - 方式1
	if (AMC98_Connect(NULL, 0) != 0) {
		errorCode = ErrorCode::MotionResetFailed;
		emit motionResetFinished_motion(errorCode);
		return false; 
	}
	pcb::delay(100);

	//发送控制指令
	//if (AMC98_Comand(1, NULL) != 0) { return false; }

	//移动到合适的位置，此时可以放置新的PCB板
	int resetPos = adminConfig->MaxMotionStroke;
	if (AMC98_start_sr_move(2, 0, resetPos, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MotionResetFailed;
		emit motionResetFinished_motion(errorCode);
		return false;
	}

	//等待运动到位
	//int motionTime = 0; //运动累计时间
	//while (true) {
	//	this->getMotionData(4);
	//	xMotionPos = motionInfo.xPos;
	//	if (motionInfo.xPos == resetPos) break;
	//	pcb::delay(500); //延时
	//	motionTime += 500; //更新累计时间
	//	if (motionTime > 30000) { //超时 30s
	//		errorCode = ErrorCode::ResetControlerFailed;
	//		emit resetControlerFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(10000); //复位等待

	//复位结束
	errorCode = ErrorCode::NoError;
	emit motionResetFinished_motion(errorCode);
	return true;
}


/******************* 重置控制器 ******************/

bool MotionControler::resetControler()
{
	//重新初始化
	if (!initControler()) return false;

	//运动结构归零
	if (!returnToZero()) return false;

	return true;
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
	case MotionControler::Unchecked:
		warningMessage = pcb::chinese("未确认运动结构的工作状态！ "); break;
	case MotionControler::InitFailed:
		warningMessage = pcb::chinese("运动结构初始化失败！ "); break;
	case MotionControler::MoveForwardFailed:
		warningMessage = pcb::chinese("运动前进失败！ "); break;
	case MotionControler::ReturnToZeroFailed:
		warningMessage = pcb::chinese("运动归零失败！ "); break;
	case MotionControler::MoveToInitialPosFailed:
		warningMessage = pcb::chinese("运动至初始拍照位置失败！ "); break;
	case MotionControler::MotionResetFailed:
		warningMessage = pcb::chinese("运动复位失败！ "); break;
	case MotionControler::ResetControlerFailed:
		warningMessage = pcb::chinese("运动控制器重置失败！ "); break;
	case MotionControler::Default:
		warningMessage = pcb::chinese("未知错误！ "); break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		warningMessage + "    \n" +
		"MotionControler: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}
