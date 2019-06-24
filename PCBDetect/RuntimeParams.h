#pragma once

#include <QWidget>
#include <QString>
#include <QDesktopWidget>
#include "Configurator.h"


namespace pcb 
{
#ifndef STRUCT_DETECT_RESULT
#define STRUCT_DETECT_RESULT
	//�����
	struct DetectResult {

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
		QString serialNum; //��Ʒ���
		QString sampleModelNum; //�ͺ�
		QString sampleBatchNum; //���κ�
		QString sampleNum; //�������

		QPoint maskRoi_tl; //ģ����Ĥ���Ͻ��ڷ�ͼ�ϵ�����λ��
		QPoint maskRoi_br; //ģ����Ĥ���½��ڷ�ͼ�ϵ�����λ��
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

		double nCamera_raw; //ԭʼ���������
		double nPhotographing_raw; //ԭʼ�����մ���

		//ϵͳ״ֵ̬ 0x123456789
		//adminConfig 1�� userConfig 2, RuntimeParams 3
		//MotionControler 4, CameraControler 5, ImageConvert 6
		//SerialNumber 7, TemplateExtract 8, Detect 9
		long systemState = 0x000000000;

		//�ܳ���=�ͺų���+���κų���+��ų���
		const int serialNumSlice[4] = { 8, 2, 2, 4 }; //��Ʒ������

	public:
		RuntimeParams();
		~RuntimeParams();

		void resetSerialNum();
		void loadDefaultValue();
		void copyTo(RuntimeParams *dst); //��������

		void updateScreenRect();

		ErrorCode calcSingleMotionStroke(pcb::AdminConfig *adminConfig);
		ErrorCode calcItemGridSize(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);
		ErrorCode calcInitialPhotoPos(pcb::AdminConfig *adminConfig);
		bool update(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);

		ErrorCode parseSerialNum();
		int getSystemResetCode(RuntimeParams &newConfig); //��ȡϵͳ���ô���

		ErrorCode checkValidity(ParamsIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(ParamsIndex index = Index_All, bool doCheck = false, AdminConfig *adminConfig = Q_NULLPTR);
		ErrorCode getErrorCode(ParamsIndex index = Index_All);//��ȡ�������
		void resetErrorCode(ParamsIndex index = Index_All);//���ô������
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������
	};
#endif //STRUCT_DETECT_PARAMS
}