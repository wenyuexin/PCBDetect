#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImageConverter.h"
#include "TemplateExtractor.h"
#include "TemplFunc.h"


class TemplateThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数

	pcb::CvMatArray *cvmatSamples; //用于提取的样本图
	TemplateExtractor *templExtractor; //提取器

public:
	TemplateThread(QObject *parent = Q_NULLPTR);
	~TemplateThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setTemplateExtractor(TemplateExtractor *ptr) { templExtractor = ptr; }
	void initTemplateExtractor();

protected:
	void run();
};
