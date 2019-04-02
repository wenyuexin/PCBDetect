#pragma once
#include <QObject>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
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
	Ui::DetectConfig *config;
	Ui::DetectParams *params;
	Ui::CvMatArray *samples;
	TemplFunc *templFunc;
	ExtractState extractState;

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();

	void setSampleImages(Ui::CvMatArray *ptr = Q_NULLPTR) { samples = ptr; }
	void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR) { params = ptr; }
	void setTemplFunc(TemplFunc *ptr = Q_NULLPTR) { templFunc = ptr; }

	void extract();
	
Q_SIGNALS:
	void extractState_extractor(int);

};
