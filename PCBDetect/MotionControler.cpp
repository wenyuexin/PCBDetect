#include "MotionControler.h"
//#include "WinNT.h"

//PortIndex ���������0,1,2,3,4
//typedef int(*Uart_AMC98_GetNewDataType)(int, unsigned char *, int);
//int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth);
//int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth)
//{
//	//getMotionData(PortIndex);
//	//emit UartNetGetNewData_motion();
//	return 0;
//}


//�������Ĺ��캯��
MotionControler::MotionControler(QThread *parent)
	: QThread(parent)
{
	caller = -1; //ģ��ĵ�����
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	xMotionPos = 0; //X���λ��
	errorCode = Unchecked; //�������Ĵ�����
	operation = NoOperation;//����ָ��
}

MotionControler::~MotionControler()
{
	qDebug() << "~MotionControler";
}


/************** �����߳� **************/

void MotionControler::run()
{
	switch (operation)
	{
	case MotionControler::NoOperation:
		break;
	case MotionControler::InitControler: //��ʼ��
		if (!initControler()) {
			errorCode = ErrorCode::InitFailed; 
		}
		break;
	case MotionControler::MoveForward: //ǰ��
		if (!moveForward()) {
			errorCode = ErrorCode::MoveForwardFailed;
		}
		break;
	case MotionControler::ReturnToZero: //����
		if (!returnToZero()) {
			errorCode = ErrorCode::ReturnToZeroFailed;
		}
		break;
	case MotionControler::MoveToInitialPos: //�˶�����ʼ����λ��
		if (!moveToInitialPos()) {
			errorCode = ErrorCode::MoveToInitialPosFailed;
		}
		break;
	case MotionControler::MotionReset: //��λ
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

//��ȡ������״̬��Ϣ - û����
//portIndex ���������0,1,2,3,4
void MotionControler::getMotionData(int portIndex)
{
	int *d = AMC98_GetCNCDataBuffer();
	motionInfo.xPos = d[137];//X�ᵱǰλ��
	motionInfo.yPos = d[149];//Y�ᵱǰλ��
	motionInfo.outputStauts = d[1];//���״̬
	motionInfo.inputStatus = d[4];//����״̬
	motionInfo.motorStatus = d[218];//���״̬
	int ts = d[226];
	CString strts = AMC98_GetInfString(ts);//��ʾ��Ϣ
	int cw = d[227];
	CString strerr = AMC98_GetErrString(cw);//������Ϣ
}


/******************* ��ʼ�� *********************/

//��ʼ�������ز���
bool MotionControler::initControler()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked; //�������Ĵ�����

	//���ö˿����ݸ��µĻص�����
	//Uart_AMC98_GetNewData(CallBack_UartNetGetNewData);

	//0����������
	if (AMC98_KZQSet_v2(0, 0, "4")) { markInitFailed(); return false; }
	pcb::delay(10);
	//1����
	if (AMC98_KZQSet_v2(0, 1, "9600,N,8,1")) { markInitFailed(); return false; }
	pcb::delay(10);
	//2�˿�
	QString _port = "";
	if (userConfig->clusterComPort.size() == 4) 
		_port = userConfig->clusterComPort.at(3);
	std::string port = _port.toStdString();
	if (AMC98_KZQSet_v2(0, 2, port.c_str())) { markInitFailed(); return false; }
	pcb::delay(10);
	//3Э��
	//str.Format(_T("%d"), xieyi_byte);
	if (AMC98_KZQSet_v2(0, 3, "2")) { markInitFailed(); return false; }
	pcb::delay(10);

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) { markInitFailed(); return false; }
	pcb::delay(100);

	//���ӿ����� - ��ʽ2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_initControler_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//��һ������˶��ṹ������г�
	double maxDist = adminConfig->MaxMotionStroke;
	int PulseNumInUnitTime = adminConfig->PulseNumInUnitTime;

	//���ÿ���ָ�� - X��
	if (!_AMC98_AddParamPC2CNC(138, int(maxDist * 10000))) return false; //�����ֵ
	if (!_AMC98_AddParamPC2CNC(145, 0 * 10000)) return false;//����Сֵ
	if (!_AMC98_AddParamPC2CNC(139, 10 * 10000)) return false;//�Ὺʼ�ٶ�
	if (!_AMC98_AddParamPC2CNC(140, 50 * 10000)) return false;//���������
	if (!_AMC98_AddParamPC2CNC(141, 10 * 10000)) return false;//��ԭ�����ٶ�
	if (!_AMC98_AddParamPC2CNC(142, 30 * 10000)) return false;//��ԭ����ٶ�
	if (!_AMC98_AddParamPC2CNC(143, 5 * 10000)) return false;//ԭ��λ��
	if (!_AMC98_AddParamPC2CNC(146, int(PulseNumInUnitTime * 10000))) return false;//1��λ������
	if (!_AMC98_AddParamPC2CNC(144, 2)) return false;//��ԭ������ 2������λ 1������λ
	if (!_AMC98_AddParamPC2CNC(147, 200)) return false;//����ʱ��
	if (!_AMC98_AddParamPC2CNC(148, 200)) return false;//����ʱ��
	if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//��������
	pcb::delay(500);

	//���ÿ���ָ�� - Y��
	//if (!_AMC98_AddParamPC2CNC(150, maxDist * 10000)) return false;//Y�����ֵ
	//if (!_AMC98_AddParamPC2CNC(157, 0 * 10000)) return false;//����Сֵ
	//if (!_AMC98_AddParamPC2CNC(151, 10 * 10000)) return false;//�Ὺʼ�ٶ�
	//if (!_AMC98_AddParamPC2CNC(152, 50 * 10000)) return false;//���������
	//if (!_AMC98_AddParamPC2CNC(153, 10 * 10000)) return false;//��ԭ�����ٶ�
	//if (!_AMC98_AddParamPC2CNC(154, 30 * 10000)) return false;//��ԭ����ٶ�
	//if (!_AMC98_AddParamPC2CNC(155, 5 * 10000)) return false;//ԭ��λ��
	//if (!_AMC98_AddParamPC2CNC(158, 85 * 10000)) return false;//1��λ������
	//if (!_AMC98_AddParamPC2CNC(156, 2)) return false;//��ԭ������ 2������λ 1������λ
	//if (!_AMC98_AddParamPC2CNC(159, 200)) return false;//����ʱ��
	//if (!_AMC98_AddParamPC2CNC(160, 200)) return false;//����ʱ��
	//if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	//if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//��������
	
	if (AMC98_ParamPC2CNC() != 0) return false;//���������
	pcb::delay(500);

	//��ʼ������
	errorCode = ErrorCode::NoError;
	emit initControlerFinished_motion(errorCode);
	return true;
}

