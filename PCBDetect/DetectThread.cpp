#include "DetectThread.h"

using pcb::CvMatVector;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::DetectResult;
using pcb::QPixmapVector;
using pcb::QImageVector;
using pcb::QImageArray;


DetectThread::DetectThread()
{
	adminConfig = Q_NULLPTR; //系统参数
	detectConfig = Q_NULLPTR; //用户参数
	detectParams = Q_NULLPTR; //运行参数
	cvmatSamples = Q_NULLPTR; //用于提取的样本图
	defectDetecter = Q_NULLPTR; //提取器
}

DetectThread::~DetectThread()
{
}

//初始化缺陷检测器
void DetectThread::initDefectDetecter()
{
	defectDetecter->setAdminConfig(adminConfig);
	defectDetecter->setDetectConfig(detectConfig);
	defectDetecter->setDetectParams(detectParams);
	defectDetecter->setSampleImages(cvmatSamples);
	defectDetecter->setDetectResult(detectResult);
	defectDetecter->initDetectFunc();
}


//运行提取线程
void DetectThread::run()
{
	defectDetecter->detect(); //检测
}

