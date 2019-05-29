#pragma once
#pragma comment(lib, "NETDLL.lib")
#pragma comment(lib, "MOTIONDLL.lib")

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
class MotionControler : public QThread
{
	Q_OBJECT

public:
	//�������ز���
	enum Operation {
		NoOperation,
		InitControler, //��ʼ��
		MoveForward, //�˶�ǰ��
		ReturnToZero, //�˶�����
		MoveToInitialPos, //�˶�����ʼλ��
		MotionReset, //�˶���λ
		ResetControler //���ÿ�����
	};

	//�˶��ṹ�Ĵ������
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x400,
		InitFailed = 0x401,
		MoveForwardFailed = 0x402,
		ReturnToZeroFailed = 0x403,
		MoveToInitialPosFailed = 0x404,
		MotionResetFailed = 0x405,
		ResetControlerFailed = 0x406,
		Default = 0x4FF
	};

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���

	int xMotionPos; //X���λ��

	struct MotionInfo //�����Ϣ
	{
		int xPos;//X�ᵱǰλ��
		int yPos;//X�ᵱǰλ��
		int inputStatus;//����״̬
		int outputStauts;//���״̬ 
		int motorStatus;//���״̬
	} motionInfo;

	QMutex mutex; //�߳���
	
	ErrorCode errorCode; //�������Ĵ�����
	Operation operation; //����ָ��

public:
	MotionControler(QThread *parent = Q_NULLPTR);
	~MotionControler();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } 
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	bool initControler(); //��ʼ��
	bool moveForward(); //ǰ��
	bool returnToZero(); //����
	bool returnToZero2(); //����
	bool moveToInitialPos();//�ƶ�����ʼλ�ã���λ��������
	bool motionReset(); //�˶���λ
	bool resetControler(); //���ÿ�����


	inline void setOperation(Operation op) { operation = op; }
	inline bool isReady() { return (this != Q_NULLPTR) && (errorCode == NoError); }
	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

private:
	void markInitFailed();
	bool _AMC98_AddParamPC2CNC(int paramNum, int data);
	void getMotionData(int portIndex);

protected:
	void run();

Q_SIGNALS:
	void UartNetGetNewData_motion();

	void initControlerFinished_motion(int);
	void resetControlerFinished_motion(int);
	void moveToInitialPosFinished_motion(int);
	void returnToZeroFinished_motion(int);
	void moveForwardFinished_motion(int);
};

