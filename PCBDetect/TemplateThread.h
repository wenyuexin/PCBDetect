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
	pcb::DetectConfig *config;
	pcb::DetectParams *params;
	pcb::QImageArray *qimages;
	pcb::CvMatArray cvmats;
	TemplFunc *templFunc = Q_NULLPTR;

	std::vector<ImageConverter *> threads; //��ʽת���߳�
	int nThreads = 10; //Ĭ���������������10


public:
	TemplateThread(QObject *parent = Q_NULLPTR);
	~TemplateThread();

	inline void setTemplateExtractor(TemplateExtractor *ptr = Q_NULLPTR) { templExtractor = ptr; }
	inline void setSampleImages(pcb::QImageArray *ptr = Q_NULLPTR) { qimages = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr = Q_NULLPTR) { params = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }

	void initTemplFunc();

protected:
	void run();


};
