#pragma once

#include <QWidget>
#include <QString>
#include <QDesktopWidget>
#include "Configurator.h"


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
	//检测结果
	struct DetectResult { 

	};
#endif //STRUCT_DETECT_RESULT


#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//运行参数
	class RuntimeParams
	{
	public:
		//系统辅助参数
		QString AppDirPath; //程序所在目录
		QString BufferDirPath; //缓存文件夹
		QString currentSampleDir; //当前样本图所在目录
		QString currentTemplDir; //当前模板图所在目录
		QString currentOutputDir; //当前的结果存储目录
		bool DeveloperMode; //开启开发者模式
		QRect ScreenRect; //界面所在的屏幕区域
		//long systemState; //系统状态值

		//系统运行所需的关键参数
		QString serialNum; //产品序号
		QString sampleModelNum; //型号
		QString sampleBatchNum; //批次号
		QString sampleNum; //样本编号

		int currentRow_detect; //检测行号
		int currentRow_extract; //提取行号

		double singleMotionStroke; //运功动结构的单步行程 mm
		int nCamera; //相机个数
		int nPhotographing; //拍照次数
		double initialPhotoPos; //初始拍照位置

		enum ParamsIndex {
			Index_All,
			Index_None,
			//产品序号相关
			Index_All_SerialNum,
			Index_serialNum,
			Index_sampleModelNum,
			Index_sampleBatchNum,
			Index_sampleNum,
			//行号
			Index_currentRow_detect,
			Index_currentRow_extract,
			//初始化相关
			Index_All_SysInit,
			Index_singleMotionStroke,
			Index_nCamera,
			Index_nPhotographing,
			Index_initialPhotoPos,
			//其他
			Index_AppDirPath,
			Index_BufferDirPath
		};

		enum ErrorCode {
			ValidParams = 0x000,
			ValidValue = 0x000,
			ValidValues = 0x000,
			Unchecked = 0x300,
			Invalid_serialNum = 0x301,
			Invalid_sampleModelNum = 0x302,
			Invalid_sampleBatchNum = 0x303,
			Invalid_sampleNum = 0x304,
			Invalid_currentRow_detect = 0x305,
			Invalid_currentRow_extract = 0x306,
			Invalid_singleMotionStroke = 0x307,
			Invalid_nCamera = 0x308,
			Invalid_nPhotographing = 0x309,
			Invalid_initialPhotoPos = 0x30A,
			Invalid_AppDirPath = 0x30B,
			Invalid_BufferDirPath = 0x30C,
			Default = 0x3FF
		};

	private:
		ErrorCode errorCode;
		ErrorCode errorCode_serialNum;
		ErrorCode errorCode_sysInit;

		//系统状态值 0x123456789
		//adminConfig 1， userConfig 2, RuntimeParams 3
		//MotionControler 4, CameraControler 5, ImageConvert 6
		//SerialNumber 7, TemplateExtract 8, Detect 9
		long systemState = 0x000000000;

		//总长度=型号长度+批次号长度+编号长度
		const int serialNumSlice[4] = { 8, 2, 2, 4 }; //产品序号组成

	public:
		RuntimeParams();
		~RuntimeParams();

		void resetSerialNum();
		void loadDefaultValue();
		ErrorCode calcSingleMotionStroke(pcb::AdminConfig *adminConfig);
		ErrorCode calcInitialPhotoPos(pcb::AdminConfig *adminConfig);
		ErrorCode calcItemGridSize(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);
		ErrorCode parseSerialNum();

		ErrorCode checkValidity(ParamsIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(ParamsIndex index = Index_All, bool doCheck = false, AdminConfig *adminConfig = Q_NULLPTR);
		ErrorCode getErrorCode(ParamsIndex index = Index_All);//获取错误代码
		void resetErrorCode(ParamsIndex index = Index_All);//重置错误代码
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

		void copyTo(RuntimeParams *dst); //拷贝参数
		int getSystemResetCode(RuntimeParams &newConfig); //获取系统重置代码
	};
#endif //STRUCT_DETECT_PARAMS
}
