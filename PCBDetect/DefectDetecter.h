#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLib.h"
#include "DetectFunc.h"


//检测核心类
class DefectDetecter : public QObject
{
	Q_OBJECT

public:
	//检测状态
	enum DetectState {
		InitialState,
		Start,
		Finished,
		Error
	};

	//错误代码
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x900,
		Default = 0x9FF
	};

private:
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数

	DetectFunc *detectFunc;
	pcb::CvMatArray *cvmatSamples; //正在检测的样本
	pcb::DetectResult *detectResult; //检测结果
	int detectState; //检测状态（用于界面显示和程序调试）
	int defectNum; //缺陷数

public:
	DefectDetecter();
	~DefectDetecter();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //系统参数
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } //用户参数
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; } //运行参数
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; } //样本图
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; } //检测结果

	void initDetectFunc();
	void detect();

private:
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void updateDetectState_detecter(int state);
	void detectFinished_detectThread(bool qualified);
};
