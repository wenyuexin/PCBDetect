#pragma once

#include <QThread>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ImageConverter.h"


//ͼ��ת���߳�
class ImgConvertThread : public QThread
{
	Q_OBJECT

private:
	ImageConverter::ErrorCode errorCode;

	pcb::CvMatArray *cvmats;
	pcb::QPixmapArray *qpixmaps;
	pcb::QImageArray *qimages;

	std::vector<ImageConverter *> converters; //��ʽת���߳�
	const int ConvertersNum = 16; //ת���̵߳������������趨ֵ

	ImageConverter::CvtCode cvtCode; //ת������
	int *currentRow; //��ǰ�к�

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

