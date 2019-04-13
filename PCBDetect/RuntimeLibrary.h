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
	void delay(unsigned long msec);//�������ӳ�
	QString selectDirPath(QWidget *parent, QString windowTitle = "");//����ʽ�ļ���·��ѡ��
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
	//�����
	struct DetectResult { 

	};
#endif //STRUCT_DETECT_RESULT


#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//���в���
	class DetectParams
	{
	public:
		QString serialNum;
		QString sampleModelNum; //�ͺ�
		QString sampleBatchNum; //���κ�
		QString sampleNum; //�������
		int currentRow_detect; //����к�
		int currentRow_extract; //��ȡ�к�
		int singleMotionStroke; //�˹����ṹ�ĵ����г� mm
		int nCamera; //�������
		int nPhotographing; //���մ���
		QString bufferDirPath; //�����ļ���

		enum ParamsIndex {
			Index_All,
			Index_None,
			//��Ʒ������
			Index_All_SerialNum,
			Index_serialNum,
			Index_sampleModelNum,
			Index_sampleBatchNum,
			Index_sampleNum,
			//�к�
			Index_currentRow_detect,
			Index_currentRow_extract,
			//��ʼ�����
			Index_All_SysInit,
			Index_singleMotionStroke,
			Index_nCamera,
			Index_nPhotographing,
			//����
			Index_bufferDirPath
		};

		enum ErrorCode {
			ValidValue = 0x000,
			ValidValues = 0x000,
			ValidParams = 0x000,
			Uncheck = 0x300,
			Invalid_serialNum = 0x301,
			Invalid_sampleModelNum = 0x302,
			Invalid_sampleBatchNum = 0x303,
			Invalid_sampleNum = 0x304,
			Invalid_currentRow_detect = 0x305,
			Invalid_currentRow_extract = 0x306,
			Invalid_singleMotionStroke = 0x307,
			Invalid_nCamera = 0x308,
			Invalid_nPhotographing = 0x309,
			Invalid_bufferDirPath = 0x30A,
			Default = 0x3FF
		};

	private:
		ErrorCode errorCode;
		ErrorCode errorCode_serialNum;
		ErrorCode errorCode_sysInit;

		//ϵͳ״ֵ̬ 0x123456789
		//adminConfig 1�� detectConfig 2, DetectParams 3
		//MotionControler 4, CameraControler 5, ImageConvert 6
		//SerialNumber 7, TemplateExtract 8, Detect 9
		long systemState = 0x000000000;

		//�ܳ���=�ͺų���+���κų���+��ų���
		const int serialNumSlice[4] = { 8, 2, 2, 4 }; //��Ʒ������

	public:
		DetectParams();
		~DetectParams();

		void resetSerialNum();
		void loadDefaultValue();
		ErrorCode calcSingleMotionStroke(pcb::AdminConfig *adminConfig);
		ErrorCode calcItemGridSize(pcb::AdminConfig *adminConfig, pcb::DetectConfig *detectConfig);
		ErrorCode parseSerialNum();

		ErrorCode checkValidity(ParamsIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(ParamsIndex index = Index_All, bool doCheck = false, AdminConfig *adminConfig = Q_NULLPTR);
		ErrorCode getErrorCode(ParamsIndex index = Index_All);//��ȡ�������
		void resetErrorCode(ParamsIndex index = Index_All);//���ô������
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

		void copyTo(DetectParams *dst); //��������
		int getSystemResetCode(DetectParams &newConfig); //��ȡϵͳ���ô���
	};
#endif //STRUCT_DETECT_PARAMS
}


