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
#include <iostream>
//#include "windows.h"


namespace pcb 
{
#ifndef PCB_FUNCTIONS_CHINESE
#define PCB_FUNCTIONS_CHINESE
	inline QString chinese(const QByteArray &src) { return QString::fromLocal8Bit(src); }
#endif //PCB_FUNCTIONS_CHINESE



#ifndef CLASS_ADMIN_CONFIG
#define CLASS_ADMIN_CONFIG 
	//系统参数类
	class AdminConfig 
	{
	public:
		int MaxMotionStroke; //机械结构的最大运动行程 mm
		int MaxCameraNum; //可用相机总数
		int PixelsNumPerUnitLength; //单位长度的像素 pix/mm
		double ImageOverlappingRate; //分图重叠率
		int ImageSize_W; //分图宽度
		int ImageSize_H; //分图高度
		double ImageAspectRatio; //图像宽高比

		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_MaxMotionStroke,
			Index_MaxCameraNum,
			Index_PixelsNumPerUnitLength,
			Index_ImageOverlappingRate,
			Index_ImageSize_W,
			Index_ImageSize_H,
			Index_ImageAspectRatio
		};

		//错误代码
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Uncheck = 0x100,
			ConfigFileMissing = 0x101,
			Invalid_MaxMotionStroke = 0x102,
			Invalid_MaxCameraNum = 0x103,
			Invalid_PixelsNumPerUnitLength = 0x104,
			Invalid_ImageOverlappingRate = 0x105,
			Invalid_ImageSize_W = 0x106,
			Invalid_ImageSize_H = 0x107,
			Invalid_ImageAspectRatio = 0x108,
			Default = 0x1FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		AdminConfig();
		~AdminConfig();
		void loadDefaultValue(); //加载默认参数

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid(bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; }
		inline void resetErrorCode() { errorCode = Uncheck; }
		inline ErrorCode getErrorCode() { return errorCode; } //获取错误代码
		static ConfigIndex convertCodeToIndex(ErrorCode code); //错误代码转参数索引
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

		ErrorCode calcImageAspectRatio(); //计算宽高比
		ConfigIndex unequals(AdminConfig &other); //不等性判断
		int getSystemResetCode(AdminConfig &newConfig); //获取系统重置代码
		void copyTo(AdminConfig *dst); //拷贝参数
	};
#endif //CLASS_ADMIN_CONFIG


#ifndef CLASS_DETECT_CONFIG
#define CLASS_DETECT_CONFIG
	//用户参数类
	class DetectConfig
	{
	public:
		QString SampleDirPath;//样本文件存储路径
		QString TemplDirPath; //模板文件的存储路径
		QString OutputDirPath;//检测结果存储路径
		QString ImageFormat; //图像后缀
		int ActualProductSize_W;//产品实际宽度,单位mm
		int ActualProductSize_H;//产品实际高度,单位mm
		int nBasicUnitInRow; //每一行中的基本单元数
		int nBasicUnitInCol; //每一列中的基本单元数
		QString clusterComPort; //COM串口

		//参数索引
		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_SampleDirPath,
			Index_TemplDirPath,
			Index_OutputDirPath,
			Index_ImageFormat,
			Index_ActualProductSize_W,
			Index_ActualProductSize_H,
			Index_nBasicUnitInRow,
			Index_nBasicUnitInCol,
			Index_clusterComPort
		};

		//错误代码
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Uncheck = 0x200,
			ConfigFileMissing = 0x201,
			Invalid_SampleDirPath = 0x202,
			Invalid_TemplDirPath = 0x203,
			Invalid_OutputDirPath = 0x204,
			Invalid_ImageFormat = 0x205,
			Invalid_ActualProductSize_W = 0x206,
			Invalid_ActualProductSize_H = 0x207,
			Invalid_nBasicUnitInRow = 0x208,
			Invalid_nBasicUnitInCol = 0x209,
			Invalid_clusterComPort = 0x20A,
			Default = 0x2FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		DetectConfig();
		~DetectConfig();
		void loadDefaultValue(); //加载默认参数

		ErrorCode checkValidity(ConfigIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(AdminConfig *adminConfig = Q_NULLPTR, bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; }
		inline void resetErrorCode() { errorCode = Uncheck; }
		inline ErrorCode getErrorCode() { return errorCode; } //获取错误代码
		static ConfigIndex convertCodeToIndex(ErrorCode code); //错误代码转索引
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

		ConfigIndex unequals(DetectConfig &other); //不等性判断
		int getSystemResetCode(DetectConfig &newConfig); //获取系统重置代码
		void copyTo(DetectConfig *dst); //拷贝参数
	};
#endif //CLASS_DETECT_CONFIG



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

		bool jsonSetValue(const QString &key, QString &value, bool encode); //写QString
		bool jsonSetValue(const QString &key, int &value, bool encode); //写int
		bool jsonSetValue(const QString &key, double &value, bool encode); //写double
		bool jsonReadValue(const QString &key, QString &value, bool decode); //读QString
		bool jsonReadValue(const QString &key, int &value, bool decode); //读int
		bool jsonReadValue(const QString &key, double &value, bool decode); //读double

		static bool loadConfigFile(const QString &fileName, DetectConfig *config);
		static bool saveConfigFile(const QString &fileName, DetectConfig *config);
		static bool loadConfigFile(const QString &fileName, AdminConfig *config);
		static bool saveConfigFile(const QString &fileName, AdminConfig *config);

		//quint64 getDiskFreeSpace(QString driver);
		bool checkDir(QString dirpath);

	private:
		void updateKeys();
		QString encrypt(QString origin) const;
		QString encrypt(const char* origin) const;
		QString decrypt(QString origin) const;
		QString decrypt(const char* origin) const;
	};
#endif //CLASS_CONFIGURATOR
}
