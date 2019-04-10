#pragma once
#include <QObject>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "TemplFunc.h"


//标准模板提取器
class TemplateExtractor : public QObject
{
	Q_OBJECT

public:
	//提取状态
	enum ExtractState {
		InitialState,
		Start,
		Finished,
		Default
	};
	
private:
	pcb::AdminConfig *adminConfig;
	pcb::DetectConfig *detectConfig;
	pcb::DetectParams *detectParams;
	pcb::CvMatArray *cvmatSamples;
	TemplFunc *templFunc;
	ExtractState extractState;

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }

	void initTemplFunc();
	void extract();
	
Q_SIGNALS:
	void extractState_extractor(int);

};
