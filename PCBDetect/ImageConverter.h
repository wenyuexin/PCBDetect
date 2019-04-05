#pragma once

#include "opencv2/opencv.hpp"
#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <qthread.h>


namespace pcb {
	class ImageConverter;
}

//图像格式转换线程
class ImageConverter : public QThread
{
	Q_OBJECT

public:
	enum CvtCode { QImage2CvMat, QPixmap2CvMat, CvMat2QImage, CvMat2QPixmap };

private:
	QImage *qimage;
	QPixmap *qpixmap;
	cv::Mat *cvmat;
	CvtCode code;

public:
	ImageConverter(QObject *parent = Q_NULLPTR);
	~ImageConverter();
	void set(QImage *src, cv::Mat *dst, CvtCode code);
	void set(QPixmap *src, cv::Mat *dst, CvtCode code);
	void set(cv::Mat *src, QImage *dst, CvtCode code);
	void set(cv::Mat *src, QPixmap *dst, CvtCode code);

protected:
	void run();

private:
	cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData);
	cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData);

	QImage CvMatToQImage(const cv::Mat &mat);
	QPixmap CvMatToQPixmap(const cv::Mat &inMat);
};
