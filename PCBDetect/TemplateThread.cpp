#include "TemplateThread.h"

using pcb::DetectConfig;
using pcb::QImageVector;
using pcb::CvMatVector;
using pcb::CvMatArray;
using pcb::QImageArray;
using pcb::DetectParams;
using cv::Size;
using cv::Mat;


TemplateThread::TemplateThread(QObject *parent)
	: QThread(parent)
{
}

TemplateThread::~TemplateThread()
{
	delete templFunc;
}


/******************** 配置 **********************/

void TemplateThread::initTemplFunc()
{
	templFunc = new TemplFunc;
	templFunc->setDetectParams(params);
	templFunc->setDetectConfig(config);
	templFunc->generateBigTempl();

	templExtractor->setTemplFunc(templFunc);
}

/******************** 运行 **********************/

void TemplateThread::run()
{
	templExtractor->extract(); //提取
}
