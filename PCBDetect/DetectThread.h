#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImageConverter.h"
#include "DetectCore.h"


class DetectThread :
	public QThread
{
	Q_OBJECT

private:
	DetectCore *detectCore; //������

	int *currentRow; //��ǰ���ڼ��������е��к�
	pcb::DetectParams *params;
	pcb::QImageArray *samples; //���ڼ���һ������
	std::vector<ImageConverter *> threads; //��ʽת���߳�
	int nThreads = 10; //Ĭ���������������10

public:
	DetectThread();
	~DetectThread();

	inline void setDetectCore(DetectCore* ptr = Q_NULLPTR) { detectCore = ptr; }
	inline void setSampleImages(pcb::QImageArray* ptr = Q_NULLPTR) { samples = ptr; }
	inline void setDetectParams(pcb::DetectParams* ptr = Q_NULLPTR) { params = ptr; }

private:
	void initImageConvertThreads();
	void deleteImageConvertThreads();
	void convertQImageToCvMat(pcb::QImageVector &src, pcb::CvMatVector &dst);

protected:
	void run();
};
