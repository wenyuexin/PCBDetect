#include "ExtractThread.h"


ExtractThread::ExtractThread(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	cvmatSamples = Q_NULLPTR; //������ȡ������ͼ
	templExtractor = Q_NULLPTR; //��ȡ��
}

ExtractThread::~ExtractThread()
{
	qDebug() << "~ExtractThread";
}


//��ʼ��ģ����ȡ��
void ExtractThread::initTemplateExtractor()
{
	templExtractor->setAdminConfig(adminConfig);
	templExtractor->setUserConfig(userConfig);
	templExtractor->setRuntimeParams(runtimeParams);
	templExtractor->setSampleImages(cvmatSamples);
	templExtractor->init();
}


//������ȡ�߳�
void ExtractThread::run()
{
	templExtractor->extract(); //��ȡ
}
