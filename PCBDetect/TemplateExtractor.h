#pragma once
#include <QObject>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "TemplFunc.h"


namespace Ui {
#ifndef TYPE_CV_MAT_CONTAINER
#define TYPE_CV_MAT_CONTAINER 
	typedef std::vector<cv::Mat> CvMatVector;
	typedef std::vector<CvMatVector> CvMatArray;
#endif //TYPE_CV_MAT_CONTAINER
}

//标准模板提取器
class TemplateExtractor : public QObject
{
	Q_OBJECT

private:
	Ui::DetectConfig *config;
	Ui::DetectParams *params;
	Ui::CvMatArray *samples;
	TemplFunc *templFunc;
	int extractState;

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();

	void setSampleImages(Ui::CvMatArray *ptr = Q_NULLPTR) { samples = ptr; }
	void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR) { params = ptr; }
	void setTemplFunc(TemplFunc *ptr = Q_NULLPTR) { templFunc = ptr; }

	void extract();

Q_SIGNALS:
	void sig_extractState_extractor(int);

};
