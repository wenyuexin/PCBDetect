#pragma once

#include <QWidget>
#include <QString>
#include <QDesktopWidget>
#include "Configurator.h"


namespace pcb 
{
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
		bool DeveloperMode; //开启开发者模式
		QString AppDirPath; //程序所在目录
		QString BufferDirPath; //缓存文件夹
		QString currentSampleDir; //当前样本图所在目录
		QString currentTemplDir; //当前模板图所在目录
		QString currentOutputDir; //当前的结果存储目录
		QRect ScreenRect; //界面所在的屏幕区域

		//系统运行所需的关键参数
		QString serialNum; //产品序号
		QString sampleModelNum; //型号
		QString sampleBatchNum; //批次号
		QString sampleNum; //样本编号

		QPoint maskRoi_tl; //模板掩膜左上角在分图上的坐标位置
		QPoint maskRoi_br; //模板掩膜右下角在分图上的坐标位置
		int currentRow_extract; //提取行号
		int currentRow_detect; //检测行号

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

		double nCamera_raw; //原始的相机个数
		double nPhotographing_raw; //原始的拍照次数

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
		void copyTo(RuntimeParams *dst); //拷贝参数

		void updateScreenRect();

		ErrorCode calcSingleMotionStroke(pcb::AdminConfig *adminConfig);
		ErrorCode calcItemGridSize(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);
		ErrorCode calcInitialPhotoPos(pcb::AdminConfig *adminConfig);
		bool update(pcb::AdminConfig *adminConfig, pcb::UserConfig *userConfig);

		ErrorCode parseSerialNum();
		int getSystemResetCode(RuntimeParams &newConfig); //获取系统重置代码

		ErrorCode checkValidity(ParamsIndex index = Index_All, AdminConfig *adminConfig = Q_NULLPTR);
		bool isValid(ParamsIndex index = Index_All, bool doCheck = false, AdminConfig *adminConfig = Q_NULLPTR);
		ErrorCode getErrorCode(ParamsIndex index = Index_All);//获取错误代码
		void resetErrorCode(ParamsIndex index = Index_All);//重置错误代码
		bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告
	};
#endif //STRUCT_DETECT_PARAMS
}