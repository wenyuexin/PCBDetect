#pragma once
#include <QObject>
#include "Configurator.h"
//#include "AMC98C.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


namespace pcb {
	class ImageConverter;
}

//�˶�������
class MotionControler : public QObject
{
	Q_OBJECT

public:
	//�˶��ṹ����ز���
	enum Operation {
		NoOperation,
		InitCameras,
		TakePhoto
	};

	//�˶��ṹ�Ĵ������
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x300,
		InitFailed = 0x301,
		moveForwardFailed = 0x302,
		returnToZeroFailed = 0x303,
		resetControler = 0x304
	};

private:
	pcb::DetectConfig *detectConfig;
	pcb::AdminConfig *adminConfig;
	int callerOfResetControler; //��λ�ĵ��ú����ı�ʶ
	ErrorCode errorCode; //�������Ĵ�����
	Operation operation;//����ָ��

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } 
	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } 

	void initControler(); //��ʼ��
	void moveForward(); //ǰ��
	void returnToZero(); //����
	void resetControler(int caller); //��λ

	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	bool showMessageBox(QWidget *parent); //��������

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

