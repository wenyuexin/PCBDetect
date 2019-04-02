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
	Ui::DetectParams *params;
	Ui::QImageArray *samples; //���ڼ���һ������
	std::vector<ImageConverter *> threads; //��ʽת���߳�
	int nThreads = 10; //Ĭ���������������10

public:
	DetectThread();
	~DetectThread();

	inline void setDetectCore(DetectCore* ptr = Q_NULLPTR) { detectCore = ptr; }
	inline void setSampleImages(Ui::QImageArray* ptr = Q_NULLPTR) { samples = ptr; }
	inline void setDetectParams(Ui::DetectParams* ptr = Q_NULLPTR) { params = ptr; }

private:
	void initImageConvertThreads();
	void deleteImageConvertThreads();
	void convertQImageToCvMat(Ui::QImageVector &src, Ui::CvMatVector &dst);

protected:
	void run();
};
