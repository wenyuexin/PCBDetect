#pragma once

#include "opencv2/opencv.hpp"
#include <QWidget>
#include <QString>
#include <QGraphicsItem>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFile>
#include <QTime>
#include <vector>


namespace pcb
{
#ifndef PCB_FUNCTIONS_CHINESE
#define PCB_FUNCTIONS_CHINESE
	inline QString chinese(const QByteArray &src) { return QString::fromLocal8Bit(src); }
#endif //PCB_FUNCTIONS_CHINESE

#ifndef PCB_FUNCTIONS
#define PCB_FUNCTIONS
	void delay(unsigned long msec);//非阻塞延迟
	QString selectDirPath(QWidget *parent, QString windowTitle = "");//交互式文件夹路径选择
	QString eraseNonDigitalCharInHeadAndTail(QString s); //删除字符串首尾的非数字字符
	void clearFiles(const QString &folderFullPath);
	void clearFolder(const QString &folderFullPath, bool included = false);

	QString boolVectorToString(const std::vector<bool> &vec);
	std::vector<bool> stringToBoolVector(const QString &str, int n = -1);
	bool isInetAddress(QString ip);
#endif //PCB_FUNCTIONS


#ifndef IMAGE_FORMAT
#define IMAGE_FORMAT
	enum ImageFormat { Unknown, BMP, JPG, PNG, TIF };
#endif //IMAGE_FORMAT

#ifndef TYPE_ITEM_GRID
#define TYPE_ITEM_GRID 
	typedef QList<QList<QPointF>> ItemGrid;
#endif //TYPE_ITEM_GRID

#ifndef TYPE_ITEM_ARRAY
#define TYPE_ITEM_ARRAY 
	typedef QVector<QVector<QGraphicsPixmapItem *>> ItemArray;
#endif //TYPE_ITEM_ARRAY

#ifndef TYPE_CV_MAT_CONTAINER
#define TYPE_CV_MAT_CONTAINER 
	typedef std::vector<cv::Mat *> CvMatVector;
	typedef std::vector<CvMatVector> CvMatArray;
#endif //TYPE_CV_MAT_CONTAINER

#ifndef TYPE_QIMAGE_CONTAINER
#define TYPE_QIMAGE_CONTAINER
	typedef std::vector<QImage *> QImageVector;
	typedef std::vector<QImageVector> QImageArray;
#endif //TYPE_QIMAGE_CONTAINER

#ifndef TYPE_QPIXMAP_CONTAINER
#define TYPE_QPIXMAP_CONTAINER 
	typedef std::vector<QPixmap *> QPixmapVector;
	typedef std::vector<QPixmapVector> QPixmapArray;
#endif //TYPE_QPIXMAP_CONTAINER
}

