#pragma once

#include "opencv2/opencv.hpp"
#include <QThread>
#include <QRunnable>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <qthread.h>
#include <QSemaphore>
//#include <Exception>


//图像格式转换线程
class ImageConverter : public QThread
{
	Q_OBJECT

public:
	enum CvtCode { 
		QImage2CvMat, 
		QPixmap2CvMat, 
		CvMat2QImage, 
		CvMat2QPixmap,
		Null
	};

	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x600,
		Invalid_ImageNum = 0x601,
		Invalid_ImageSize = 0x602,
		Default = 0x6FF
	};

private:
	QImage *qimage;
	QPixmap *qpixmap;
	cv::Mat *cvmat;
	CvtCode code; //用于确定从哪个类型转换到哪个类型
	QSemaphore *semaphore; //控制转换数量的信号量

public:
	ImageConverter(QObject *parent = Q_NULLPTR);
	~ImageConverter();
	void set(QImage *src, cv::Mat *dst, CvtCode code);
	void set(QPixmap *src, cv::Mat *dst, CvtCode code);
	void set(cv::Mat *src, QImage *dst, CvtCode code);
	void set(cv::Mat *src, QPixmap *dst, CvtCode code);
	inline void setSemaphore(QSemaphore *sem) { semaphore = sem; }

protected:
	void run();

private:
	cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData);
	cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData);

	QImage CvMatToQImage(const cv::Mat &mat);
	QPixmap CvMatToQPixmap(const cv::Mat &inMat);
};
