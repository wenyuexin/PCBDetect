#include "MotionControler.h"
//#include "WinNT.h"

//PortIndex ���������0,1,2,3,4,
typedef int (*Uart_AMC98_GetNewDataType)(int,unsigned char *,int);  //����ָ������
int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth);
int CallBack_UartNetGetNewData(int PortIndex, unsigned char *pGetData, int lth)
{
	//theApp.GetNewData(PortIndex);
	return 0;
}

MotionControler::MotionControler(QObject *parent)
	: QObject(parent)
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	detectConfig = Q_NULLPTR; //�û�����
	detectParams = Q_NULLPTR; //���в���
	errorCode = Uncheck; //�������Ĵ�����
	running = false; //�����Ƿ���������
}

MotionControler::~MotionControler()
{
}


/******************* �˶����� *********************/

//��ʼ�������ز���
bool MotionControler::initControler()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck; //�������Ĵ�����

	Uart_AMC98_GetNewData_v2(&CallBack_UartNetGetNewData);

	//0����������
	if (AMC98_KZQSet_v2(0, 0, "4") != 0) {
		markInitFailed(); return false;
	}
	//1����
	if (AMC98_KZQSet_v2(0, 1, "9600,N,8,1") != 0) {
		markInitFailed(); return false;
	}
	//2�˿�
	if (AMC98_KZQSet_v2(0, 2, "4") != 0) {
		markInitFailed(); return false;
	}
	//3Э��
	//str.Format(_T("%d"), xieyi_byte);
	if (AMC98_KZQSet_v2(0, 3, "2") != 0) {
		markInitFailed(); return false;
	}

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}

	//���ӿ����� - ��ʽ2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_initControler_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//��һ������˶��ṹ������г�
	int maxDist = adminConfig->MaxMotionStroke;
	if (maxDist > 500) {
		qDebug() << "invalid maxDist"; return false;
	}

	//���ÿ���ָ�� - X��
	if (!_AMC98_AddParamPC2CNC(138, maxDist * 10000)) return false; //�����ֵ
	if (!_AMC98_AddParamPC2CNC(145, 0 * 10000)) return false;//����Сֵ
	if (!_AMC98_AddParamPC2CNC(139, 10 * 10000)) return false;//�Ὺʼ�ٶ�
	if (!_AMC98_AddParamPC2CNC(140, 50 * 10000)) return false;//���������
	if (!_AMC98_AddParamPC2CNC(141, 10 * 10000)) return false;//��ԭ�����ٶ�
	if (!_AMC98_AddParamPC2CNC(142, 30 * 10000)) return false;//��ԭ����ٶ�
	if (!_AMC98_AddParamPC2CNC(143, 5 * 10000)) return false;//ԭ��λ��
	if (!_AMC98_AddParamPC2CNC(146, 85 * 10000)) return false;//1��λ������
	if (!_AMC98_AddParamPC2CNC(144, 2)) return false;//��ԭ������ 2������λ 1������λ
	if (!_AMC98_AddParamPC2CNC(147, 200)) return false;//����ʱ��
	if (!_AMC98_AddParamPC2CNC(148, 200)) return false;//����ʱ��
	if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//��������


	//���ÿ���ָ�� - Y��
	if (!_AMC98_AddParamPC2CNC(150, maxDist * 10000)) return false;//Y�����ֵ
	if (!_AMC98_AddParamPC2CNC(157, 0 * 10000)) return false;//����Сֵ
	if (!_AMC98_AddParamPC2CNC(151, 10 * 10000)) return false;//�Ὺʼ�ٶ�
	if (!_AMC98_AddParamPC2CNC(152, 50 * 10000)) return false;//���������
	if (!_AMC98_AddParamPC2CNC(153, 10 * 10000)) return false;//��ԭ�����ٶ�
	if (!_AMC98_AddParamPC2CNC(154, 30 * 10000)) return false;//��ԭ����ٶ�
	if (!_AMC98_AddParamPC2CNC(155, 5 * 10000)) return false;//ԭ��λ��
	if (!_AMC98_AddParamPC2CNC(158, 85 * 10000)) return false;//1��λ������
	if (!_AMC98_AddParamPC2CNC(156, 2)) return false;//��ԭ������ 2������λ 1������λ
	if (!_AMC98_AddParamPC2CNC(159, 200)) return false;//����ʱ��
	if (!_AMC98_AddParamPC2CNC(160, 200)) return false;//����ʱ��
	if (!_AMC98_AddParamPC2CNC(243, 1)) return false;//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	if (!_AMC98_AddParamPC2CNC(60000, 3)) return false;//��������
	if (AMC98_ParamPC2CNC() != 0) return false;//���������

	//��ʼ������
	errorCode = ErrorCode::NoError;
	running = false;
	emit initControlerFinished_motion();
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
	running = false;
	emit initControlerFinished_motion();
}


