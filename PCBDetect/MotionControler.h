#pragma once
#pragma comment(lib, "NETDLL.lib")
#pragma comment(lib,"MOTIONDLL.lib")
#include "AMC98C.h"
#include "MOTION_DLL.h"
#include "Configurator.h"
#include "RuntimeLib.h"
#include <QObject>
#include <QThread>
#include <QMutex> 

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


//�˶�������
class MotionControler : public QObject
{
	Q_OBJECT

public:
	//�˶��ṹ�Ĵ������
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x400,
		InitFailed = 0x401,
		MoveForwardFailed = 0x402,
		ReturnToZeroFailed = 0x403,
		MoveToInitialPosFailed = 0x404,
		ResetControlerFailed = 0x405,
		Default = 0x4FF
	};

	const int MaxRuntime = 5000; //��λms

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���

	bool running; //�����Ƿ���������
	QMutex mutex; //�߳���
	
	ErrorCode errorCode; //�������Ĵ�����

	const int RETRY_NUM = 1;

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } 
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	bool initControler(); //��ʼ��
	bool moveForward(); //ǰ��
	bool returnToZero(); //����
	bool moveToInitialPos();//�ƶ�����ʼλ�ã���λ��������
	bool resetControler(); //��λ
	bool isRunning(); //�жϵ�ǰ�Ƿ����������еĲ���

	inline bool isReady() { return errorCode == NoError; }
	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

private:
	void markInitFailed();
	bool _AMC98_AddParamPC2CNC(int paramNum, int data);
	void on_initControler_finished();
	void on_moveForward_finished();
	void on_returnToZero_finished();
	void on_resetControler_finished();

Q_SIGNALS:
	void initControlerFinished_motion();
	void moveForwardFinished_motion();
	void returnToZeroFinished_motion();
	void moveToInitialPosFinished_motion();
	void resetControlerFinished_motion();
};

