#pragma once

#include <QThread>
#include <QSemaphore>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ImageConverter.h"


//图像转换线程
//包括cv::Mat到QImage、QPixmap，以及后两种到cv::Mat的转换
//注意，除cv::Mat到QPixmap之外，其他转换不能同时处理超过ConvertersNum张图片
class ImgConvertThread : public QThread
{
	Q_OBJECT

private:
	ImageConverter::ErrorCode errorCode;

	pcb::CvMatArray *cvmatArray;
	pcb::QPixmapArray *qpixmapArray;
	pcb::QImageArray *qimageArray;
	int *currentRow; //当前行号

	pcb::CvMatVector *cvmatVector;
	pcb::QPixmapVector *qpixmapVector;
	pcb::QImageVector *qimageVector;

	const int ConvertersNum = 8; //转换线程的总数不超过设定值
	ImageConverter::CvtCode cvtCode; //转换代码
	QSemaphore *semaphore; //限制转换器的信号量
	std::vector<ImageConverter *> converters; //格式转换线程

public:
	ImgConvertThread(QObject *parent = Q_NULLPTR);
	~ImgConvertThread();

	inline void setQImages(pcb::QImageArray *ptr) { qimageArray = ptr; }
	inline void setQPixmaps(pcb::QPixmapArray *ptr) { qpixmapArray = ptr; }
	inline void setCvMats(pcb::CvMatArray *ptr) { cvmatArray = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row; }

	inline void setQImages(pcb::QImageVector *ptr) { qimageVector = ptr; }
	inline void setQPixmaps(pcb::QPixmapVector *ptr) { qpixmapVector = ptr; }
	inline void setCvMats(pcb::CvMatVector *ptr) { cvmatVector = ptr; }

	inline void setCvtCode(ImageConverter::CvtCode code) { cvtCode = code; }

private:
	void initImageConverters();
	void deleteImageConverters();

	void convertQImagesToCvMats(const pcb::QImageVector &src, pcb::CvMatVector &dst);
	void convertQPixmapsToCvMats(const pcb::QPixmapVector &src, pcb::CvMatVector &dst);
	void convertCvMatsToQImages(const pcb::CvMatVector &src, pcb::QImageVector &dst);
	void convertCvMatsToQPixmaps(const pcb::CvMatVector &src, pcb::QPixmapVector &dst);

protected:
	void run();

Q_SIGNALS:
	void convertFinished_convertThread();
};

