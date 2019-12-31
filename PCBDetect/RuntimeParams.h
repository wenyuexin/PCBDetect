#pragma once

#include <QWidget>
#include <QString>
#include <QDesktopWidget>
#include "Configurator.h"
#include "ProductID.h"


namespace pcb 
{
#ifndef STRUCT_DETECT_RESULT
#define STRUCT_DETECT_RESULT
	//ȱ����Ϣ
	struct FlawInfo {
		//QString filePath;
		QString flawIndex; //ȱ�ݱ��
		int xPos; //ȱ������ͼ�е�x����
		int yPos; //ȱ������ͼ�е�y����
		int flawType; //ȱ�����ͣ�0δ֪ 1��· 2��· 3͹�� 4ȱʧ
	};

	//�����
	struct DetectResult {
		bool SampleIsQualified; //��ǰ��Ʒ�Ƿ�ϸ�
		cv::Mat fullImage; //�����ȱ�ݵ���ͼ
		std::vector<FlawInfo> flawInfo; //ȱ����Ϣ
		QDate detectionDate; //�������
	};
#endif //STRUCT_DETECT_RESULT


#ifndef STRUCT_DETECT_PARAMS
#define STRUCT_DETECT_PARAMS
	//���в���
	class RuntimeParams
	{
	public:
		//ϵͳ��������
		bool DeveloperMode; //����������ģʽ
		QString AppDirPath; //��������Ŀ¼
		QString BufferDirPath; //�����ļ���
		QString currentSampleDir; //��ǰ����ͼ����Ŀ¼
		QString currentTemplDir; //��ǰģ��ͼ����Ŀ¼
		QString currentOutputDir; //��ǰ�Ľ���洢Ŀ¼
		QRect ScreenRect; //�������ڵ���Ļ����

		//ϵͳ��������Ĺؼ�����
		ProductID productID; //��Ʒid

		QPoint maskRoi_tl; //ģ����Ĥ���Ͻ��ڷ�ͼ�ϵ�����λ��
		QPoint maskRoi_br; //ģ����Ĥ���½��ڷ�ͼ�ϵ�����λ��
		bool UsingDefaultSegThresh; //ʹ��Ĭ�ϵķָ���ֵ
		int segThresh; //ͼ��ָ����ֵ 0-255

		int currentRow_extract; //��ȡ�к�
		int currentRow_detect; //����к�

		double singleMotionStroke; //�˹����ṹ�ĵ����г� mm
		int nCamera; //�������
		int nPhotographing; //���մ���
		double initialPhotoPos; //��ʼ����λ��

		enum ParamsIndex {
			Index_All,
			Index_None,
			//��Ʒ������
			Index_productID,
			//�к�
			Index_currentRow_detect,
			Index_currentRow_extract,
			//��ʼ�����
			Index_All_SysInit,
			Index_singleMotionStroke,
			Index_nCamera,
			Index_nPhotographing,
			Index_initialPhotoPos,
			//����
			Index_AppDirPath,
			Index_BufferDirPath
		};

		enum ErrorCode {
			ValidParams = 0x000,
			ValidValue = 0x000,
			ValidValues = 0x000,
			Unchecked = 0x300,
			Invalid_productID = 0x301,
			Invalid_currentRow_detect = 0x302,
			Invalid_currentRow_extract = 0x303,
			Invalid_singleMotionStroke = 0x304,
			Invalid_nCamera = 0x305,
			Invalid_nPhotographing = 0x306,
			Invalid_initialPhotoPos = 0x307,
			Invalid_AppDirPath = 0x308,
			Invalid_BufferDirPath = 0x309,
			Default = 0x3FF
		};

	private:
		ErrorCode errorCode;
		ErrorCode errorCode_sysInit;

		//ϵͳ״ֵ̬ 0x123456789
		//adminConfig 1�� userConfig 2, RuntimeParams 3
		//MotionControler 4, CameraControler 5, ImageConvert 6
		//SerialNumber 7, TemplateExtract 8, Detect 9
		long systemState = 0x000000000;

		double nCamera_raw; //ԭʼ���������
		double nPhotographing_raw; //ԭʼ�����մ���

	public:
		RuntimeParams();
		~RuntimeParams();

		void loadDefaultValue();
		void copyTo(RuntimeParams *dst); //��������

		void updateScreenRect();

		ErrorCode calcSingleMotionStroke(pcb::AdminConfig *adminConfig);
		ErrorCode calcItemGridSize(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);
		ErrorCode calcInitialPhotoPos(pcb::AdminConfig *adminConfig);
		bool update(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);

		int getSystemResetCode(RuntimeParams &newConfig); //��ȡϵͳ���ô���

		ErrorCode checkValidity(ParamsIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(ParamsIndex index = Index_All, bool doCheck = false, AdminConfig *adminConfig = Q_NULLPTR);
		ErrorCode getErrorCode(ParamsIndex index = Index_All);//��ȡ�������
		void resetErrorCode(ParamsIndex index = Index_All);//���ô������
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������
	};
#endif //STRUCT_DETECT_PARAMS
}