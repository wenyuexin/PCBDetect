#include "ExtractThread.h"


ExtractThread::ExtractThread(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	cvmatSamples = Q_NULLPTR; //用于提取的样本图
	templExtractor = Q_NULLPTR; //提取器
}

ExtractThread::~ExtractThread()
{
	qDebug() << "~ExtractThread";
}


//初始化模板提取器
void ExtractThread::initTemplateExtractor()
{
	templExtractor->setAdminConfig(adminConfig);
	templExtractor->setUserConfig(userConfig);
	templExtractor->setRuntimeParams(runtimeParams);
	templExtractor->setSampleImages(cvmatSamples);
	templExtractor->init();
}


//运行提取线程
void ExtractThread::run()
{
	templExtractor->extract(); //提取
}
