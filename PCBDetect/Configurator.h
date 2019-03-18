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
	private: 
		int validFlag = 0;

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


		enum ConfigIndex { //参数索引
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

		enum ErrorCode { //错误代码
			ValidConfig = 0,
			ConfigFileMissing = 100,
			Invalid_SampleDirPath = 101,
			Invalid_TemplDirPath = 102,
			Invalid_OutputDirPath = 103,
			Invalid_ImageFormat = 104,
			Invalid_nCamera = 105,
			Invalid_nPhotographing = 106,
			Invalid_nBasicUnitInRow = 107,
			Invalid_nBasicUnitInCol = 108,
			Invalid_ImageAspectRatio_W = 109,
			Invalid_ImageAspectRatio_H = 110,
			Invalid_ImageAspectRatio = 111
		};

		//参数有效性
		bool isValid() { 
			if (validFlag == 0) checkValidity();
			return validFlag == 1;
		}

		//参数有效性判断
		ErrorCode checkValidity(ConfigIndex index = Index_All) {
			ErrorCode code = ValidConfig;
			switch (index)
			{
			case Index_All:
			case Index_SampleDirPath:
				if (!QFileInfo(SampleDirPath).isDir()) code = Invalid_SampleDirPath;
				if (code != ValidConfig || index != Index_All) break;
			case Index_TemplDirPath:
				if (!QFileInfo(TemplDirPath).isDir()) code = Invalid_TemplDirPath;
				if (code != ValidConfig || index != Index_All) break;
			case Index_OutputDirPath:
				if (!QFileInfo(OutputDirPath).isDir()) code = Invalid_OutputDirPath;
				if (code != ValidConfig || index != Index_All) break;
			case Index_ImageFormat:
				if (code != ValidConfig || index != Index_All) break;
			case Index_nCamera:
				if (nCamera < 1) code = Invalid_nCamera;
				if (code != ValidConfig || index != Index_All) break;
			case Index_nPhotographing:
				if (nPhotographing < 1) code = Invalid_nPhotographing;
				if (code != ValidConfig || index != Index_All) break;
			case Index_nBasicUnitInRow:
				if (nBasicUnitInRow < 1) code = Invalid_nBasicUnitInRow;
				if (code != ValidConfig || index != Index_All) break;
			case Index_nBasicUnitInCol:
				if (nBasicUnitInCol < 1) code = Invalid_nBasicUnitInCol;
				if (code != ValidConfig || index != Index_All) break;
			case Index_ImageAspectRatio_W:
				if (ImageAspectRatio_W < 1) code = Invalid_ImageAspectRatio_W;
				if (code != ValidConfig || index != Index_All) break;
			case Index_ImageAspectRatio_H:
				if (ImageAspectRatio_H < 1) code = Invalid_ImageAspectRatio_H;
				if (code != ValidConfig || index != Index_All) break;
			case Index_ImageAspectRatio:
				if (code != ValidConfig || index != Index_All) break;
			}
			validFlag = ((code == ValidConfig)? 1 : -1);
			return code;
		}
		
		//计算宽高比
		ErrorCode calcImageAspectRatio() {
			ErrorCode code = checkValidity(Index_ImageAspectRatio_W);
			if (code != ValidConfig) return code;
			code = checkValidity(Index_ImageAspectRatio_H);
			if (code != ValidConfig) return code;
			ImageAspectRatio = 1.0 * ImageAspectRatio_W / ImageAspectRatio_H;
			return ValidConfig;
		}

		//判断是否需要重置系统
		bool getResetFlag(DetectConfig &other) {
			if (nCamera != other.nCamera) return true;
			if (nPhotographing != other.nPhotographing) return true;
			if (ImageAspectRatio_W != other.ImageAspectRatio_W || ImageAspectRatio_H != other.ImageAspectRatio_H) {
				if (abs(ImageAspectRatio - other.ImageAspectRatio) < 1E-5) return true;
			}
			return false;
		}

		//不相等判断
		ConfigIndex unequals(DetectConfig &other) {
			if (SampleDirPath != other.SampleDirPath) return Index_SampleDirPath;
			if (TemplDirPath != other.TemplDirPath) return Index_TemplDirPath;
			if (OutputDirPath != other.OutputDirPath) return Index_OutputDirPath;
			if (ImageFormat != other.ImageFormat) return Index_ImageFormat;
			if (nCamera != other.nCamera) return Index_nCamera;
			if (nPhotographing != other.nPhotographing) return Index_nPhotographing;
			if (nBasicUnitInRow != other.nBasicUnitInRow) return Index_nBasicUnitInRow;
			if (nBasicUnitInCol != other.nBasicUnitInCol) return Index_nBasicUnitInCol;
			if (ImageAspectRatio_W != other.ImageAspectRatio_W) return Index_ImageAspectRatio_W;
			if (ImageAspectRatio_H != other.ImageAspectRatio_H) return Index_ImageAspectRatio_H;
			return Index_None;
		}

		//拷贝结构体
		void copyTo(DetectConfig &other) {
			other.validFlag = validFlag; //参数有效性
			other.SampleDirPath = SampleDirPath; //样本文件存储路径
			other.TemplDirPath = TemplDirPath;//模板文件的存储路径
			other.OutputDirPath = OutputDirPath;//检测结果存储路径
			other.ImageFormat = ImageFormat; //图像后缀
			other.nCamera = nCamera; //相机个数
			other.nPhotographing = nPhotographing; //拍照次数
			other.nBasicUnitInRow = nBasicUnitInRow; //每一行中的基本单元数
			other.nBasicUnitInCol = nBasicUnitInCol; //每一列中的基本单元数
			other.ImageAspectRatio_W = ImageAspectRatio_W; //宽高比中的宽
			other.ImageAspectRatio_H = ImageAspectRatio_H; //宽高比中的高
			other.ImageAspectRatio = ImageAspectRatio; //样本图像的宽高比
		}

		//参数报错
		static void showMessageBox(QWidget *parent, ErrorCode code) {
			QString valueName;
			if (code == ConfigFileMissing) {
				QString message = QString::fromLocal8Bit("config文件丢失，已生成默认config文件!  \n")
					+ QString::fromLocal8Bit("系统即将重新获取配置参数 ...  \n");
				QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
					message + "ErrorCode: " + QString::number(code),
					QString::fromLocal8Bit("确定"));
				return;
			}

			switch (code)
			{
			case Ui::DetectConfig::Invalid_SampleDirPath:
				valueName = QString::fromLocal8Bit("模板路径"); break;
			case Ui::DetectConfig::Invalid_TemplDirPath:
				valueName = QString::fromLocal8Bit("模板路径"); break;
			case Ui::DetectConfig::Invalid_OutputDirPath:
				valueName = QString::fromLocal8Bit("输出路径"); break;
			case Ui::DetectConfig::Invalid_ImageFormat:
				valueName = QString::fromLocal8Bit("图像格式"); break;
			case Ui::DetectConfig::Invalid_nCamera:
				valueName = QString::fromLocal8Bit("相机个数"); break;
			case Ui::DetectConfig::Invalid_nPhotographing:
				valueName = QString::fromLocal8Bit("拍摄次数"); break;
			case Ui::DetectConfig::Invalid_nBasicUnitInRow:
			case Ui::DetectConfig::Invalid_nBasicUnitInCol:
				valueName = QString::fromLocal8Bit("基本单元数"); break;
			case Ui::DetectConfig::Invalid_ImageAspectRatio_W:
			case Ui::DetectConfig::Invalid_ImageAspectRatio_H:
			case Ui::DetectConfig::Invalid_ImageAspectRatio:
				valueName = QString::fromLocal8Bit("图像宽高比"); break;
			default:
				valueName = ""; break;
			}

			QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
				QString::fromLocal8Bit("参数无效，请在参数设置界面重新设置") + valueName + "!    \n" 
					+ "ErrorCode: " + QString::number(code),
				QString::fromLocal8Bit("确定"));
		}

		static ConfigIndex convertCodeToIndex(ErrorCode code) {
			switch (code)
			{
			case Ui::DetectConfig::ValidConfig:
				return Index_None;
			case Ui::DetectConfig::Invalid_SampleDirPath:
				return Index_SampleDirPath;
			case Ui::DetectConfig::Invalid_TemplDirPath:
				return Index_TemplDirPath;
			case Ui::DetectConfig::Invalid_OutputDirPath:
				return Index_OutputDirPath;
			case Ui::DetectConfig::Invalid_ImageFormat:
				return Index_ImageFormat;
			case Ui::DetectConfig::Invalid_nCamera:
				return Index_nCamera;
			case Ui::DetectConfig::Invalid_nPhotographing:
				return Index_nPhotographing;
			case Ui::DetectConfig::Invalid_nBasicUnitInRow:
				return Index_nBasicUnitInRow;
			case Ui::DetectConfig::Invalid_nBasicUnitInCol:
				return Index_nBasicUnitInCol;
			case Ui::DetectConfig::Invalid_ImageAspectRatio_W:
				return Index_ImageAspectRatio_W;
			case Ui::DetectConfig::Invalid_ImageAspectRatio_H:
				return Index_ImageAspectRatio_H;
			case Ui::DetectConfig::Invalid_ImageAspectRatio:
				return Index_ImageAspectRatio;
			}
			return Index_None;
		}
	};
#endif //STRUCT_DETECT_CONFIG


#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//程序运行期间使用的临时变量或参数
	struct DetectParams {
		QString sampleModelNum; //型号
		QString sampleBatchNum; //批次号
		QString sampleNum; //样本编号
		QSize imageSize; //样本图像的原始尺寸
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

	bool jsonSetValue(const QString &key, QString &value); //写QString
	bool jsonSetValue(const QString &key, int &value); //写int
	bool jsonSetValue(const QString &key, double &value); //写double
	bool jsonReadValue(const QString &key, QString &value); //读QString
	bool jsonReadValue(const QString &key, int &value); //读int
	bool jsonReadValue(const QString &key, double &value); //读double

	

	quint64 getDiskFreeSpace(QString driver);
	bool checkDir(QString dirpath);
};
