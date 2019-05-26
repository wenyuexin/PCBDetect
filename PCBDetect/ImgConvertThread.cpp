#include "ImgConvertThread.h"

using pcb::QImageVector;
using pcb::QPixmapVector;
using pcb::CvMatVector;


ImgConvertThread::ImgConvertThread(QObject *parent)
	: QThread(parent)
{

	//成员变量初始化
	errorCode = ImageConverter::ErrorCode::Default;
	cvmats = Q_NULLPTR;
	qpixmaps = Q_NULLPTR;
	qimages = Q_NULLPTR;
	cvtCode = ImageConverter::CvtCode::Null; //转换代码
	currentRow = Q_NULLPTR; //当前行号

	//初始化图像转换器
	initImageConverters();
}

ImgConvertThread::~ImgConvertThread()
{
	qDebug() << "~ImgConvertThread";
	deleteImageConverters();
}


void ImgConvertThread::run()
{
	qDebug() << "==================== " << pcb::chinese("转换图像类型：")
		<< "( currentRow_show =" << *currentRow << ")" << endl;

	clock_t t1 = clock();
	errorCode = ImageConverter::Unchecked;

	if (*currentRow < 0) { 
		qDebug() << "Warning: ImgConvertThread: currentRow < 0"; return; 
	}
	if (cvmats->size() < 1 || (cvmats->at(*currentRow)).size() < 1) { 
		qDebug() << "warning: invalid size of cvmats"; 
		errorCode = ImageConverter::ErrorCode::Invalid_ImageNum;
		return; 
	}
	if ((cvmats->at(*currentRow))[0] == Q_NULLPTR || 
		(cvmats->at(*currentRow))[0]->size().width < 1) 
	{
		qDebug() << "warning: invalid imageSize"; 
		errorCode = ImageConverter::ErrorCode::Invalid_ImageSize;
		return;
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
	qDebug() << "==================== " << pcb::chinese("图像类型转换结束：")
		<< (t2 - t1) << "( currentRow_show =" << *currentRow << ")" << endl;

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
