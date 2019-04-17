#include "SysInitThread.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::Configurator;


SysInitThread::SysInitThread()
{
	adminConfig = Q_NULLPTR; //ϵͳ��������
	detectConfig = Q_NULLPTR; //�û���������
	detectParams = Q_NULLPTR; //�û���������
	motionControler = Q_NULLPTR; //�˶�������
	cameraControler = Q_NULLPTR; //���������
	bootStatus = 0x0000; //����״̬
}

SysInitThread::~SysInitThread()
{
}


/***************** �����߳� *****************/

void SysInitThread::run()
{
	//����ִ�г�ʼ��
	if (bootStatus == 0x0000) {  
		//������ĳ�ʼ��
		if (!initAdminConfig()) { bootStatus |= 0x0100; return; }
		if (!initDetectConfig()) { bootStatus |= 0x0100; return; }
		emit initGraphicsView_initThread(-1);
		qApp->processEvents();

		//�������в���
		if (!initDetectParams()) { bootStatus |= 0x1100; return; }
		emit initGraphicsView_initThread(0);
		qApp->processEvents();

		//��ʼ���˶��ṹ
		if (!initMotionControler()) { bootStatus |= 0x0010; return; }

		//��ʼ�����
		if (!initCameraControler()) { bootStatus |= 0x0001; return; }
	}
	
	////ϵͳ����adminConfig��ʼ���쳣
	//if (!((bootStatus & 0xF000) >> 12)) { 
	//	if (!initAdminConfig()) { bootStatus |= 0x1000; return; }
	//	else { bootStatus &= 0x0FFF; }
	//}
	////�û�����detectConfig��ʼ���쳣
	//if (!((bootStatus & 0x0F00) >> 8)) { 
	//	if (!initDetectConfig()) { bootStatus |= 0x0100; return; }
	//	else                     { bootStatus &= 0xF0FF; }
	//}
	////�˶��ṹ��ʼ���쳣
	//if (!((bootStatus & 0x00F0) >> 4)) { 
	//	if (!initMotionControler()) { bootStatus |= 0x0010; return; }
	//	else                        { bootStatus &= 0xFF0F; }
	//}
	////�����ʼ���쳣
	//if (!(bootStatus & 0x000F)) { 
	//	if (!initCameraControler()) { bootStatus |= 0x0001; return; }
	//	else                        { bootStatus &= 0xFFF0; }
	//}
	
	//��ʼ������
	emit sysInitFinished_initThread();
}


/****************** ��ʼ�� ********************/

//��adminConfig���г�ʼ��
bool SysInitThread::initAdminConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڻ�ȡϵͳ���� ..."));
	qApp->processEvents();
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.admin.config", adminConfig)) {
		emit adminConfigError_initThread(); return false;
	}
	else {
		AdminConfig::ErrorCode code;//�������
		//�����߱�
		code = adminConfig->calcImageAspectRatio();
		if (code != AdminConfig::ValidValue) {
			emit adminConfigError_initThread(); return false; 
		}
		//������Ч���ж�
		code = adminConfig->checkValidity(AdminConfig::Index_All);
		if (code != AdminConfig::ValidConfig) { 
			emit adminConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("ϵͳ������ȡ����   "));
	pcb::delay(800);
	return true;
}

//��DetectConfig���г�ʼ��
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڻ�ȡ�û����� ..."));
	qApp->processEvents();
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", detectConfig)) {
		emit detectConfigError_initThread(); return false;
	}
	else {
		DetectConfig::ErrorCode code;//�������
		//������Ч���ж�
		code = detectConfig->checkValidity(DetectConfig::Index_All, adminConfig);
		if (code != DetectConfig::ValidConfig) { 
			emit detectConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("�û�������ȡ����   "));
	pcb::delay(800);
	return true;
}

//��DetectParams���г�ʼ��
bool SysInitThread::initDetectParams()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڸ������в��� ..."));
	qApp->processEvents();
	pcb::delay(1000);
	
	DetectParams::ErrorCode code, code2;
	code = detectParams->calcSingleMotionStroke(adminConfig);
	if (code != DetectParams::ValidValue) {
		emit detectParamsError_initThread(); return false;
	}
	pcb::delay(10);

	code2 = detectParams->calcItemGridSize(adminConfig, detectConfig);
	if (code2 != DetectParams::ValidValue) {
		emit detectParamsError_initThread(); return false;
	}

	emit sysInitStatus_initThread(pcb::chinese("���в������½���    "));
	pcb::delay(800);
	return true;
}

//��ʼ���˶�������
bool SysInitThread::initMotionControler()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڳ�ʼ���˶��ṹ ..."));
	pcb::delay(800);
	
	if (!motionControler->initControler()) {
		emit motionError_initThread(MotionControler::InitFailed);
		return false; //��ʼ��������
	}

	if (!motionControler->isReady()) {
		MotionControler::ErrorCode code;
		code = motionControler->getErrorCode();
		emit motionError_initThread(code);
		return false;
	}

	emit sysInitStatus_initThread(pcb::chinese("�˶��ṹ��ʼ������    "));
	pcb::delay(600);
	return true;
}

//��ʼ�����������
bool SysInitThread::initCameraControler()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڳ�ʼ����� ..."));
	pcb::delay(800);
	cameraControler->setOperation(CameraControler::InitCameras);
	cameraControler->start(); //�����߳�
	cameraControler->wait(); //�̵߳ȴ�
	if (cameraControler->getErrorCode() != CameraControler::NoError) { 
		emit cameraError_initThread(); return false; 
	}
	emit sysInitStatus_initThread(pcb::chinese("�����ʼ������    "));
	pcb::delay(600);
	return true;
}
