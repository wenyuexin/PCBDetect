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


namespace pcb 
{
#ifndef PCB_FUNCTIONS
#define PCB_FUNCTIONS
	void delay(unsigned long msec);
#endif //PCB_FUNCTIONS

#ifndef IMAGE_FORMAT
#define IMAGE_FORMAT
	enum ImageFormat { BMP, JPG, PNG, TIFF };
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

#ifndef STRUCT_DETECT_RESULT
#define STRUCT_DETECT_RESULT 
	struct DetectResult { //检测结果

	};
#endif //STRUCT_DETECT_RESULT



#ifndef CLASS_DETECT_CONFIG
#define CLASS_DETECT_CONFIG
	//用户参数类
	class DetectConfig 
	{
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
			ValidValue = 0x000,
			Uncheck = 0x100,
			ConfigFileMissing = 0x101,
			Invalid_SampleDirPath = 0x102,
			Invalid_TemplDirPath = 0x103,
			Invalid_OutputDirPath = 0x104,
			Invalid_ImageFormat = 0x105,
			Invalid_nCamera = 0x106,
			Invalid_nPhotographing = 0x107,
			Invalid_nBasicUnitInRow = 0x108,
			Invalid_nBasicUnitInCol = 0x109,
			Invalid_ImageAspectRatio_W = 0x10A,
			Invalid_ImageAspectRatio_H = 0x10B,
			Invalid_ImageAspectRatio = 0x10C,
			Default = 0x1FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		DetectConfig() = default;
		~DetectConfig() = default;

		bool isValid();
		ErrorCode checkValidity(ConfigIndex index = Index_All);
		inline void resetErrorCode() { errorCode = Uncheck; }
		inline ErrorCode getErrorCode() { return errorCode; } //获取错误代码
		static ConfigIndex convertCodeToIndex(ErrorCode code); //错误代码转索引

		ErrorCode calcImageAspectRatio(); //计算宽高比
		int getSystemResetCode(DetectConfig &newConfig); //获取系统重置代码
		ConfigIndex unequals(DetectConfig &other); //不等性判断
		void copyTo(DetectConfig *dst); //拷贝参数
		void loadDefaultValue(); //加载默认参数
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告
	};
#endif //CLASS_DETECT_CONFIG


#ifndef CLASS_ADMIN_CONFIG
#define CLASS_ADMIN_CONFIG 
	//系统参数类
	class AdminConfig 
	{
	public:
		double MaxMotionStroke; //机械结构的最大运动行程
		int MaxCameraNum; //可用相机的总数
		int MaxPhotographingNum; //最大拍照次数

		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_MaxMotionStroke,
			Index_MaxCameraNum
		};

		//错误代码
		enum ErrorCode {
			ValidConfig = 0x000,
			Uncheck = 0x200,
			ConfigFileMissing = 0x201,
			Invalid_MaxMotionStroke = 0x202,
			Invalid_MaxCameraNum = 0x203,
			Default = 0x1FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		AdminConfig() = default;
		~AdminConfig() = default;

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid();
		inline ErrorCode getErrorCode() { return errorCode; }

		static ConfigIndex convertCodeToIndex(ErrorCode code);
		bool showMessageBox(QWidget *parent, ErrorCode code = Default);
	};
#endif //CLASS_ADMIN_CONFIG


#ifndef CLASS_CONFIGURATOR
#define CLASS_CONFIGURATOR 
	//参数配置器：
	//用于完成参数类和参数配置文件之间的读写操作
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

		static bool saveConfigFile(const QString &fileName, DetectConfig *config);
		static bool loadConfigFile(const QString &fileName, DetectConfig *config);
		static bool saveConfigFile(const QString &fileName, AdminConfig *config);
		static bool loadConfigFile(const QString &fileName, AdminConfig *config);

		quint64 getDiskFreeSpace(QString driver);
		bool checkDir(QString dirpath);

	private:
		void updateKeys();
		QString encrypt(QString origin) const;
		QString encrypt(const char* origin) const;
		QString decrypt(QString origin) const;
		QString decrypt(const char* origin) const;
	};
#endif //CLASS_CONFIGURATOR



#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//程序运行期间使用的临时变量或参数
	class DetectParams 
	{
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
}
