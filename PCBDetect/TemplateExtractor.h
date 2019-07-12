#pragma once
#include <QObject>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ExtractFunc.h"
#include <QStringList>


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

	double scalingFactor; //缩放因子
	cv::Size originalFullImageSize; //整图图像的原始尺寸
	cv::Size scaledFullImageSize; //经过缩放后的整图的尺寸
	cv::Size originalSubImageSize; //分图图像的原始尺寸
	cv::Size scaledSubImageSize; //经过缩放后的分图的尺寸

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }

	void extract();
	
private:
	void initExtractFunc();
	void makeCurrentTemplDir(std::vector<QString>&);

Q_SIGNALS:
	void extractState_extractor(int);

};
