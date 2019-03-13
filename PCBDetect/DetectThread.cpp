#include "DetectThread.h"

using Ui::CvMatVector;
using Ui::DetectConfig;
using Ui::DetectParams;
using Ui::DetectResult;
using Ui::QPixmapVector;
using Ui::QImageVector;
using Ui::QImageArray;


DetectThread::DetectThread()
{
	initImageConvertThreads();
}

DetectThread::~DetectThread()
{
	deleteImageConvertThreads();
}


/***************** 启动线程 ******************/

//检测当前的一行样本
void DetectThread::run()
{
	QImageVector src = (*samples)[params->currentRow_detect];
	CvMatVector samples;

	double t11 = clock();
	convertQImageToCvMat(src, samples); //QImage转Mat
	double t22 = clock();
	qDebug() << "convert images :" << (t22 - t11) << "ms  ( currentRow_detect -" << params->currentRow_detect << ")";

	detectCore->setSampleImages(&samples); //配置样本图
	detectCore->doDetect(); //检测
}


/**************** Qt至opencv的图像格式转换 *****************/

void DetectThread::convertQImageToCvMat(QImageVector &src, CvMatVector &dst)
{
	//开启转换线程
	size_t vectorSize = src.size();
	dst.resize(vectorSize);
	for (int i = 0; i < vectorSize; i++) {
		threads[i]->set(src[i], &(dst[i]), Ui::QImage2Mat);
		threads[i]->start();
	}
	//线程等待
	for (int i = 0; i < samples->size(); i++) {
		threads[i]->wait();
	}
}

void DetectThread::initImageConvertThreads()
{
	threads.resize(nThreads);
	for (int i = 0; i < nThreads; i++) {
		threads[i] = new ImgConvertThread();
	}
}

void DetectThread::deleteImageConvertThreads()
{
	for (int i = 0; i < nThreads; i++) {
		delete threads[i];
		threads[i] = Q_NULLPTR;
	}
}
