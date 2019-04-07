#include "SysInitThread.h"

using pcb::DetectConfig;
using pcb::AdminConfig;
using pcb::Configurator;


SysInitThread::SysInitThread()
{
	detectConfig = Q_NULLPTR; //�û���������
	adminConfig = Q_NULLPTR; //ϵͳ��������
	cameraControler = Q_NULLPTR; //���������
	bootStatus = 0x0000; //����״̬
}

SysInitThread::~SysInitThread()
{
}


/***************** �����߳� *****************/

void SysInitThread::run()
{
	if (bootStatus == 0x0000) {  //����ִ�г�ʼ��
		if (!initDetectConfig()) { bootStatus |= 0x1000;  return; }
		if (!initAdminConfig()) { bootStatus |= 0x0100;  return; }
		if (!initCameraControler()) { bootStatus |= 0x0001;  return; }
	}
	else if (!((bootStatus & 0xF000) >> 12)) { //detectConfig��ʼ���쳣
		if (!initDetectConfig()) { bootStatus |= 0x1000; return; }
		else                     { bootStatus &= 0x0FFF; }
	}
	else if (!((bootStatus & 0x0F00) >> 8)) { //adminConfig��ʼ���쳣
		if (!initAdminConfig()) { bootStatus |= 0x0100; return; }
		else                     { bootStatus &= 0xF0FF; }
	}
	else if (!(bootStatus & 0x000F) { //�����ʼ���쳣
		if (!initCameraControler()) { bootStatus |= 0x0001; return; }
		else                        { bootStatus &= 0xFFF0; }
	}
	
	//��ʼ������
	emit sysInitFinished_initThread();
}


/****************** ��ʼ�� ********************/

//��DetectConfig���г�ʼ��
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڻ�ȡ�û����� ..."));
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", detectConfig)) {
		emit detectConfigError_initThread(); return false;
	}
	else {
		DetectConfig::ErrorCode code;
		//�����߱�
		code = detectConfig->calcImageAspectRatio();
		if (code != DetectConfig::ValidValue) {
			emit detectConfigError_initThread(); return false; 
		}
		//������Ч���ж�
		code = detectConfig->checkValidity(DetectConfig::Index_All);
		if (code != DetectConfig::ValidConfig) { 
			emit detectConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("�û�������ȡ����   "));
	pcb::delay(500);
	return true;
}

//��adminConfig���г�ʼ��
bool SysInitThread::initAdminConfig()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڻ�ȡϵͳ���� ..."));
	pcb::delay(1000);

	if (!Configurator::loadConfigFile("/.admin.config", adminConfig)) {
		emit adminConfigError_initThread(); return false;
	}
	else {
		//������Ч���ж�
		AdminConfig::ErrorCode code;//�������
		code = adminConfig->checkValidity(AdminConfig::Index_All);
		if (code != AdminConfig::ValidConfig) { 
			emit adminConfigError_initThread(); return false; 
		}
	}

	emit sysInitStatus_initThread(pcb::chinese("ϵͳ������ȡ����   "));
	pcb::delay(500);
	return true;
}

//��ʼ�����������
bool SysInitThread::initCameraControler()
{
	emit sysInitStatus_initThread(pcb::chinese("���ڳ�ʼ����� ..."));
	pcb::delay(1000);
	cameraControler->setOperation(CameraControler::InitCameras);
	cameraControler->start(); //�����߳�
	cameraControler->wait(); //�̵߳ȴ�
	if (cameraControler->getErrorCode() != CameraControler::NoError) { 
		emit cameraError_initThread(); return false; 
	}
	pcb::delay(600);
	emit sysInitStatus_initThread(pcb::chinese("�����ʼ������    "));
	return true;
}
