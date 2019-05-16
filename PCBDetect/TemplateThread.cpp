#include "TemplateThread.h"


TemplateThread::TemplateThread(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	cvmatSamples = Q_NULLPTR; //用于提取的样本图
	templExtractor = Q_NULLPTR; //提取器
}

TemplateThread::~TemplateThread()
{
	qDebug() << "~TemplateThread";
}


//初始化模板提取器
void TemplateThread::initTemplateExtractor()
{
	templExtractor->setAdminConfig(adminConfig);
	templExtractor->setUserConfig(userConfig);
	templExtractor->setRuntimeParams(runtimeParams);
	templExtractor->setSampleImages(cvmatSamples);
	templExtractor->initTemplFunc();
}


//运行提取线程
void TemplateThread::run()
{
	templExtractor->extract(); //提取
}