//���ò���
bool MotionControler::_AMC98_AddParamPC2CNC(int paramNum, int data)
{
	if (AMC98_AddParamPC2CNC(paramNum, data) != 0) { //�����ֵ
		markInitFailed(); return false;
	}
	return true;
}

//��ǳ�ʼ��ʧ��
void MotionControler::markInitFailed()
{
	errorCode = ErrorCode::InitFailed;
	emit initControlerFinished_motion(errorCode);
}


/*********** �˶�����ʼλ�á�ǰ�� ************/

//�ƶ�����ʼ����λ��
//��λ�󼴿ɵ������ִ�е�һ��ͼ�������
bool MotionControler::moveToInitialPos()
{
	QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) {
		errorCode = ErrorCode::MoveToInitialPosFailed;
		emit moveToInitialPosFinished_motion(errorCode);
		return false;
	}
	pcb::delay(50);

	//���Ϳ���ָ��
	int endingPos = runtimeParams->initialPhotoPos; //��ʼ����λ��
	if (AMC98_start_sr_move(2, 0, endingPos, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MoveToInitialPosFailed;
		emit moveToInitialPosFinished_motion(errorCode);
		return false;
	}

	//�ȴ��˶���λ
	//int motionTime = 0; //�˶��ۼ�ʱ��
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos == endingPos) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //��ʱ
	//	motionTime += 500; //�����ۼ�ʱ��
	//	if (motionTime > 30000) { //��ʱ 30s
	//		errorCode = ErrorCode::MoveToInitialPosFailed;
	//		emit moveToInitialPosFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(8000);

	//�����ʼ����λ��
	errorCode = ErrorCode::NoError;
	emit moveToInitialPosFinished_motion(errorCode);
	return true;
}


//ǰ��
bool MotionControler::moveForward()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) { 
		errorCode = ErrorCode::MoveForwardFailed;
		emit moveForwardFinished_motion(errorCode);
		return false; 
	}
	pcb::delay(50);

	//���Ϳ���ָ��
	double dist = -runtimeParams->singleMotionStroke;
	if (AMC98_start_sr_move(2, 0, dist, WeizhiType_XD, 10, 50, 100, 100, 0, 0) != 0) {
		errorCode = ErrorCode::MoveForwardFailed;
		emit moveForwardFinished_motion(errorCode);
		return false;
	}

	//�ȴ��˶���λ
	//int motionTime = 0; //�˶��ۼ�ʱ��
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos = xMotionPos + dist) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //��ʱ
	//	motionTime += 500; //�����ۼ�ʱ��
	//	if (motionTime > 20000) { //��ʱ 20s
	//		errorCode = ErrorCode::MoveForwardFailed;
	//		emit moveForwardFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(3000);

	//ǰ������
	errorCode = ErrorCode::NoError;
	emit moveForwardFinished_motion(errorCode);
	return true;
}


/******************* ���� ******************/

