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


//�����������������
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
		double PixelsNumPerUnitLength; //��λ���ȵ������� pix/mm
		double ImageOverlappingRate_W; //��ͼ�ص���(��)
		double ImageOverlappingRate_H; //��ͼ�ص���(��)
		int ImageSize_W; //��ͼ���
		int ImageSize_H; //��ͼ�߶�
		double ImageAspectRatio; //ͼ���߱�

		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_MaxMotionStroke,
			Index_MaxCameraNum,
			Index_PixelsNumPerUnitLength,
			Index_ImageOverlappingRate_W,
			Index_ImageOverlappingRate_H,
			Index_ImageSize_W,
			Index_ImageSize_H,
			Index_ImageAspectRatio
		};

		//�������
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Unchecked = 0x100,
			ConfigFileMissing = 0x101,
			Invalid_MaxMotionStroke = 0x102,
			Invalid_MaxCameraNum = 0x103,
			Invalid_PixelsNumPerUnitLength = 0x104,
			Invalid_ImageOverlappingRate_W = 0x105,
			Invalid_ImageOverlappingRate_H = 0x106,
			Invalid_ImageSize_W = 0x107,
			Invalid_ImageSize_H = 0x108,
			Invalid_ImageAspectRatio = 0x109,
			Default = 0x1FF
		};

	private:
		ErrorCode errorCode = Unchecked;
		const QSize MaxResolutionOfCamera = QSize(4384, 3288);


	public:
		AdminConfig();
		~AdminConfig();
		void loadDefaultValue(); //����Ĭ�ϲ���

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid(bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; }
		inline void resetErrorCode() { errorCode = Unchecked; }
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
	class UserConfig
	{
	public:
		QString TemplDirPath; //ģ��·��
		QString SampleDirPath;//����·��
		QString OutputDirPath;//���·��
		QString ImageFormat; //ͼ���׺
		QString clusterComPort; //COM����

		int ActualProductSize_W;//��Ʒʵ�ʿ��,��λmm
		int ActualProductSize_H;//��Ʒʵ�ʸ߶�,��λmm
		int nBasicUnitInRow; //ÿһ���еĻ�����Ԫ��
		int nBasicUnitInCol; //ÿһ���еĻ�����Ԫ��

		int matchingAccuracyLevel; //ƥ�侫�ȵȼ���1�߾��� 2�;���
		int concaveRateThresh; //��·ȱʧ�ʵ���ֵ
		int convexRateThresh; //��·͹���ʵ���ֵ

		//��������
		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_TemplDirPath,
			Index_SampleDirPath,
			Index_OutputDirPath,
			Index_ImageFormat,
			Index_clusterComPort,
			Index_ActualProductSize_W,
			Index_ActualProductSize_H,
			Index_nBasicUnitInRow,
			Index_nBasicUnitInCol,
			Index_matchingAccuracyLevel,
			Index_concaveRateThresh,
			Index_convexRateThresh
		};

		//�������
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Unchecked = 0x200,
			ConfigFileMissing = 0x201,
			Invalid_TemplDirPath = 0x202,
			Invalid_SampleDirPath = 0x203,
			Invalid_OutputDirPath = 0x204,
			Invalid_ImageFormat = 0x205,
			Invalid_clusterComPort = 0x206,
			Invalid_ActualProductSize_W = 0x207,
			Invalid_ActualProductSize_H = 0x208,
			Invalid_nBasicUnitInRow = 0x209,
			Invalid_nBasicUnitInCol = 0x20A,
			Invalid_matchingAccuracyLevel = 0x20B,
			Invalid_concaveRateThresh = 0x20C,
			Invalid_convexRateThresh = 0x20D,
			Default = 0x2FF
		};

	private:
		ErrorCode errorCode = Unchecked;

	public:
		UserConfig();
		~UserConfig();
		void loadDefaultValue(); //����Ĭ�ϲ���

		ErrorCode checkValidity(ConfigIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(AdminConfig *adminConfig = Q_NULLPTR, bool doCheck = false);
		inline void markConfigFileMissing() { errorCode = ConfigFileMissing; } //����ļ���ʧ
		inline void resetErrorCode() { errorCode = Unchecked; } //���ô������
		inline ErrorCode getErrorCode() { return errorCode; } //��ȡ�������
		static ConfigIndex convertCodeToIndex(ErrorCode code); //�������ת����
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

		ConfigIndex unequals(UserConfig &other); //�������ж�
		int getSystemResetCode(UserConfig &newConfig); //��ȡϵͳ���ô���
		void copyTo(UserConfig *dst); //��������
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
		ushort keys[4]; //��Կ

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

		static bool loadConfigFile(const QString &fileName, AdminConfig *config);
		static bool saveConfigFile(const QString &fileName, AdminConfig *config);
		static bool loadConfigFile(const QString &fileName, UserConfig *config);
		static bool saveConfigFile(const QString &fileName, UserConfig *config);

		//quint64 getDiskFreeSpace(QString driver);
		bool checkDir(QString dirpath);

	private:
		void updateKeys(); //������Կ
		QString encrypt(QString origin) const; //����
		QString encrypt(const char* origin) const; //����
		QString decrypt(QString origin) const; //����
		QString decrypt(const char* origin) const; //����
	};
#endif //CLASS_CONFIGURATOR
}