//�˶��ṹǰ��
bool MotionControler::moveForward()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}

	//���ӿ����� - ��ʽ2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_moveForward_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//���Ϳ���ָ��
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

	//ǰ������
	errorCode = ErrorCode::NoError;
	running = false;
	emit moveForwardFinished_motion();
	return true;
}


//�˶��ṹ����
bool MotionControler::returnToZero()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//���ӿ����� - ��ʽ1
	int counter = RETRY_NUM; //������
	while (counter > 0) {
		if (!AMC98_Connect(Q_NULLPTR, 0)) break;
		counter -= 1; //���¼�����
		if (counter == 0) { markInitFailed(); return false; }
	}

	//���ӿ����� - ��ʽ2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_returnToZero_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//���Ϳ���ָ��
	if (AMC98_start_sr_move(2, 0, 0, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MoveForwardFailed;
		running = false;
		emit returnToZeroFinished_motion();
		return false;
	}

	//�������
	errorCode = ErrorCode::NoError;
	running = false;
	emit returnToZeroFinished_motion();
	return true;
}

//�ƶ�����ʼ����λ��
//��λ�󼴿ɵ������ִ�е�һ��ͼ�������
bool MotionControler::moveToInitialPos()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}

	//���ӿ����� - ��ʽ2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_returnToZero_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//���Ϳ���ָ��
	if (AMC98_start_sr_move(2, 0, 245, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::MoveToInitialPosFailed;
		running = false;
		emit moveToInitialPosFinished_motion();
		return false;
	}

	pcb::delay(15000);

	//�ƶ�����
	errorCode = ErrorCode::NoError;
	running = false;
	emit moveToInitialPosFinished_motion();
	return true;
}

/**
 * ���ܣ��˶��ṹ��λ
 * ������caller �����ߵı�ʶ
 */
bool MotionControler::resetControler()
{
	QMutexLocker locker(&mutex);
	if (running) return false;
	running = true;
	errorCode = ErrorCode::Uncheck;

	//���ӿ����� - ��ʽ1
	if (AMC98_Connect(NULL, 0) != 0) { 
		markInitFailed(); return false; 
	}
	
	//���ӿ����� - ��ʽ2
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_resetControler_finished;
	//if (AMC98_Connect((HWND)(&funcPtr), 0) != 0) return false;

	//���Ϳ���ָ��
	//if (AMC98_Comand(1, NULL) != 0) { return false; }

	//�ƶ������ʵ�λ�ã���ʱ���Է����µ�PCB��
	if (AMC98_start_sr_move(2, 0, 450, WeizhiType_JD, 10, 100, 200, 200, 0, 0) != 0) {
		errorCode = ErrorCode::ResetControlerFailed;
		running = false;
		emit resetControlerFinished_motion();
		return false;
	}

	//��λ����
	errorCode = ErrorCode::NoError;
	running = false;
	emit resetControlerFinished_motion();
	return true;
}


/********* �жϲ���״̬�����ͽ����ź� - ��ʱû�� ***********/

//�жϵ�ǰ�Ƿ����������еĲ���
bool MotionControler::isRunning()
{ 
	QMutexLocker locker(&mutex);
	return running; 
}

//���ͳ�ʼ���������ź�
void MotionControler::on_initControler_finished()
{
	//QMutexLocker locker(&mutex);
	errorCode = NoError;
	running = false;
	emit initControlerFinished_motion();
}

//�����˶��ṹǰ���������ź�
void MotionControler::on_moveForward_finished()
{
	//QMutexLocker locker(&mutex);
	running = false;
	emit moveForwardFinished_motion();
}

//�����˶��ṹ����������ź�
void MotionControler::on_returnToZero_finished()
{
	//QMutexLocker locker(&mutex);
	running = false;
	emit returnToZeroFinished_motion();
}

//�����˶��ṹ��λ�������ź�
void MotionControler::on_resetControler_finished()
{
	//QMutexLocker locker(&mutex);
	running = false;
	emit resetControlerFinished_motion();
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
	case MotionControler::Uncheck:
		warningMessage = pcb::chinese("δȷ���˶��ṹ�Ĺ���״̬��"); break;
	case MotionControler::InitFailed:
		warningMessage = pcb::chinese("�˶��ṹ��ʼ��ʧ�ܣ�  "); break;
	case MotionControler::MoveForwardFailed:
		warningMessage = pcb::chinese("�˶��ṹǰ��ʧ�ܣ�    "); break;
	case MotionControler::ReturnToZeroFailed:
		warningMessage = pcb::chinese("�˶��ṹ����ʧ�ܣ�    "); break;
	case MotionControler::MoveToInitialPosFailed:
		warningMessage = pcb::chinese("�˶�����ʼ����λ��ʧ�ܣ�    "); break;
	case MotionControler::ResetControlerFailed:
		warningMessage = pcb::chinese("�˶��ṹ��λʧ�ܣ�    "); break;
	case MotionControler::Default:
		warningMessage = pcb::chinese("δ֪����"); break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		warningMessage + "    \n" +
		"MotionControler: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}
