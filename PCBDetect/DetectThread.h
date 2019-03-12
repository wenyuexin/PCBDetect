#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImgConvertThread.h"
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
	std::vector<ImgConvertThread *> threads; //��ʽת���߳�
	int nThreads = 10; //Ĭ���������������10

public:
	DetectThread();
	~DetectThread();

	void setDetectCore(DetectCore *ptr = Q_NULLPTR);
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR);
	void setSampleImages(Ui::QImageArray *ptr = Q_NULLPTR);

private:
	void initImageConvertThreads();
	void deleteImageConvertThreads();
	void convertQImageToCvMat(Ui::QImageVector &src, Ui::CvMatVector &dst);

protected:
	void run();
};
