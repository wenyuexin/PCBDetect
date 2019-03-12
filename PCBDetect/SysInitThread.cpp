#include "SysInitThread.h"


SysInitThread::SysInitThread()
{
}

SysInitThread::~SysInitThread()
{
}


void SysInitThread::run()
{
	//��ȡ���������ļ�����config���г�ʼ��
	emit initializeStatus_initThread(QString::fromLocal8Bit("���ڻ�ȡ��ʷ�������� ..."));
	Sleep(1500);

	int ConfigCode = initDetectConfig();
	if (ConfigCode) { emit configError_initThread(ConfigCode); return; }

	emit initializeStatus_initThread(QString::fromLocal8Bit("��ʷ�������û�ȡ����  "));

	//������ʼ������
	Sleep(1200);
	
	//��ʼ������
	emit initializeFinished_initThread();
}


/****************** ��ʼ�� ********************/

//���ò����ṹ���ָ��
void SysInitThread::setDetectConfig(DetectConfig *ptr) { config = ptr; }


//��config���г�ʼ��
int SysInitThread::initDetectConfig()
{
	QString configFilePath = QDir::currentPath() + "/.config";
	QFile configFile(configFilePath);
	if (!configFile.open(QIODevice::ReadOnly)) { //�������ļ������ڣ�������Ĭ�ϵ������ļ�
		Configurator::init(configFilePath); 
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

		// ...
		configFile.close();

		//������Ч���ж�
		if (!(QFileInfo(config->OutputDirPath).isDir())) return -1;
		if (!(QFileInfo(config->TemplDirPath).isDir())) return -2;
		if (!(QFileInfo(config->SampleDirPath).isDir())) return -3;
	}

	config->imageSize = QSize(4384, 3288); //����ͼ��ĳߴ�
	return 0;
}
