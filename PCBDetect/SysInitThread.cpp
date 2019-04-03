#include "SysInitThread.h"

using Ui::DetectConfig;


SysInitThread::SysInitThread()
{
	bootStatus = 0x00; //����״̬
}

SysInitThread::~SysInitThread()
{
}


/***************** �����߳� *****************/

void SysInitThread::run()
{
	if (bootStatus == 0x00) {  //����ִ�г�ʼ��
		if (!initDetectConfig()) { bootStatus |= 0x10;  return; }
		if (!initCameraControler()) { bootStatus |= 0x01;  return; }
	}
	else if ((bootStatus & 0x10) >> 4 == 0x1) { //DetectConfig��ʼ���쳣
		if (!initDetectConfig()) { bootStatus |= 0x10; return; }
		else                     { bootStatus &= 0x01; }
	}
	else if ((bootStatus & 0x01) == 0x1) { //�����ʼ���쳣
		if (!initCameraControler()) { bootStatus |= 0x01; return; }
		else                        { bootStatus &= 0x10; }
	}
	
	//��ʼ������
	emit sysInitFinished_initThread();
}


/****************** ��ʼ�� ********************/

//��config���г�ʼ��
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("���ڻ�ȡ��ʷ�������� ..."));
	Ui::delay(1000);

	if (!Configurator::loadConfigFile("/.user.config", detectConfig)) {
		emit configError_initThread(DetectConfig::ConfigFileMissing); return false;
	}
	else {
		//�����߱�
		DetectConfig::ErrorCode code = detectConfig->calcImageAspectRatio();
		if (code != DetectConfig::ValidValue) {
			emit configError_initThread(code); return false; 
		}
		//������Ч���ж�
		code = detectConfig->checkValidity(DetectConfig::Index_All);
		if (code != DetectConfig::ValidConfig) { 
			emit configError_initThread(code); return false; 
		}
	}

	emit sysInitStatus_initThread(QString::fromLocal8Bit("��ʷ�������û�ȡ����   "));
	Ui::delay(500);
	return true;
}

//��ʼ�����������
bool SysInitThread::initCameraControler()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("���ڳ�ʼ����� ..."));
	Ui::delay(1000);
	cameraControler->setOperation(CameraControler::InitCameras);
	cameraControler->start(); //�����߳�
	cameraControler->wait(); //�̵߳ȴ�
	if (cameraControler->getErrorCode() != CameraControler::NoError) { 
		emit cameraError_initThread(); return false; 
	}
	Ui::delay(600);
	emit sysInitStatus_initThread(QString::fromLocal8Bit("�����ʼ������    "));
	return true;
}
