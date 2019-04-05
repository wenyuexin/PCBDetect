#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImageConverter.h"

namespace Ui {
#ifndef TYPE_CV_MAT_CONTAINER
#define TYPE_CV_MAT_CONTAINER 
	typedef std::vector<cv::Mat *> CvMatVector;
	typedef std::vector<CvMatVector> CvMatArray;
#endif //TYPE_CV_MAT_CONTAINER
}


class ImgConvertThread : public QThread
{
	Q_OBJECT

private:
	//pcb::DetectConfig *config;
	//pcb::DetectParams *params;

	pcb::CvMatArray *cvmats = Q_NULLPTR;
	pcb::QPixmapArray *qpixmaps = Q_NULLPTR;
	pcb::QImageArray *qimages = Q_NULLPTR;

	std::vector<ImageConverter *> converters; //格式转换线程
	int ConvertersNum = 15; //转换线程的总数不超过15
	ImageConverter::CvtCode cvtCode;
	int *currentRow; //当前行号

public:
	ImgConvertThread(QObject *parent = Q_NULLPTR);
	~ImgConvertThread();

	//inline void setDetectParams(pcb::DetectParams *ptr) { params = ptr; }
	//inline void setDetectConfig(pcb::DetectConfig *ptr) { config = ptr; }

	inline void setQImages(pcb::QImageArray *ptr) { qimages = ptr; }
	inline void setQPixmaps(pcb::QPixmapArray *ptr) { qpixmaps = ptr; }
	inline void setCvMats(pcb::CvMatArray *ptr) { cvmats = ptr; }
	inline void setCvtCode(ImageConverter::CvtCode code) { cvtCode = code; }
	inline void setCurrentRow(int *row) { currentRow = row; }

private:
	void initImageConverters();
	void deleteImageConverters();

	void convertQImageToCvMat(const pcb::QImageVector &src, pcb::CvMatVector &dst);
	void convertQPixmapToCvMat(const pcb::QPixmapVector &src, pcb::CvMatVector &dst);
	void convertCvMatToQImage(const pcb::CvMatVector &src, pcb::QImageVector &dst);
	void convertCvMatToQPixmap(const pcb::CvMatVector &src, pcb::QPixmapVector &dst);

protected:
	void run();

Q_SIGNALS:
	void convertFinished_convertThread();
};

