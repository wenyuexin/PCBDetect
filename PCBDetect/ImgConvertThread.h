#pragma once

#include <QThread>
#include <QThreadPool>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ImageConverter.h"


//ͼ��ת���߳�
//����cv::Mat��QImage��QPixmap���Լ������ֵ�cv::Mat��ת��
class ImgConvertThread : public QThread
{
	Q_OBJECT

private:
	ImageConverter::ErrorCode errorCode;

	pcb::CvMatArray *cvmatArray;
	pcb::QPixmapArray *qpixmapArray;
	pcb::QImageArray *qimageArray;
	int *currentRow; //��ǰ�к�

	pcb::CvMatVector *cvmatVector;
	pcb::QPixmapVector *qpixmapVector;
	pcb::QImageVector *qimageVector;

	const int ConvertersNum = 8; //ת���̵߳������������趨ֵ
	ImageConverter::CvtCode cvtCode; //ת������
	std::vector<ImageConverter *> converters; //��ʽת���߳�
	QThreadPool *convertPool; //ͼ��ת�����̳߳�

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
	void initImageConverters(int);
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

