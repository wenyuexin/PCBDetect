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
	//Ui::DetectConfig *config;
	//Ui::DetectParams *params;

	Ui::CvMatArray *cvmats = Q_NULLPTR;
	Ui::QPixmapArray *qpixmaps = Q_NULLPTR;
	Ui::QImageArray *qimages = Q_NULLPTR;

	std::vector<ImageConverter *> converters; //格式转换线程
	int ConvertersNum = 15; //转换线程的总数不超过15
	ImageConverter::CvtCode cvtCode;
	int *currentRow; //当前行号

public:
	ImgConvertThread(QObject *parent = Q_NULLPTR);
	~ImgConvertThread();

	//inline void setDetectParams(Ui::DetectParams *ptr) { params = ptr; }
	//inline void setDetectConfig(Ui::DetectConfig *ptr) { config = ptr; }

	inline void setQImages(Ui::QImageArray *ptr) { qimages = ptr; }
	inline void setQPixmaps(Ui::QPixmapArray *ptr) { qpixmaps = ptr; }
	inline void setCvMats(Ui::CvMatArray *ptr) { cvmats = ptr; }
	inline void setCvtCode(ImageConverter::CvtCode code) { cvtCode = code; }
	inline void setCurrentRow(int *row) { currentRow = row; }

private:
	void initImageConverters();
	void deleteImageConverters();

	void convertQImageToCvMat(const Ui::QImageVector &src, Ui::CvMatVector &dst);
	void convertQPixmapToCvMat(const Ui::QPixmapVector &src, Ui::CvMatVector &dst);
	void convertCvMatToQImage(const Ui::CvMatVector &src, Ui::QImageVector &dst);
	void convertCvMatToQPixmap(const Ui::CvMatVector &src, Ui::QPixmapVector &dst);

protected:
	void run();

Q_SIGNALS:
	void convertFinished_convertThread();
};

