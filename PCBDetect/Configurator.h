#pragma once

#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTextStream>
#include <QDebug>
#include <QTime>
#include <QJsonObject>
#include <QGraphicsItem>
#include <string>
#include <vector>
#include "windows.h"
#include <iostream>


namespace Ui {
	class Configurator;
	void delay(unsigned long msec);

#ifndef IMAGE_FORMAT
#define IMAGE_FORMAT
	enum ImageFormat { BMP, JPG, PNG };
#endif //IMAGE_FORMAT

#ifndef STRUCT_DETECT_CONFIG
#define STRUCT_DETECT_CONFIG 
	//参数配置结构体
	struct DetectConfig { 
		QString SampleDirPath; //样本文件存储路径
		QString TemplDirPath;//模板文件的存储路径
		QString OutputDirPath;//检测结果存储路径
		QString ImageFormat; //图像后缀
		QSize imageSize; //样本图像的尺寸
		int nCamera; //相机个数
		int nPhotographing; //拍照次数
		int nBasicUnitInRow; //每一行中的基本单元数
		int nBasicUnitInCol; //每一列中的基本单元数
	};
#endif //STRUCT_DETECT_CONFIG

#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//程序运行期间使用的临时变量或参数
	struct DetectParams {
		QString sampleModelNum; //型号
		QString sampleBatchNum; //批次号
		QString sampleNum; //样本编号
		int currentRow_detect; //检测行号
		int currentRow_extract; //提取行号
	};
#endif //STRUCT_DETECT_PARAMS

#ifndef TYPE_ITEM_GRID
#define TYPE_ITEM_GRID 
	typedef QList<QList<QPointF>> ItemGrid;
#endif //TYPE_ITEM_GRID

#ifndef TYPE_ITEM_ARRAY
#define TYPE_ITEM_ARRAY 
	typedef QVector<QVector<QGraphicsPixmapItem *>> ItemArray;
#endif //TYPE_ITEM_ARRAY

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

#ifndef STRUCT_DETECT_RESULT
#define STRUCT_DETECT_RESULT 
	struct DetectResult { //检测结果

	};
#endif //STRUCT_DETECT_RESULT
}


//参数配置文件的读写
class Configurator
{
private:
	QFile *configFile;

public:
	Configurator(QFile *file = Q_NULLPTR);
	~Configurator();

	static void init(QString filePath);
	void jsonSetValue(const QString &key, QString &value); //写
	bool jsonReadValue(const QString &key, QString &value); //读QString
	bool jsonReadValue(const QString &key, int &value); //读int
	bool jsonReadValue(const QString &key, double &value); //读double

	quint64 getDiskFreeSpace(QString driver);
	bool checkDir(QString dirpath);
};
