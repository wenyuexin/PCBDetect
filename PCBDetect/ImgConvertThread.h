#pragma once

#include <QThread>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ImageConverter.h"


//图像转换线程
class ImgConvertThread : public QThread
{
	Q_OBJECT

private:
	ImageConverter::ErrorCode errorCode;

	pcb::CvMatArray *cvmats;
	pcb::QPixmapArray *qpixmaps;
	pcb::QImageArray *qimages;

	std::vector<ImageConverter *> converters; //格式转换线程
	const int ConvertersNum = 16; //转换线程的总数不超过设定值

	ImageConverter::CvtCode cvtCode; //转换代码
	int *currentRow; //当前行号

public:
	ImgConvertThread(QObject *parent = Q_NULLPTR);
	~ImgConvertThread();

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

