#include "DetectThread.h"

using pcb::CvMatVector;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::DetectResult;
using pcb::QPixmapVector;
using pcb::QImageVector;
using pcb::QImageArray;


DetectThread::DetectThread()
{
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	cvmatSamples = Q_NULLPTR; //用于提取的样本图
	defectDetecter = Q_NULLPTR; //提取器
}

DetectThread::~DetectThread()
{
	qDebug() << "~DetectThread";
}

//初始化缺陷检测器
void DetectThread::initDefectDetecter()
{
	defectDetecter->setAdminConfig(adminConfig);
	defectDetecter->setUserConfig(userConfig);
	defectDetecter->setRuntimeParams(runtimeParams);
	defectDetecter->setSampleImages(cvmatSamples);
	defectDetecter->setDetectResult(detectResult);
	defectDetecter->init();
}


//运行提取线程
void DetectThread::run()
{
	defectDetecter->detect(); //检测
}

