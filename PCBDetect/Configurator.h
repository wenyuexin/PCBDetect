#pragma once

#include "opencv2/opencv.hpp"
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


#ifndef STRUCT_ADMIN_CONFIG
#define STRUCT_ADMIN_CONFIG 
	struct AdminConfig {
		double MaxMotionStroke; //机械结构的最大运动行程
		int MaxCameraNum; //可用相机的总数
		int MaxPhotographingNum; //最大拍照次数
	};
#endif //STRUCT_ADMIN_CONFIG


#ifndef STRUCT_DETECT_CONFIG
#define STRUCT_DETECT_CONFIG
	//参数配置结构体
	class DetectConfig {
	private:
		int validFlag = 0;//1所有参数有效 -1检测到存在无效参数 0未检测或已检测的部分参数都有效

	public:
		QString SampleDirPath; //样本文件存储路径
		QString TemplDirPath;//模板文件的存储路径
		QString OutputDirPath;//检测结果存储路径
		QString ImageFormat; //图像后缀
		int nCamera; //相机个数
		int nPhotographing; //拍照次数
		int nBasicUnitInRow; //每一行中的基本单元数
		int nBasicUnitInCol; //每一列中的基本单元数
		double ImageAspectRatio; //样本图像的宽高比
		int ImageAspectRatio_W; //宽高比中的宽
		int ImageAspectRatio_H; //宽高比中的高

		//参数索引
		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_SampleDirPath,
			Index_TemplDirPath,
			Index_OutputDirPath,
			Index_ImageFormat,
			Index_nCamera,
			Index_nPhotographing,
			Index_nBasicUnitInRow,
			Index_nBasicUnitInCol,
			Index_ImageAspectRatio_W,
			Index_ImageAspectRatio_H,
			Index_ImageAspectRatio
		};

		//错误代码
		enum ErrorCode {
			ValidConfig = 0x000,
			ConfigFileMissing = 0x100,
			Invalid_SampleDirPath = 0x101,
			Invalid_TemplDirPath = 0x102,
			Invalid_OutputDirPath = 0x103,
			Invalid_ImageFormat = 0x104,
			Invalid_nCamera = 0x105,
			Invalid_nPhotographing = 0x106,
			Invalid_nBasicUnitInRow = 0x107,
			Invalid_nBasicUnitInCol = 0x108,
			Invalid_ImageAspectRatio_W = 0x109,
			Invalid_ImageAspectRatio_H = 0x10A,
			Invalid_ImageAspectRatio = 0x10B
		};

	public:
		DetectConfig() = default;
		~DetectConfig() = default;

		bool isValid();
		ErrorCode checkValidity(ConfigIndex index = Index_All);
		ErrorCode calcImageAspectRatio();
		bool getSystemResetFlag(DetectConfig &other);
		ConfigIndex unequals(DetectConfig &other);
		void copyTo(DetectConfig &other);
		void loadDefaultValue();
		static void showMessageBox(QWidget *parent, ErrorCode code);
		static ConfigIndex convertCodeToIndex(ErrorCode code);
	};
#endif //STRUCT_DETECT_CONFIG


#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//程序运行期间使用的临时变量或参数
	class DetectParams {
	public:
		QString sampleModelNum; //型号
		QString sampleBatchNum; //批次号
		QString sampleNum; //样本编号
		QSize imageSize; //样本图像的原始尺寸
		int currentRow_detect; //检测行号
		int currentRow_extract; //提取行号

	public:
		DetectParams() = default;
		~DetectParams() = default;

		void resetSerialNum();
		void loadDefaultValue();
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
	QDateTime fileDateTime;
	int keys[4]; //秘钥

public:
	Configurator(QFile *file = Q_NULLPTR);
	~Configurator();

	static void createConfigFile(QString filePath);

	bool jsonSetValue(const QString &key, QString &value); //写QString
	bool jsonSetValue(const QString &key, int &value); //写int
	bool jsonSetValue(const QString &key, double &value); //写double
	bool jsonReadValue(const QString &key, QString &value); //读QString
	bool jsonReadValue(const QString &key, int &value); //读int
	bool jsonReadValue(const QString &key, double &value); //读double

	static bool saveConfigFile(const QString &fileName, Ui::DetectConfig *config);
	static bool loadConfigFile(const QString &fileName, Ui::DetectConfig *config);

	quint64 getDiskFreeSpace(QString driver);
	bool checkDir(QString dirpath);

private:
	void updateKeys();
	QString encrypt(QString origin) const;
	QString encrypt(const char* origin) const;
	QString decrypt(QString origin) const;
	QString decrypt(const char* origin) const;
};