//�˶��ṹ����
bool MotionControler::returnToZero()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//���ӿ�����
	if (AMC98_Connect(Q_NULLPTR, 0) != 0) { 
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false; 
	}
	pcb::delay(100);

	//���Ϳ���ָ��
	int zeroPos = 0;
	if (AMC98_Comand(1, NULL) != 0) {
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false;
	}

	//�ȴ��˶���λ
	//int motionTime = 0; //�˶��ۼ�ʱ��
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos == zeroPos) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //��ʱ
	//	motionTime += 500; //�����ۼ�ʱ��
	//	if (motionTime > 30000) { //��ʱ 30s
	//		errorCode = ErrorCode::ReturnToZeroFailed;
	//		emit returnToZeroFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(10000);

	//�������
	errorCode = ErrorCode::NoError;
	emit returnToZeroFinished_motion(errorCode);
	return true;
}

bool MotionControler::returnToZero2()
{
	//QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//���ӿ�����
	if (AMC98_Connect(Q_NULLPTR, 0) != 0) {
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false;
	}
	pcb::delay(100);

	//���Ϳ���ָ��
	int zeroPos = 0;
	if (AMC98_start_sr_move(2, 0, zeroPos, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::ReturnToZeroFailed;
		emit returnToZeroFinished_motion(errorCode);
		return false;
	}

	//�ȴ��˶���λ
	//int motionTime = 0; //�˶��ۼ�ʱ��
	//while (true) {
	//	this->getMotionData(0);
	//	if (motionInfo.xPos == zeroPos) {
	//		xMotionPos = motionInfo.xPos; break;
	//	}
	//	pcb::delay(500); //��ʱ
	//	motionTime += 500; //�����ۼ�ʱ��
	//	if (motionTime > 30000) { //��ʱ 30s
	//		errorCode = ErrorCode::ReturnToZeroFailed;
	//		emit returnToZeroFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(9000);

	//�������
	errorCode = ErrorCode::NoError;
	emit returnToZeroFinished_motion(errorCode);
	return true;
}



/******************* �˶���λ ******************/

//�˶��ṹ��λ
bool MotionControler::motionReset()
{
	QMutexLocker locker(&mutex);
	errorCode = ErrorCode::Unchecked;
	//Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) {
		errorCode = ErrorCode::MotionResetFailed;
		emit motionResetFinished_motion(errorCode);
		return false; 
	}
	pcb::delay(100);

	//���Ϳ���ָ��
	//if (AMC98_Comand(1, NULL) != 0) { return false; }

	//�ƶ������ʵ�λ�ã���ʱ���Է����µ�PCB��
	int resetPos = adminConfig->MaxMotionStroke;
	if (AMC98_start_sr_move(2, 0, resetPos, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MotionResetFailed;
		emit motionResetFinished_motion(errorCode);
		return false;
	}

	//�ȴ��˶���λ
	//int motionTime = 0; //�˶��ۼ�ʱ��
	//while (true) {
	//	this->getMotionData(4);
	//	xMotionPos = motionInfo.xPos;
	//	if (motionInfo.xPos == resetPos) break;
	//	pcb::delay(500); //��ʱ
	//	motionTime += 500; //�����ۼ�ʱ��
	//	if (motionTime > 30000) { //��ʱ 30s
	//		errorCode = ErrorCode::ResetControlerFailed;
	//		emit resetControlerFinished_motion(errorCode);
	//		return false;
	//	}
	//}

	pcb::delay(10000); //��λ�ȴ�

	//��λ����
	errorCode = ErrorCode::NoError;
	emit motionResetFinished_motion(errorCode);
	return true;
}


/******************* ���ÿ����� ******************/

bool MotionControler::resetControler()
{
	//���³�ʼ��
	if (!initControler()) return false;

	//�˶��ṹ����
	if (!returnToZero()) return false;

	return true;
}


/******************* ���� ******************/

//��������
bool MotionControler::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == MotionControler::NoError) return false;

	QString warningMessage;
	switch (tempCode)
	{
	case MotionControler::Unchecked:
		warningMessage = pcb::chinese("δȷ���˶��ṹ�Ĺ���״̬�� "); break;
	case MotionControler::InitFailed:
		warningMessage = pcb::chinese("�˶��ṹ��ʼ��ʧ�ܣ� "); break;
	case MotionControler::MoveForwardFailed:
		warningMessage = pcb::chinese("�˶�ǰ��ʧ�ܣ� "); break;
	case MotionControler::ReturnToZeroFailed:
		warningMessage = pcb::chinese("�˶�����ʧ�ܣ� "); break;
	case MotionControler::MoveToInitialPosFailed:
		warningMessage = pcb::chinese("�˶�����ʼ����λ��ʧ�ܣ� "); break;
	case MotionControler::MotionResetFailed:
		warningMessage = pcb::chinese("�˶���λʧ�ܣ� "); break;
	case MotionControler::ResetControlerFailed:
		warningMessage = pcb::chinese("�˶�����������ʧ�ܣ� "); break;
	case MotionControler::Default:
		warningMessage = pcb::chinese("δ֪���� "); break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		warningMessage + "    \n" +
		"MotionControler: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}
