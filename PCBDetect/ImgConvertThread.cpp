#include "ImgConvertThread.h"

using cv::Mat;
using Ui::CvtCode;


ImgConvertThread::ImgConvertThread(QObject *parent)
	: QThread(parent)
{
}

ImgConvertThread::~ImgConvertThread()
{
}


/**************** 配置线程 *****************/

void ImgConvertThread::set(QImage *src, Mat *dst, Ui::CvtCode code)
{
	this->qimage = src;
	this->cvmat = dst;
	this->code = code;
	this->qpixmap = Q_NULLPTR;
}

void ImgConvertThread::set(QPixmap *src, Mat *dst, Ui::CvtCode code)
{
	this->qpixmap = src;
	this->cvmat = dst;
	this->code = code;
	this->qimage = Q_NULLPTR;
}

/**************** 执行线程 *****************/

void ImgConvertThread::run()
{
	switch (code)
	{
	case Ui::QImage2Mat:
		*cvmat = QImageToCvMat(*qimage, true);
		break;
	case Ui::QPixmap2Mat:
		*cvmat = QPixmapToCvMat(*qpixmap, true);
		break;
	default:
		break;
	}
}


/*********** Qt至opencv的图像格式转换 *************/

Mat ImgConvertThread::QImageToCvMat(const QImage &inImage, bool inCloneImageData)
{
	switch (inImage.format())
	{
		// 8-bit, 4 channel
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC4,
			const_cast<uchar*>(inImage.bits()),
			static_cast<size_t>(inImage.bytesPerLine())
		);
		return (inCloneImageData ? mat.clone() : mat);
	}

	// 8-bit, 3 channel
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
	{
		if (!inCloneImageData) {
			qWarning() << "QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
		}

		QImage swapped = inImage;
		if (inImage.format() == QImage::Format_RGB32) {
			swapped = swapped.convertToFormat(QImage::Format_RGB888);
		}

		swapped = swapped.rgbSwapped();
		return cv::Mat(swapped.height(), swapped.width(),
			CV_8UC3,
			const_cast<uchar*>(swapped.bits()),
			static_cast<size_t>(swapped.bytesPerLine())
		).clone();
	}

	// 8-bit, 1 channel
	case QImage::Format_Indexed8:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC1,
			const_cast<uchar*>(inImage.bits()),
			static_cast<size_t>(inImage.bytesPerLine())
		);
		return (inCloneImageData ? mat.clone() : mat);
	}

	default:
		qWarning() << "QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
		break;
	}
	return cv::Mat();
}


Mat ImgConvertThread::QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData)
{
	return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}

