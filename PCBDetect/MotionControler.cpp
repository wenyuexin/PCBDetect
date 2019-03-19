#include "MotionControler.h"

MotionControler::MotionControler(QObject *parent)
	: QObject(parent)
{
}

MotionControler::~MotionControler()
{
}


/******************* �˶����� *********************/

//��ʼ�������ز���
void MotionControler::initControler()
{
	// TODO: Add your control notification handler code here

	CString str;
	//0����������
	AMC98_KZQSet(0, 0, _T("4"));
	//1����
	AMC98_KZQSet(0, 1, _T("9600,N,8,1"));
	//2�˿�
	str.Format(_T("%d"), 4);//COM1������1
	//str = 2;
	AMC98_KZQSet(0, 2, str);
	//3Э��
	str.Format(_T("%d"), xieyi_byte);
	AMC98_KZQSet(0, 3, str);
	
	//AMC98_Connect(NULL, 0);//������ǰ��
	void (MotionControler::*funcPtr)();
	funcPtr = &MotionControler::on_initControler_finished;
	AMC98_Connect((HWND)(&funcPtr), 0);

	AMC98_AddParamPC2CNC(138, (int)(500 * 10000));//�����ֵ
	AMC98_AddParamPC2CNC(145, (int)(0 * 10000));//����Сֵ
	AMC98_AddParamPC2CNC(139, (int)(10 * 10000));//�Ὺʼ�ٶ�
	AMC98_AddParamPC2CNC(140, (int)(50 * 10000));//���������
	AMC98_AddParamPC2CNC(141, (int)(10 * 10000));//��ԭ�����ٶ�
	AMC98_AddParamPC2CNC(142, (int)(30 * 10000));//��ԭ����ٶ�
	AMC98_AddParamPC2CNC(143, (int)(5 * 10000));//ԭ��λ��
	AMC98_AddParamPC2CNC(146, (int)(66 * 10000));//1��λ������
	AMC98_AddParamPC2CNC(144, 2);//��ԭ������ 2������λ 1������λ
	AMC98_AddParamPC2CNC(147, 200);//����ʱ��
	AMC98_AddParamPC2CNC(148, 200);//����ʱ��
	AMC98_AddParamPC2CNC(243, 1);//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	AMC98_AddParamPC2CNC(60000, 3);//��������

	AMC98_AddParamPC2CNC(150, (int)(500 * 10000));//Y�����ֵ
	AMC98_AddParamPC2CNC(157, (int)(0 * 10000));//����Сֵ
	AMC98_AddParamPC2CNC(151, (int)(10 * 10000));//�Ὺʼ�ٶ�
	AMC98_AddParamPC2CNC(152, (int)(50 * 10000));//���������
	AMC98_AddParamPC2CNC(153, (int)(10 * 10000));//��ԭ�����ٶ�
	AMC98_AddParamPC2CNC(154, (int)(30 * 10000));//��ԭ����ٶ�
	AMC98_AddParamPC2CNC(155, (int)(5 * 10000));//ԭ��λ��
	AMC98_AddParamPC2CNC(158, (int)(66 * 10000));//1��λ������
	AMC98_AddParamPC2CNC(156, 2);//��ԭ������ 2������λ 1������λ
	AMC98_AddParamPC2CNC(159, 200);//����ʱ��
	AMC98_AddParamPC2CNC(160, 200);//����ʱ��
	AMC98_AddParamPC2CNC(243, 1);//��Ҫ��λ�ĵ�� 1ҪX��λ 3ҪXY��λ 0��Ҫ��λ
	AMC98_AddParamPC2CNC(60000, 3);//��������
	AMC98_ParamPC2CNC();//���������
}

//�˶��ṹǰ��
void MotionControler::moveForward()
{
	void (MotionControler::*funcPtr)();
	funcPtr = &MotionControler::on_moveForward_finished;
	AMC98_Connect((HWND)(&funcPtr), 0);

	//AMC98_Connect(NULL, 0);
	AMC98_start_sr_move(2, 0, 80, WeizhiType_XD, 10, 50, 100, 100, 0, 0);
}

//�˶��ṹ����
void MotionControler::returnToZero()
{
	void (MotionControler::*funcPtr)();
	funcPtr = &MotionControler::on_returnToZero_finished;
	AMC98_Connect((HWND)(&funcPtr), 0);

	//AMC98_Connect(NULL, 0);
	AMC98_start_sr_move(2, 0, 0, WeizhiType_JD, 10, 100, 200, 200, 0, 0);
}

//�˶��ṹ��λ
void MotionControler::resetControler()
{
	void (MotionControler::*funcPtr)();
	funcPtr = &MotionControler::on_resetControler_finished;
	AMC98_Connect((HWND)(&funcPtr), 0);

	//AMC98_Connect(NULL, 0);
	AMC98_Comand(1, NULL);
}

/**************** ���ͽ����ź� *******************/

//��ʼ��
void MotionControler::on_initControler_finished()
{

}

//�˶��ṹǰ��
void MotionControler::on_moveForward_finished()
{
	emit signal_moveForward_finished();
}

//�˶��ṹ����
void MotionControler::on_returnToZero_finished()
{

}

//�˶��ṹ��λ
void MotionControler::on_resetControler_finished()
{

}