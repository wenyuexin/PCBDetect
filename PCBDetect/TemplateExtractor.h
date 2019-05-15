#pragma once
#include <QObject>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLib.h"
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
		Error
	};

	//错误代码
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x800,
		Default = 0x8FF
	};
	
private:
	pcb::AdminConfig *adminConfig;
	pcb::UserConfig *userConfig;
	pcb::RuntimeParams *runtimeParams;
	pcb::CvMatArray *cvmatSamples;
	ExtractFunc *templFunc;
	ExtractState extractState;

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }

	void initTemplFunc();
	void extract();
	
Q_SIGNALS:
	void extractState_extractor(int);

};
