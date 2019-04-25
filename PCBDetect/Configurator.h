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
	//ϵͳ������
	class AdminConfig 
	{
	public:
		int MaxMotionStroke; //��е�ṹ������˶��г� mm
		int MaxCameraNum; //�����������
		int PixelsNumPerUnitLength; //��λ���ȵ����� pix/mm
		double ImageOverlappingRate; //��ͼ�ص���
		int ImageSize_W; //��ͼ���
		int ImageSize_H; //��ͼ�߶�
		double ImageAspectRatio; //ͼ���߱�

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

		//�������
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
		void loadDefaultValue(); //����Ĭ�ϲ���

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid(bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; }
		inline void resetErrorCode() { errorCode = Uncheck; }
		inline ErrorCode getErrorCode() { return errorCode; } //��ȡ�������
		static ConfigIndex convertCodeToIndex(ErrorCode code); //�������ת��������
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

		ErrorCode calcImageAspectRatio(); //�����߱�
		ConfigIndex unequals(AdminConfig &other); //�������ж�
		int getSystemResetCode(AdminConfig &newConfig); //��ȡϵͳ���ô���
		void copyTo(AdminConfig *dst); //��������
	};
#endif //CLASS_ADMIN_CONFIG


#ifndef CLASS_DETECT_CONFIG
#define CLASS_DETECT_CONFIG
	//�û�������
	class DetectConfig
	{
	public:
		QString SampleDirPath;//�����ļ��洢·��
		QString TemplDirPath; //ģ���ļ��Ĵ洢·��
		QString OutputDirPath;//������洢·��
		QString ImageFormat; //ͼ���׺
		int ActualProductSize_W;//��Ʒʵ�ʿ��,��λmm
		int ActualProductSize_H;//��Ʒʵ�ʸ߶�,��λmm
		int nBasicUnitInRow; //ÿһ���еĻ�����Ԫ��
		int nBasicUnitInCol; //ÿһ���еĻ�����Ԫ��
		QString clusterComPort; //COM����

		//��������
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

		//�������
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
		void loadDefaultValue(); //����Ĭ�ϲ���

		ErrorCode checkValidity(ConfigIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(AdminConfig *adminConfig = Q_NULLPTR, bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; }
		inline void resetErrorCode() { errorCode = Uncheck; }
		inline ErrorCode getErrorCode() { return errorCode; } //��ȡ�������
		static ConfigIndex convertCodeToIndex(ErrorCode code); //�������ת����
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

		ConfigIndex unequals(DetectConfig &other); //�������ж�
		int getSystemResetCode(DetectConfig &newConfig); //��ȡϵͳ���ô���
		void copyTo(DetectConfig *dst); //��������
	};
#endif //CLASS_DETECT_CONFIG



#ifndef CLASS_CONFIGURATOR
#define CLASS_CONFIGURATOR 
	//������������
	//������ɲ�����Ͳ��������ļ�֮��Ķ�д����
	class Configurator
	{
	private:
		QFile *configFile;
		QDateTime fileDateTime;
		int keys[4]; //��Կ

	public:
		Configurator(QFile *file = Q_NULLPTR);
		~Configurator();

		static void createConfigFile(QString filePath);

		bool jsonSetValue(const QString &key, QString &value, bool encode); //дQString
		bool jsonSetValue(const QString &key, int &value, bool encode); //дint
		bool jsonSetValue(const QString &key, double &value, bool encode); //дdouble
		bool jsonReadValue(const QString &key, QString &value, bool decode); //��QString
		bool jsonReadValue(const QString &key, int &value, bool decode); //��int
		bool jsonReadValue(const QString &key, double &value, bool decode); //��double

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
