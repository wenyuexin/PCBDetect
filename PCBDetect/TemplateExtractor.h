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
	int extractState;

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();

	void setSampleImages(Ui::CvMatArray *ptr = Q_NULLPTR);
	void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR);
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR);

	void extract(TemplFunc *templFunc);

Q_SIGNALS:
	void sig_extractState_extractor(int);

};
