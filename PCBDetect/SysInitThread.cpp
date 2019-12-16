#include "SysInitThread.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::Configurator;


SysInitThread::SysInitThread()
{
	adminConfig = Q_NULLPTR; //ϵͳ��������
	userConfig = Q_NULLPTR; //�û���������
	runtimeParams = Q_NULLPTR; //�û���������
	motionControler = Q_NULLPTR; //�˶�������
	cameraControler = Q_NULLPTR; //���������
	bootStatus = 0x0000; //����״̬
}

SysInitThread::~SysInitThread()
{
	qDebug() << "~SysInitThread";
}


/***************** �����߳� *****************/

void SysInitThread::run()
{
	//����ִ�г�ʼ��
	if (bootStatus == 0x0000) {  
		//������ĳ�ʼ��
		if (!initAdminConfig()) { bootStatus |= 0x0100; return; }
		if (!initUserConfig()) { bootStatus |= 0x0100; return; }
		emit initGraphicsView_initThread(-1);
		qApp->processEvents();

		//�������в���
		if (!initRuntimeParams()) { bootStatus |= 0x1100; return; }
		emit initGraphicsView_initThread(0);
		qApp->processEvents();

		////��ʼ���˶��ṹ
		//if (!initMotionControler()) { bootStatus |= 0x0010; return; }

		//��ʼ�����
		if (!initCameraControler()) { bootStatus |= 0x0001; return; }
	}
	
	////ϵͳ����adminConfig��ʼ���쳣
	//if (!((bootStatus & 0xF000) >> 12)) { 
	//	if (!initAdminConfig()) { bootStatus |= 0x1000; return; }
	//	else { bootStatus &= 0x0FFF; }
	//}
	////�û�����userConfig��ʼ���쳣
	//if (!((bootStatus & 0x0F00) >> 8)) { 
	//	if (!initUserConfig()) { bootStatus |= 0x0100; return; }
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

//��UserConfig���г�ʼ��
bool SysInitThread::initUserConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڻ�ȡ�û����� ..."));
	qApp->processEvents();
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", userConfig)) {
		emit userConfigError_initThread(); return false;
	}
	else {
		UserConfig::ErrorCode code;//�������
		//������Ч���ж�
		code = userConfig->checkValidity(UserConfig::Index_All, adminConfig);
		if (code != UserConfig::ValidConfig) { 
			emit userConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("�û�������ȡ����   "));
	pcb::delay(800);
	return true;
}

//��RuntimeParams���г�ʼ��
bool SysInitThread::initRuntimeParams()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڸ������в��� ..."));
	qApp->processEvents();
	pcb::delay(1000);
	
	RuntimeParams::ErrorCode code;

	//���㵥��ǰ������
	code = runtimeParams->calcSingleMotionStroke(adminConfig);
	if (code != RuntimeParams::ValidValue) {
		emit runtimeParamsError_initThread(); return false;
	}
	//����������������մ���
	code = runtimeParams->calcItemGridSize(adminConfig, userConfig);
	if (code != RuntimeParams::ValidValue) {
		emit runtimeParamsError_initThread(); return false;
	}
	//�����ʼ����λ��
	code = runtimeParams->calcInitialPhotoPos(adminConfig);
	if (code != RuntimeParams::ValidValue) {
		emit runtimeParamsError_initThread(); return false;
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
	
	motionControler->setOperation(MotionControler::InitControler);
	motionControler->start(); //ִ�г�ʼ��
	motionControler->wait(); //�̵߳ȴ�
	if (!motionControler->isReady()) {
		emit motionError_initThread(MotionControler::InitFailed);
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
	if (!cameraControler->isReady()) { 
		emit cameraError_initThread(); return false; 
	}
	emit sysInitStatus_initThread(pcb::chinese("�����ʼ������    "));
	pcb::delay(600);
	return true;
}
