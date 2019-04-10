#include "TemplateThread.h"


TemplateThread::TemplateThread(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //系统参数
	detectConfig = Q_NULLPTR; //用户参数
	detectParams = Q_NULLPTR; //运行参数
	cvmatSamples = Q_NULLPTR; //用于提取的样本图
	templExtractor = Q_NULLPTR; //提取器
}

TemplateThread::~TemplateThread()
{
}


//初始化模板提取器
void TemplateThread::initTemplateExtractor()
{
	templExtractor->setAdminConfig(adminConfig);
	templExtractor->setDetectConfig(detectConfig);
	templExtractor->setDetectParams(detectParams);
	templExtractor->setSampleImages(cvmatSamples);
	templExtractor->initTemplFunc();
}


//运行提取线程
void TemplateThread::run()
{
	templExtractor->extract(); //提取
}
