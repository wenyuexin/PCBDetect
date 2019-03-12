#pragma once

#include "opencv2/opencv.hpp"
#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QDebug>


namespace Ui {
	enum CvtCode { QImage2Mat, QPixmap2Mat };
}

//图像格式转换线程
class ImgConvertThread : public QThread
{
	Q_OBJECT

private:
	QImage *qimage;
	QPixmap *qpixmap;
	cv::Mat *cvmat;
	Ui::CvtCode code;

public:
	ImgConvertThread(QObject *parent = Q_NULLPTR);
	~ImgConvertThread();
	void set(QImage *src, cv::Mat *dst, Ui::CvtCode code);
	void set(QPixmap *src, cv::Mat *dst, Ui::CvtCode code);

protected:
	void run();

private:
	cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData);
	cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData);
};
