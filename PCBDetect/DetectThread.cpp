#include "DetectThread.h"

using pcb::CvMatVector;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::DetectResult;
using pcb::QPixmapVector;
using pcb::QImageVector;
using pcb::QImageArray;


DetectThread::DetectThread()
{
	initImageConvertThreads();
}

DetectThread::~DetectThread()
{
	deleteImageConvertThreads();
}


/***************** �����߳� ******************/

//��⵱ǰ��һ������
void DetectThread::run()
{
	QImageVector src = (*samples)[params->currentRow_detect];
	CvMatVector samples;

	double t11 = clock();
	convertQImageToCvMat(src, samples); //QImageתMat
	double t22 = clock();
	qDebug() << "convert images :" << (t22 - t11) << "ms  ( currentRow_detect -" << params->currentRow_detect << ")";

	detectCore->setSampleImages(&samples); //��������ͼ
	detectCore->doDetect(); //���
}


/**************** Qt��opencv��ͼ���ʽת�� *****************/

void DetectThread::convertQImageToCvMat(QImageVector &src, CvMatVector &dst)
{
	//����ת���߳�
	size_t vectorSize = src.size();
	dst.resize(vectorSize);
	for (int i = 0; i < vectorSize; i++) {
		threads[i]->set(src[i], dst[i], ImageConverter::QImage2CvMat);
		threads[i]->start();
	}
	//�̵߳ȴ�
	for (int i = 0; i < samples->size(); i++) {
		threads[i]->wait();
	}
}

void DetectThread::initImageConvertThreads()
{
	threads.resize(nThreads);
	for (int i = 0; i < nThreads; i++) {
		threads[i] = new ImageConverter();
	}
}

void DetectThread::deleteImageConvertThreads()
{
	for (int i = 0; i < threads.size(); i++) {
		delete threads[i];
		threads[i] = Q_NULLPTR;
	}
}
