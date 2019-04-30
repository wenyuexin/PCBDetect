#include "TemplateThread.h"


TemplateThread::TemplateThread(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	cvmatSamples = Q_NULLPTR; //������ȡ������ͼ
	templExtractor = Q_NULLPTR; //��ȡ��
}

TemplateThread::~TemplateThread()
{
	qDebug() << "~TemplateThread";
}


//��ʼ��ģ����ȡ��
void TemplateThread::initTemplateExtractor()
{
	templExtractor->setAdminConfig(adminConfig);
	templExtractor->setUserConfig(userConfig);
	templExtractor->setRuntimeParams(runtimeParams);
	templExtractor->setSampleImages(cvmatSamples);
	templExtractor->initTemplFunc();
}


//������ȡ�߳�
void TemplateThread::run()
{
	templExtractor->extract(); //��ȡ
}
