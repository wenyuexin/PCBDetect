#include "ImgConvertThread.h"

using pcb::QImageVector;
using pcb::QPixmapVector;
using pcb::CvMatVector;


ImgConvertThread::ImgConvertThread(QObject *parent)
	: QThread(parent)
{
	initImageConverters();
}

ImgConvertThread::~ImgConvertThread()
{
	deleteImageConverters();
}


void ImgConvertThread::run()
{
	qDebug() << "ImgConvertThread -> start ( currentRow -" << *currentRow << ")";
	clock_t t1 = clock();

	if (*currentRow < 0) { qDebug() << "Warning: ImgConvertThread: currentRow < 0"; return; }
	if (cvmats->size() < 1 || (cvmats->at(*currentRow)).size() < 1) { 
		qDebug() << "warning: invalid size of cvmats"; return; 
	}
	if ((cvmats->at(*currentRow))[0]->size().width < 1) {
		qDebug() << "warning: invalid imageSize"; return;
	}

	switch (cvtCode)
	{
	case ImageConverter::QImage2CvMat:
		convertQImageToCvMat(qimages->at(*currentRow), cvmats->at(*currentRow)); break;
	case ImageConverter::QPixmap2CvMat:
		convertQPixmapToCvMat(qpixmaps->at(*currentRow), cvmats->at(*currentRow)); break;
	case ImageConverter::CvMat2QImage:
		convertCvMatToQImage(cvmats->at(*currentRow), qimages->at(*currentRow)); break;
	case ImageConverter::CvMat2QPixmap:
		convertCvMatToQPixmap(cvmats->at(*currentRow), qpixmaps->at(*currentRow)); break;
	default:
		break;
	}

	clock_t t2 = clock();
	qDebug() << "ImgConvertThread: " << (t2-t1) << "ms ( currentRow -" << *currentRow << ")";

	emit convertFinished_convertThread();
}


/************* 图像转换器的初始化、delete操作 ***************/

void ImgConvertThread::initImageConverters()
{
	converters.resize(ConvertersNum);
	for (int i = 0; i < ConvertersNum; i++) {
		converters[i] = new ImageConverter();
	}
}

void ImgConvertThread::deleteImageConverters()
{
	for (int i = 0; i < converters.size(); i++) {
		delete converters[i];
		converters[i] = Q_NULLPTR;
	}
}


/**************** 图像格式转换 *****************/

//QImage转cv::Mat
void ImgConvertThread::convertQImageToCvMat(const QImageVector &src, CvMatVector &dst)
{
	//开启转换线程
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new cv::Mat; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::QImage2CvMat);
		converters[i]->start(); //开始转换
	}
	//线程等待
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}

//QPixmap转cv::Mat
void ImgConvertThread::convertQPixmapToCvMat(const QPixmapVector &src, CvMatVector &dst)
{
	//开启转换线程
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new cv::Mat; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::QPixmap2CvMat);//配置转换器
		converters[i]->start(); //开始转换
	}
	//线程等待
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}

//cv::Mat转QImage
void ImgConvertThread::convertCvMatToQImage(const CvMatVector &src, QImageVector &dst)
{
	//开启转换线程
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new QImage; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::CvMat2QImage);
		converters[i]->start(); //开始转换
	}
	//线程等待
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}

//cv::Mat转QPixmap
void ImgConvertThread::convertCvMatToQPixmap(const CvMatVector &src, QPixmapVector &dst)
{
	//开启转换线程
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new QPixmap; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::CvMat2QPixmap);//配置转换器
		converters[i]->start(); //开始转换
	}
	//线程等待
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}
