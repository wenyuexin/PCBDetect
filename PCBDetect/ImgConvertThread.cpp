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
	currentRow = Q_NULLPTR; //当前行号
	cvtCode = ImageConverter::CvtCode::Null; //转换代码

	//线程池
	convertPool = Q_NULLPTR;
	convertPool = QThreadPool::globalInstance(); //获取全局的线程池实例
	convertPool->setMaxThreadCount(ConvertersNum); //设置最大可启动的线程数
	convertPool->setExpiryTimeout(-1); //没有超时等待

	converters.resize(0);
	initImageConverters(ConvertersNum); //设置转换器
}

ImgConvertThread::~ImgConvertThread()
{
	qDebug() << "~ImgConvertThread";
	deleteImageConverters();
}


void ImgConvertThread::run()
{
	qDebug() << "==================== " << pcb::chinese("转换图像类型") << endl;

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
		<< (t2 - t1) << "ms)" << endl;
	
	emit convertFinished_convertThread();
}


/************* 图像转换器的初始化、delete操作 ***************/

void ImgConvertThread::initImageConverters(int num)
{
	if (num <= converters.size() && converters.size() < 500) return;
	
	int from = converters.size();
	converters.resize(num);
	for (int i = from; i < num; i++) {
		converters[i] = new ImageConverter();
		converters[i]->setAutoDelete(false); //运行结束后不自动delete
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
	initImageConverters(src.size()); //转换器初始化

	//开启转换线程
	dst.resize(src.size());
	for (int i = 0; i < src.size(); i++) {
		dst[i] = new cv::Mat; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::QImage2CvMat);//配置转换器
		convertPool->start(converters[i]);
	}

	//等待所有图像转换结束
	convertPool->waitForDone();
}

//QPixmap转cv::Mat
void ImgConvertThread::convertQPixmapsToCvMats(const QPixmapVector &src, CvMatVector &dst)
{
	initImageConverters(src.size()); //转换器初始化

	//开启转换线程
	dst.resize(src.size());
	for (int i = 0; i < src.size(); i++) {
		dst[i] = new cv::Mat; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::QPixmap2CvMat);//配置转换器
		convertPool->start(converters[i]);
	}

	//等待所有图像转换结束
	convertPool->waitForDone();
}

//cv::Mat转QImage
void ImgConvertThread::convertCvMatsToQImages(const CvMatVector &src, QImageVector &dst)
{
	initImageConverters(src.size()); //转换器初始化

	//开启转换线程
	dst.resize(src.size());
	for (int i = 0; i < src.size(); i++) {
		dst[i] = new QImage; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::CvMat2QImage);//配置转换器
		convertPool->start(converters[i]);
	}

	//等待所有图像转换结束
	convertPool->waitForDone();
}

//cv::Mat转QPixmap
void ImgConvertThread::convertCvMatsToQPixmaps(const CvMatVector &src, QPixmapVector &dst)
{
	initImageConverters(src.size()); //转换器初始化

	//开启转换线程
	dst.resize(src.size());
	for (int i = 0; i < src.size(); i++) {
		dst[i] = new QPixmap; //分配内存
		converters[i]->set(src[i], dst[i], ImageConverter::CvMat2QPixmap);//配置转换器
		convertPool->start(converters[i]);
	}

	//等待所有图像转换结束
	convertPool->waitForDone();
}
