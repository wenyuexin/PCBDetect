#include "ImgConvertThread.h"

using pcb::QImageVector;
using pcb::QPixmapVector;
using pcb::CvMatVector;


ImgConvertThread::ImgConvertThread(QObject *parent)
	: QThread(parent)
{

	//��Ա������ʼ��
	errorCode = ImageConverter::ErrorCode::Default;
	cvmatArray = Q_NULLPTR;
	qpixmapArray = Q_NULLPTR;
	qimageArray = Q_NULLPTR;
	cvtCode = ImageConverter::CvtCode::Null; //ת������
	currentRow = Q_NULLPTR; //��ǰ�к�
	semaphore = Q_NULLPTR;

	//��ʼ��ͼ��ת����
	initImageConverters();
}

ImgConvertThread::~ImgConvertThread()
{
	qDebug() << "~ImgConvertThread";
	delete semaphore; //ɾ���ź���
	semaphore = Q_NULLPTR;
	deleteImageConverters();
}


void ImgConvertThread::run()
{
	qDebug() << "==================== " << pcb::chinese("ת��ͼ������")
		<< "( currentRow_show =" << *currentRow << ")" << endl;

	clock_t t1 = clock();
	errorCode = ImageConverter::Unchecked;

	//��Ч�Լ���
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

	//��ʼת��
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
	qDebug() << "==================== " << pcb::chinese("ͼ������ת����")
		<< (t2 - t1) << "ms ( currentRow_show =" << *currentRow << ")" << endl;

	emit convertFinished_convertThread();
}


/************* ͼ��ת�����ĳ�ʼ����delete���� ***************/

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


/**************** ͼ���ʽת�� *****************/

//QImageתcv::Mat
void ImgConvertThread::convertQImagesToCvMats(const QImageVector &src, CvMatVector &dst)
{
	//����ת���߳�
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new cv::Mat; //�����ڴ�
		converters[i]->set(src[i], dst[i], ImageConverter::QImage2CvMat);
		converters[i]->start(); //��ʼת��
	}
	//�̵߳ȴ�
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}

//QPixmapתcv::Mat
void ImgConvertThread::convertQPixmapsToCvMats(const QPixmapVector &src, CvMatVector &dst)
{
	//����ת���߳�
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new cv::Mat; //�����ڴ�
		converters[i]->set(src[i], dst[i], ImageConverter::QPixmap2CvMat);//����ת����
		converters[i]->start(); //��ʼת��
	}
	//�̵߳ȴ�
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}

//cv::MatתQImage
void ImgConvertThread::convertCvMatsToQImages(const CvMatVector &src, QImageVector &dst)
{
	//����ת���߳�
	size_t srcSize = src.size();
	dst.resize(srcSize);
	for (int i = 0; i < srcSize; i++) {
		dst[i] = new QImage; //�����ڴ�
		converters[i]->set(src[i], dst[i], ImageConverter::CvMat2QImage);
		converters[i]->start(); //��ʼת��
	}
	//�̵߳ȴ�
	for (int i = 0; i < srcSize; i++) {
		converters[i]->wait();
	}
}

//cv::MatתQPixmap
void ImgConvertThread::convertCvMatsToQPixmaps(const CvMatVector &src, QPixmapVector &dst)
{
	//����ת���߳�
	dst.resize(src.size());
	for (int i = 0; i < src.size(); i++) {
		semaphore->acquire();
		dst[i] = new QPixmap; //�����ڴ�
		for (int j = 0; j < ConvertersNum; j++) {
			if (converters[j]->isRunning()) continue;
			converters[j]->set(src[i], dst[i], ImageConverter::CvMat2QPixmap);//����ת����
			converters[j]->start(); //��ʼת��
			break;
		}
	}

	//�̵߳ȴ�
	for (int i = 0; i < ConvertersNum; i++) {
		converters[i]->wait();
	}

	semaphore->acquire(ConvertersNum);
	semaphore->release(ConvertersNum);
}
