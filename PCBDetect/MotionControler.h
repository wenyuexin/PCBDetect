#pragma once

#include "Configurator.h"
#include "RuntimeLibrary.h"
#include <QObject>
#include <QThread>
#include <QMutex> 
//#include "AMC98C.h"

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
		ResetControlerFailed = 0x404,
		Default
	};

	const int MaxRuntime = 5000; //��λms

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //�û�����
	int callerOfResetControler; //��λ�ĵ��ú����ı�ʶ
	bool running; //�����Ƿ���������
	ErrorCode errorCode; //�������Ĵ�����
	QMutex mutex; //�߳���

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } 
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void initControler(); //��ʼ��
	void moveForward(); //ǰ��
	void returnToZero(); //����
	void resetControler(int caller); //��λ
	bool isRunning(); //�жϵ�ǰ�Ƿ����������еĲ���

	inline bool isReady() { return errorCode == NoError; }
	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

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

