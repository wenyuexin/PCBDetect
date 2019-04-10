#include "TemplateThread.h"


TemplateThread::TemplateThread(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	detectConfig = Q_NULLPTR; //�û�����
	detectParams = Q_NULLPTR; //���в���
	cvmatSamples = Q_NULLPTR; //������ȡ������ͼ
	templExtractor = Q_NULLPTR; //��ȡ��
}

TemplateThread::~TemplateThread()
{
}


//��ʼ��ģ����ȡ��
void TemplateThread::initTemplateExtractor()
{
	templExtractor->setAdminConfig(adminConfig);
	templExtractor->setDetectConfig(detectConfig);
	templExtractor->setDetectParams(detectParams);
	templExtractor->setSampleImages(cvmatSamples);
	templExtractor->initTemplFunc();
}


//������ȡ�߳�
void TemplateThread::run()
{
	templExtractor->extract(); //��ȡ
}
