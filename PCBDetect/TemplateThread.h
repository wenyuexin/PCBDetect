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
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数

	pcb::CvMatArray *cvmatSamples; //用于提取的样本图
	TemplateExtractor *templExtractor; //提取器

public:
	TemplateThread(QObject *parent = Q_NULLPTR);
	~TemplateThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setTemplateExtractor(TemplateExtractor *ptr) { templExtractor = ptr; }
	void initTemplateExtractor();

protected:
	void run();
};
