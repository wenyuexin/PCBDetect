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
#include "pcbFuncLib.h"


//参数类与参数配置器
namespace pcb 
{
#ifndef CLASS_ADMIN_CONFIG
#define CLASS_ADMIN_CONFIG 
	//系统参数类
	class AdminConfig 
	{
	public:
		int MaxMotionStroke; //机械结构的最大运动行程 mm
		int PulseNumInUnitTime; //单位时间内的脉冲数

		int MaxCameraNum; //可用相机总数
		double PixelsNumPerUnitLength; //单位长度的像素数 pix/mm
		double ImageOverlappingRate_W; //分图重叠率(宽)
		double ImageOverlappingRate_H; //分图重叠率(高)
		int ImageSize_W; //分图宽度
		int ImageSize_H; //分图高度
		double ImageAspectRatio; //图像宽高比

		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_MaxMotionStroke,
			Index_PulseNumInUnitTime,
			Index_MaxCameraNum,
			Index_PixelsNumPerUnitLength,
			Index_ImageOverlappingRate_W,
			Index_ImageOverlappingRate_H,
			Index_ImageSize_W,
			Index_ImageSize_H,
			Index_ImageAspectRatio
		};

		//错误代码
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Unchecked = 0x100,
			ConfigFileMissing = 0x101,
			Invalid_MaxMotionStroke = 0x102,
			Invalid_PulseNumInUnitTime = 0x103,
			Invalid_MaxCameraNum = 0x104,
			Invalid_PixelsNumPerUnitLength = 0x105,
			Invalid_ImageOverlappingRate_W = 0x106,
			Invalid_ImageOverlappingRate_H = 0x107,
			Invalid_ImageSize_W = 0x108,
			Invalid_ImageSize_H = 0x109,
			Invalid_ImageAspectRatio = 0x10A,
			Default = 0x1FF
		};

	private:
		ErrorCode errorCode = Unchecked;
		const QSize MaxResolutionOfCamera = QSize(5472, 3648);


	public:
		AdminConfig();
		~AdminConfig();
		void loadDefaultValue(); //加载默认参数

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid(bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; }
		inline void resetErrorCode() { errorCode = Unchecked; }
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
	class UserConfig
	{
	public:
		//通用常数
		const int MaxDefectTypeNum = 4;

		//参数 - 基本设置
		QString TemplDirPath; //模板路径
		QString SampleDirPath;//样本路径
		QString OutputDirPath;//结果路径
		QString ImageFormat; //图像后缀

		int ActualProductSize_W;//产品实际宽度,单位mm
		int ActualProductSize_H;//产品实际高度,单位mm
		int nBasicUnitInRow; //每一行中的基本单元数
		int nBasicUnitInCol; //每一列中的基本单元数

		//参数 - 运动结构
		QString clusterComPort; //COM串口

		//参数 - 相机组
		int exposureTime; //曝光时间 ms （范围大概10-500）
		int colorMode; //色彩模式：0彩色 1黑白

		//参数 - 检测算法
		std::vector<bool> defectTypeToBeProcessed; //待处理的缺陷类型：0短路 1断路 2凸起 3缺失
		int matchingAccuracyLevel; //匹配精度等级：1高精度 2低精度
		int concaveRateThresh; //线路缺失率的阈值
		int convexRateThresh; //线路凸起率的阈值

		//参数 - 文件同步
		QString inetAddressOfRecheckPC; //复查设备的IP地址

		//参数索引
		enum ConfigIndex {
			Index_All,
			Index_None,
			//基本参数
			Index_TemplDirPath,
			Index_SampleDirPath,
			Index_OutputDirPath,
			Index_ImageFormat,
			Index_ActualProductSize_W,
			Index_ActualProductSize_H,
			Index_nBasicUnitInRow,
			Index_nBasicUnitInCol,
			//运动结构
			Index_clusterComPort,
			//相机
			Index_exposureTime,
			Index_colorMode,
			//检测算法
			Index_defectTypeToBeProcessed,
			Index_matchingAccuracyLevel,
			Index_concaveRateThresh,
			Index_convexRateThresh,
			//文件同步
			Index_inetAddressOfRecheckPC
		};

		//错误代码
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Unchecked = 0x200,
			ConfigFileMissing = 0x201,
			//基本参数
			Invalid_TemplDirPath = 0x202,
			Invalid_SampleDirPath = 0x203,
			Invalid_OutputDirPath = 0x204,
			Invalid_ImageFormat = 0x205,
			Invalid_ActualProductSize_W = 0x206,
			Invalid_ActualProductSize_H = 0x207,
			Invalid_nBasicUnitInRow = 0x208,
			Invalid_nBasicUnitInCol = 0x209,
			//运动结构
			Invalid_clusterComPort = 0x20A,
			//相机
			Invalid_exposureTime = 0x20B,
			Invalid_colorMode = 0x20C,
			//检测算法
			Invalid_defectTypeToBeProcessed = 0x20D,
			Invalid_matchingAccuracyLevel = 0x20E,
			Invalid_concaveRateThresh = 0x20F,
			Invalid_convexRateThresh = 0x210,
			//文件同步
			Invalid_inetAddressOfRecheckPC = 0x211,
			//其他
			Default = 0x2FF
		};

	private:
		ErrorCode errorCode = Unchecked;

	public:
		UserConfig();
		~UserConfig();
		void loadDefaultValue(); //加载默认参数

		ErrorCode checkValidity(ConfigIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(AdminConfig *adminConfig = Q_NULLPTR, bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; } //标记文件丢失
		inline void resetErrorCode() { errorCode = Unchecked; } //重置错误代码
		inline ErrorCode getErrorCode() { return errorCode; } //获取错误代码
		static ConfigIndex convertCodeToIndex(ErrorCode code); //错误代码转索引
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

		ConfigIndex unequals(UserConfig &other); //不等性判断
		void copyTo(UserConfig *dst); //拷贝参数

		int getSystemResetCode(UserConfig &newConfig); //获取系统重置代码
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
		ushort keys[4]; //秘钥

	public:
		Configurator(QFile *file = Q_NULLPTR);
		~Configurator();

		static void createConfigFile(QString filePath);

		
		bool jsonSetValue(const QString &key, QString &value, bool encode); //写QString
		bool jsonSetValue(const QString &key, int &value, bool encode); //写int
		bool jsonSetValue(const QString &key, double &value, bool encode); //写double
		bool jsonSetValue(const QString &key, std::vector<bool> &value, bool encode);

		bool jsonReadValue(const QString &key, QString &value, bool decode); //读QString
		bool jsonReadValue(const QString &key, int &value, bool decode); //读int
		bool jsonReadValue(const QString &key, double &value, bool decode); //读double
		bool jsonReadValue(const QString &key, std::vector<bool> &value, bool decode);

		static bool loadConfigFile(const QString &fileName, AdminConfig *config);
		static bool saveConfigFile(const QString &fileName, AdminConfig *config);
		static bool loadConfigFile(const QString &fileName, UserConfig *config);
		static bool saveConfigFile(const QString &fileName, UserConfig *config);

		//quint64 getDiskFreeSpace(QString driver);
		//bool checkDir(QString dirpath);

	private:
		void updateKeys(); //更新秘钥
		QString encrypt(QString origin) const; //加密
		QString encrypt(const char* origin) const; //加密
		QString decrypt(QString origin) const; //解密
		QString decrypt(const char* origin) const; //解密

		bool _jsonSetValue(const QString &key, QString &value, bool encode);
		bool _jsonReadValue(const QString &key, QString &value, bool decode);
	};
#endif //CLASS_CONFIGURATOR
}
