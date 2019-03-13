#include "TemplateThread.h"

using Ui::DetectConfig;
using Ui::QImageVector;
using Ui::CvMatVector;
using Ui::CvMatArray;
using Ui::QImageArray;
using Ui::DetectParams;
using cv::Size;
using cv::Mat;


TemplateThread::TemplateThread(QObject *parent)
	: QThread(parent)
{
	initImageConvertThreads();
}

TemplateThread::~TemplateThread()
{
	delete templFunc;
	deleteImageConvertThreads();
}


/******************** ���� **********************/

void TemplateThread::setTemplateExtractor(TemplateExtractor *ptr) { templExtractor = ptr; }

void TemplateThread::setSampleImages(QImageArray *ptr) { qimages = ptr; }

void TemplateThread::setDetectParams(DetectParams *ptr) { params = ptr; }

void TemplateThread::setDetectConfig(DetectConfig *ptr) { config = ptr; }

void TemplateThread::initTemplFunc()
{
	templFunc = new TemplFunc;
	Mat temp = Mat(Size(params->imageSize.width()*config->nCamera, params->imageSize.height()*config->nPhotographing), CV_8UC3);
	templFunc->big_templ = temp;
}

/******************** ���� **********************/

void TemplateThread::run()
{
	//��ʼ��cvmats
	if (cvmats.size() == 0) {
		cvmats.resize(qimages->size());
		templExtractor->setSampleImages(&cvmats); //��������ͼ
	}

	//ת��
	QImageVector vec = (*qimages)[params->currentRow_extract];
	double t11 = clock();
	convertQImageToCvMat(vec, cvmats[params->currentRow_extract]); //QImageתMat
	double t22 = clock();
	qDebug() << "convert images :" << (t22 - t11) << "ms  ( currentRow_detect -" << params->currentRow_extract << ")";

	//��ȡ
	templExtractor->extract(templFunc); 
}


/**************** Qt��opencv��ͼ���ʽת�� *****************/

void TemplateThread::convertQImageToCvMat(QImageVector &src, CvMatVector &dst)
{
	//����ת���߳�
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		threads[i]->set(src[i], &(dst[i]), Ui::QImage2Mat);
		threads[i]->start();
	}
	//�̵߳ȴ�
	for (int i = 0; i < qimages->size(); i++) {
		threads[i]->wait();
	}
}

void TemplateThread::initImageConvertThreads()
{
	threads.resize(nThreads);
	for (int i = 0; i < nThreads; i++) {
		threads[i] = new ImgConvertThread();
	}
}

void TemplateThread::deleteImageConvertThreads()
{
	for (int i = 0; i < nThreads; i++) {
		delete threads[i];
		threads[i] = Q_NULLPTR;
	}
}