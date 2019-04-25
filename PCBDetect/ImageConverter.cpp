#include "ImageConverter.h"

using cv::Mat;


ImageConverter::ImageConverter(QObject *parent)
	: QThread(parent)
{
}

ImageConverter::~ImageConverter()
{
	qDebug() << "~ImageConverter";
}


/**************** 配置线程 *****************/

void ImageConverter::set(QImage *src, Mat *dst, CvtCode code)
{
	this->qimage = src;
	this->cvmat = dst;
	this->code = code;
	this->qpixmap = Q_NULLPTR;
}

void ImageConverter::set(QPixmap *src, Mat *dst, CvtCode code)
{
	this->qpixmap = src;
	this->cvmat = dst;
	this->code = code;
	this->qimage = Q_NULLPTR;
}

void ImageConverter::set(cv::Mat *src, QImage *dst, CvtCode code) 
{
	this->cvmat = src;
	this->qimage = dst;
	this->code = code;
	this->qpixmap = Q_NULLPTR;
}

void ImageConverter::set(cv::Mat *src, QPixmap *dst, CvtCode code)
{
	this->cvmat = src;
	this->qpixmap = dst;
	this->code = code;
	this->qimage = Q_NULLPTR;
}


/**************** 运行线程 *****************/

void ImageConverter::run()
{
	switch (code)
	{
	case CvtCode::QImage2CvMat:
		*cvmat = QImageToCvMat(*qimage, true); break;
	case CvtCode::QPixmap2CvMat:
		*cvmat = QPixmapToCvMat(*qpixmap, true); break;
	case CvtCode::CvMat2QImage:
		*qimage = CvMatToQImage(*cvmat); break;
	case CvtCode::CvMat2QPixmap:
		*qpixmap = CvMatToQPixmap(*cvmat); break;
	default:
		break;
	}
}


/*********** Qt至opencv的图像格式转换 *************/

Mat ImageConverter::QImageToCvMat(const QImage &inImage, bool inCloneImageData)
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


Mat ImageConverter::QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData)
{
	return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}


/*********** opencv至Qt的图像格式转换 *************/

QImage ImageConverter::CvMatToQImage(const cv::Mat &mat)
{
	switch (mat.type())
	{
	// 8-bit, 4 channel
	case CV_8UC4:
	{
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
		return image;
	}

	// 8-bit, 3 channel
	case CV_8UC3:
	{
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}

	// 8-bit, 1 channel
	case CV_8UC1:
	{
		static QVector<QRgb>  sColorTable;
		// only create our color table once
		if (sColorTable.isEmpty()) {
			for (int i = 0; i < 256; ++i)
				sColorTable.push_back(qRgb(i, i, i));
		}
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
		image.setColorTable(sColorTable);
		return image;
	}

	default:
		qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
		break;
	}
	return QImage();
}


QPixmap ImageConverter::CvMatToQPixmap(const cv::Mat &inMat)
{
	return QPixmap::fromImage(CvMatToQImage(inMat));
}
