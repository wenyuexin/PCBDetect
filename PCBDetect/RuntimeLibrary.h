#pragma once

#include <QString>
#include <QWidget>
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
	class DetectParams
	{
	public:
		QString sampleModelNum; //型号
		QString sampleBatchNum; //批次号
		QString sampleNum; //样本编号
		int currentRow_detect; //检测行号
		int currentRow_extract; //提取行号
		int singleMotionStroke; //运功动结构的单步行程 mm
		int nCamera; //相机个数
		int nPhotographing; //拍照次数

		enum ParamsIndex {
			Index_All,
			Index_None,
			Index_sampleModelNum,
			Index_sampleBatchNum,
			Index_sampleNum,
			Index_currentRow_detect,
			Index_currentRow_extract,
			Index_singleMotionStroke,
			Index_nCamera,
			Index_nPhotographing
		};

		enum ErrorCode {
			ValidParams = 0x000,
			ValidValue = 0x000,
			Uncheck = 0x300,
			Invalid_singleMotionStroke = 0x301,
			Invalid_nCamera = 0x302,
			Invalid_nPhotographing = 0x303,
			Default = 0x3FF
		};

	private:
		ErrorCode errorCode = Uncheck;

		//系统状态值 0x123456789
		//adminConfig 1， detectConfig 2, DetectParams 3
		//MotionControler 4, CameraControler 5, ImageConvert 6
		//TemplateExtract 7, Detect 8, SerialNumber 9
		long systemState = 0x000000000;

	public:
		DetectParams();
		~DetectParams();

		void resetSerialNum();
		void loadDefaultValue();
		int calcSingleMotionStroke(pcb::AdminConfig *adminConfig);
		int calcItemGridSize(pcb::AdminConfig *adminConfig, pcb::DetectConfig *detectConfig);

		bool isValid(AdminConfig *adminConfig = Q_NULLPTR);
		ErrorCode checkValidity(ParamsIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		inline ErrorCode getErrorCode() { return errorCode; }
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告
	};
#endif //STRUCT_DETECT_PARAMS
}


