#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImgConvertThread.h"
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

	std::vector<ImgConvertThread *> threads; //��ʽת���߳�
	int nThreads = 10; //Ĭ���������������10

	TemplFunc *templFunc;

public:
	TemplateThread(QObject *parent = Q_NULLPTR);
	~TemplateThread();

	void setTemplateExtractor(TemplateExtractor *ptr = Q_NULLPTR);
	void setSampleImages(Ui::QImageArray *ptr = Q_NULLPTR);
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR);

	void setDetectConfig(Ui::DetectConfig *config);

private:
	void initImageConvertThreads();
	void deleteImageConvertThreads();
	void convertQImageToCvMat(Ui::QImageVector &src, Ui::CvMatVector &dst);

protected:
	void run();


};
