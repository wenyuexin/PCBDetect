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
		Finished
	};
	
private:
	pcb::AdminConfig *adminConfig;
	pcb::DetectConfig *detectConfig;
	pcb::DetectParams *detectParams;
	pcb::CvMatArray *samples;
	TemplFunc *templFunc;
	ExtractState extractState;

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();

	void setSampleImages(pcb::CvMatArray *ptr = Q_NULLPTR) { samples = ptr; }
	void setDetectConfig(pcb::AdminConfig *ptr = Q_NULLPTR) { adminConfig = ptr; }
	void setDetectConfig(pcb::DetectConfig *ptr = Q_NULLPTR) { detectConfig = ptr; }
	void setDetectParams(pcb::DetectParams *ptr = Q_NULLPTR) { detectParams = ptr; }
	void setTemplFunc(TemplFunc *ptr = Q_NULLPTR) { templFunc = ptr; }

	void extract();
	
Q_SIGNALS:
	void extractState_extractor(int);

};
