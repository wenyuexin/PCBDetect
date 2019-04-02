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
	TemplateExtractor *templExtractor;
	Ui::DetectConfig *config;
	Ui::DetectParams *params;
	Ui::QImageArray *qimages;
	Ui::CvMatArray cvmats;
	TemplFunc *templFunc = Q_NULLPTR;

	std::vector<ImageConverter *> threads; //格式转换线程
	int nThreads = 10; //默认相机个数不超过10


public:
	TemplateThread(QObject *parent = Q_NULLPTR);
	~TemplateThread();

	inline void setTemplateExtractor(TemplateExtractor *ptr = Q_NULLPTR) { templExtractor = ptr; }
	inline void setSampleImages(Ui::QImageArray *ptr = Q_NULLPTR) { qimages = ptr; }
	inline void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR) { params = ptr; }
	inline void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }

	void initTemplFunc();

protected:
	void run();


};
