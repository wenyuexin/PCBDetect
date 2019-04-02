#include "TemplateThread.h"

using Ui::DetectConfig;
using Ui::QImageVector;
using Ui::CvMatVector;
using Ui::CvMatArray;
using Ui::QImageArray;
using Ui::DetectParams;
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
