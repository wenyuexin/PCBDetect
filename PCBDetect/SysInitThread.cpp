#include "SysInitThread.h"

using Ui::DetectConfig;


SysInitThread::SysInitThread()
{
	bootStatus = 0; //����״̬
}

SysInitThread::~SysInitThread()
{
}


/***************** �����߳� *****************/

void SysInitThread::run()
{
	switch (bootStatus)
	{
	case 0: //����ִ�г�ʼ��
	case 1: //��ȡ���������ļ�����config���г�ʼ��
		if (!initDetectConfig()) { bootStatus = 1;  return; }
	case 2: //������ʼ������
		Ui::delay(1000);
	default:
		break;
	}
	
	//��ʼ������
	emit sysInitFinished_initThread();
}


/****************** ��ʼ�� ********************/

//��config���г�ʼ��
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("���ڻ�ȡ��ʷ�������� ..."));
	Ui::delay(1200);

	QString configFilePath = QDir::currentPath() + "/.config";
	QFile configFile(configFilePath);
	if (!configFile.open(QIODevice::ReadOnly)) { //�������ļ������ڣ�������Ĭ�ϵ������ļ�
		Configurator::init(configFilePath); 
		emit configError_initThread(DetectConfig::ConfigFileMissing); return false;
	}
	else { //�������ļ��ж�ȡ����
		Configurator configurator(&configFile);
		configurator.jsonReadValue("OutputDirPath", config->OutputDirPath);
		configurator.jsonReadValue("SampleDirPath", config->SampleDirPath);
		configurator.jsonReadValue("TemplDirPath", config->TemplDirPath);
		configurator.jsonReadValue("ImageFormat", config->ImageFormat);

		configurator.jsonReadValue("nCamera", config->nCamera);
		configurator.jsonReadValue("nPhotographing", config->nPhotographing);
		configurator.jsonReadValue("nBasicUnitInRow", config->nBasicUnitInRow);
		configurator.jsonReadValue("nBasicUnitInCol", config->nBasicUnitInCol);
		configurator.jsonReadValue("ImageAspectRatio_W", config->ImageAspectRatio_W);
		configurator.jsonReadValue("ImageAspectRatio_H", config->ImageAspectRatio_H);
		configFile.close();
		
		//�����߱�
		DetectConfig::ErrorCode code = config->calcImageAspectRatio();
		if (code != DetectConfig::ValidConfig) { emit configError_initThread(code); return false; }

		//������Ч���ж�
		code = config->checkValidity(DetectConfig::Index_All);
		if (code != DetectConfig::ValidConfig) { emit configError_initThread(code); return false; }
	}

	emit sysInitStatus_initThread(QString::fromLocal8Bit("��ʷ�������û�ȡ����  "));
	return true;
}
