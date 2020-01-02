#include "ImgConvertThread.h"

using pcb::QImageVector;
using pcb::QPixmapVector;
using pcb::CvMatVector;


ImgConvertThread::ImgConvertThread(QObject *parent)
	: QThread(parent)
{

	//成员变量初始化
	errorCode = ImageConverter::ErrorCode::Default;
	cvmatArray = Q_NULLPTR;
	qpixmapArray = Q_NULLPTR;
	qimageArray = Q_NULLPTR;
	cvtCode = ImageConverter::CvtCode::Null; //转换代码
	currentRow = Q_NULLPTR; //当前行号
	semaphore = Q_NULLPTR;

	//初始化图像转换器
	initImageConverters();
}

ImgConvertThread::~ImgConvertThread()
{
	qDebug() << "~ImgConvertThread";
	delete semaphore; //删除信号量
	semaphore = Q_NULLPTR;
	deleteImageConverters();
}


void ImgConvertThread::run()
{
	qDebug() << "==================== " << pcb::chinese("转换图像类型")
		<< "( currentRow_show =" << *currentRow << ")" << endl;

	clock_t t1 = clock();
	errorCode = ImageConverter::Unchecked;

	//有效性检验
	if (currentRow != Q_NULLPTR) {
		if (*currentRow < 0) {
			qDebug() << "ImgConvertThread: Warning: ImgConvertThread: currentRow < 0"; 
			return;
		}
		if (cvmatArray->size() < 1 || (cvmatArray->at(*currentRow)).size() < 1) {
			qDebug() << "ImgConvertThread: warning: invalid size of cvmatArray";
			errorCode = ImageConverter::ErrorCode::Invalid_ImageNum;
			return;
		}
		if ((cvmatArray->at(*currentRow))[0] == Q_NULLPTR ||
			(cvmatArray->at(*currentRow))[0]->size().width < 1)
		{
			qDebug() << "ImgConvertThread: warning: invalid imageSize";
			errorCode = ImageConverter::ErrorCode::Invalid_ImageSize;
			return;
		}
	}

	//开始转换
	switch (cvtCode)
	{
	case ImageConverter::QImage2CvMat:
		if (currentRow == Q_NULLPTR) convertQImagesToCvMats(*qimageVector, *cvmatVector);
		else convertQImagesToCvMats(qimageArray->at(*currentRow), cvmatArray->at(*currentRow)); 
		break;
	case ImageConverter::QPixmap2CvMat:
		if (currentRow == Q_NULLPTR) convertQPixmapsToCvMats(*qpixmapVector, *cvmatVector);
		else convertQPixmapsToCvMats(qpixmapArray->at(*currentRow), cvmatArray->at(*currentRow)); 	
		break;
	case ImageConverter::CvMat2QImage:
		if (currentRow == Q_NULLPTR) convertCvMatsToQImages(*cvmatVector, *qimageVector);
		else convertCvMatsToQImages(cvmatArray->at(*currentRow), qimageArray->at(*currentRow));
		break;
	case ImageConverter::CvMat2QPixmap:
		if (currentRow == Q_NULLPTR) convertCvMatsToQPixmaps(*cvmatVector, *qpixmapVector);
		else convertCvMatsToQPixmaps(cvmatArray->at(*currentRow), qpixmapArray->at(*currentRow));
		break;
	default:
		break;
	}

	clock_t t2 = clock();
	qDebug() << "==================== " << pcb::chinese("图像类型转换：")
		<< (t2 - t1) << "ms ( currentRow_show =" << *currentRow << ")" << endl;

	emit convertFinished_convertThread();
}


/************* 图像转换器的初始化、delete操作 ***************/

void ImgConvertThread::initImageConverters()
{
	semaphore = new QSemaphore(ConvertersNum);

	converters.resize(ConvertersNum);
	for (int i = 0; i < ConvertersNum; i++) {
		converters[i] = new ImageConverter();
		converters[i]->setSemaphore(semaphore);
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
void ImgConvertThread::convertQImagesToCvMats(const QImageVector &src, CvMatVector &dst)
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
void ImgConvertThread::convertQPixmapsToCvMats(const QPixmapVector &src, CvMatVector &dst)
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
void ImgConvertThread::convertCvMatsToQImages(const CvMatVector &src, QImageVector &dst)
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
void ImgConvertThread::convertCvMatsToQPixmaps(const CvMatVector &src, QPixmapVector &dst)
{
	//开启转换线程
	dst.resize(src.size());
	for (int i = 0; i < src.size(); i++) {
		semaphore->acquire();
		dst[i] = new QPixmap; //分配内存
		for (int j = 0; j < ConvertersNum; j++) {
			if (converters[j]->isRunning()) continue;
			converters[j]->set(src[i], dst[i], ImageConverter::CvMat2QPixmap);//配置转换器
			converters[j]->start(); //开始转换
			break;
		}
	}

	//线程等待
	for (int i = 0; i < ConvertersNum; i++) {
		converters[i]->wait();
	}

	semaphore->acquire(ConvertersNum);
	semaphore->release(ConvertersNum);
}
