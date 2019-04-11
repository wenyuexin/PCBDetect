#include "MotionControler.h"

MotionControler::MotionControler(QObject *parent)
	: QObject(parent)
{
	detectConfig = Q_NULLPTR;
	adminConfig = Q_NULLPTR;
	callerOfResetControler = 0; //��λ�ĵ��ú����ı�ʶ
	errorCode = Uncheck; //�������Ĵ�����
	running = false; //�����Ƿ���������
}

MotionControler::~MotionControler()
{
}


/******************* �˶����� *********************/

//��ʼ�������ز���
void MotionControler::initControler()
{
	QMutexLocker locker(&mutex);
	if (running) return;
	else running = true;

	////0����������
	//AMC98_KZQSet(0, 0, _T("4"));
	////1����
	//AMC98_KZQSet(0, 1, _T("9600,N,8,1"));
	////2�˿�
	//CString str;
	//str.Format(_T("%d"), 4);//COM1������1
	//AMC98_KZQSet(0, 2, str);
	////3Э��
	//str.Format(_T("%d"), xieyi_byte);
	//AMC98_KZQSet(0, 3, str);
	//
	////AMC98_Connect(NULL, 0);//������ǰ��
	//void (MotionControler::*funcPtr)();
	//funcPtr = &MotionControler::on_initControler_finished;
	//AMC98_Connect((HWND)(&funcPtr), 0);

	//AMC98_AddParamPC2CNC(138, (int)(500 * 10000));//�����ֵ
	//AMC98_AddParamPC2CNC(145, (int)(0 * 10000));//����Сֵ
	//AMC98_AddParamPC2CNC(139, (int)(10 * 10000));//�Ὺʼ�ٶ�
	//AMC98_AddParamPC2CNC(140, (int)(50 * 10000));//���������
	//AMC98_AddParamPC2CNC(141, (int)(10 * 10000));//��ԭ�����ٶ�
	//AMC98_AddParamPC2CNC(142, (int)(30 * 10000));//��ԭ����ٶ�
	//AMC98_AddParamPC2CNC(143, (int)(5 * 10000));//ԭ��λ��
	//AMC98_AddParamPC2CNC(146, (int)(66 * 10000));//1��λ������
	//AMC98_AddParamPC2CNC(144, 2);//��ԭ������ 2������λ 1������λ
	//AMC98_AddParamPC2CNC(147, 200);//����ʱ��
	//AMC98_AddParamPC2CNC(148, 200);//����ʱ��
	//AMC98_AddParamPC2CNC(243, 1);//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	//AMC98_AddParamPC2CNC(60000, 3);//��������

	//AMC98_AddParamPC2CNC(150, (int)(500 * 10000));//Y�����ֵ
	//AMC98_AddParamPC2CNC(157, (int)(0 * 10000));//����Сֵ
	//AMC98_AddParamPC2CNC(151, (int)(10 * 10000));//�Ὺʼ�ٶ�
	//AMC98_AddParamPC2CNC(152, (int)(50 * 10000));//���������
	//AMC98_AddParamPC2CNC(153, (int)(10 * 10000));//��ԭ�����ٶ�
	//AMC98_AddParamPC2CNC(154, (int)(30 * 10000));//��ԭ����ٶ�
	//AMC98_AddParamPC2CNC(155, (int)(5 * 10000));//ԭ��λ��
	//AMC98_AddParamPC2CNC(158, (int)(66 * 10000));//1��λ������
	//AMC98_AddParamPC2CNC(156, 2);//��ԭ������ 2������λ 1������λ
	//AMC98_AddParamPC2CNC(159, 200);//����ʱ��
	//AMC98_AddParamPC2CNC(160, 200);//����ʱ��
	//AMC98_AddParamPC2CNC(243, 1);//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	//AMC98_AddParamPC2CNC(60000, 3);//��������
	//AMC98_ParamPC2CNC();//���������

	//������ʱ����
	pcb::delay(1000);
	//running = false;
	//emit initControlerFinished_motion();

	on_initControler_finished();
}

//�˶��ṹǰ��
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

//�˶��ṹ����
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
 * ���ܣ��˶��ṹ��λ
 * ������caller �����ߵı�ʶ
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


/************ �жϲ���״̬�����ͽ����ź� *************/

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
	emit resetControlerFinished_motion(callerOfResetControler);
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
		warningMessage = pcb::chinese("�˶���������ʧ�ܣ�    "); break;
	case MotionControler::ResetControlerFailed:
		warningMessage = pcb::chinese("�˶�������λʧ�ܣ�    "); break;
	default:
		warningMessage = pcb::chinese("δ֪����"); break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		warningMessage + "    \n" +
		"MotionControler: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}
