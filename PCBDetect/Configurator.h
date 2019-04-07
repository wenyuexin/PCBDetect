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
	void delay(unsigned long msec);//�������ӳ�
	inline QString chinese(const QByteArray &str) { return  QString::fromLocal8Bit(str); }
	QString selectDirPath(QString windowTitle = chinese("��ѡ��·��"));//����ʽ�ļ���·��ѡ��
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


#ifndef STRUCT_DETECT_RESULT
#define STRUCT_DETECT_RESULT 
	struct DetectResult { //�����

	};
#endif //STRUCT_DETECT_RESULT



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

		//��������
		enum ConfigIndex {
			Index_All,
			Index_None,
			Index_SampleDirPath,
			Index_TemplDirPath,
			Index_OutputDirPath,
			Index_ImageFormat,
			Index_ActualProductSize_W;
		    Index_ActualProductSize_H;
			Index_nBasicUnitInRow,
			Index_nBasicUnitInCol,
		};

		//�������
		enum ErrorCode {
			ValidConfig = 0x000,
			ValidValue = 0x000,
			Uncheck = 0x100,
			ConfigFileMissing = 0x101,
			Invalid_SampleDirPath = 0x102,
			Invalid_TemplDirPath = 0x103,
			Invalid_OutputDirPath = 0x104,
			Invalid_ImageFormat = 0x105,
			Index_ActualProductSize_W = 0x106,
		    Index_ActualProductSize_H = 0x107,
			Invalid_nBasicUnitInRow = 0x108,
			Invalid_nBasicUnitInCol = 0x109,
			Default = 0x1FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		DetectConfig() = default;
		~DetectConfig() = default;
		void loadDefaultValue(); //����Ĭ�ϲ���

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid();
		inline void resetErrorCode() { errorCode = Uncheck; }
		inline ErrorCode getErrorCode() { return errorCode; } //��ȡ�������
		static ConfigIndex convertCodeToIndex(ErrorCode code); //�������ת����
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

		ConfigIndex unequals(DetectConfig &other); //�������ж�
		int getSystemResetCode(DetectConfig &newConfig); //��ȡϵͳ���ô���
		void copyTo(DetectConfig *dst); //��������
	};
#endif //CLASS_DETECT_CONFIG


#ifndef CLASS_ADMIN_CONFIG
#define CLASS_ADMIN_CONFIG 
	//ϵͳ������
	class AdminConfig 
	{
	public:
		int MaxMotionStroke; //��е�ṹ������˶��г�
		int MaxCameraNum; //�������������
		int PixelsNumPerUnitLength; //ͼ��ֱ��� pix/mm
		double ImageOverlappingRate; //��ͼ�ص���
		int ImageSize_W; //ͼ����
		int ImageSize_H; //ͼ��߶�
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
			Uncheck = 0x200,
			ConfigFileMissing = 0x201,
			Invalid_MaxMotionStroke = 0x202,
			Invalid_MaxCameraNum = 0x203,
			Invalid_PixelsNumPerUnitLength = 0x204,
			Invalid_ImageOverlappingRate = 0x205,
			Invalid_ImageSize_W = 0x206,
			Invalid_ImageSize_H = 0x207,
			Invalid_ImageAspectRatio = 0x208,
			Default = 0x2FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		AdminConfig() = default;
		~AdminConfig() = default;
		void loadDefaultValue(); //����Ĭ�ϲ���

		ErrorCode checkValidity(ConfigIndex index = Index_All);
		bool isValid();
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

		bool jsonSetValue(const QString &key, QString &value); //дQString
		bool jsonSetValue(const QString &key, int &value); //дint
		bool jsonSetValue(const QString &key, double &value); //дdouble
		bool jsonReadValue(const QString &key, QString &value); //��QString
		bool jsonReadValue(const QString &key, int &value); //��int
		bool jsonReadValue(const QString &key, double &value); //��double

		static bool loadConfigFile(const QString &fileName, DetectConfig *config);
		static bool saveConfigFile(const QString &fileName, DetectConfig *config);
		static bool loadConfigFile(const QString &fileName, AdminConfig *config);
		static bool saveConfigFile(const QString &fileName, AdminConfig *config);

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
	//���������ڼ�ʹ�õ���ʱ���������
	class DetectParams 
	{
	public:
		QString sampleModelNum; //�ͺ�
		QString sampleBatchNum; //���κ�
		QString sampleNum; //�������
		int currentRow_detect; //����к�
		int currentRow_extract; //��ȡ�к�
		int nCamera; //�������
		int nPhotographing; //���մ���

		enum ErrorCode {
			ValidParams = 0x000,
			Uncheck = 0x300,
			Invalid_nCamera = 0x301,
			Default = 0x3FF
		};

	private:
		ErrorCode errorCode = Uncheck;

	public:
		DetectParams() = default;
		~DetectParams() = default;

		void resetSerialNum();
		void loadDefaultValue();
		int updateGridSize(AdminConfig *adminConfig, DetectConfig *detectConfig);
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������
	};
#endif //STRUCT_DETECT_PARAMS
}
