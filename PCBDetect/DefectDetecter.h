#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "DetectFunc.h"


//检测核心类
class DefectDetecter : public QObject
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数
	pcb::DetectResult *detectResult; //检测结果
	pcb::CvMatVector *samples; //正在检测的一行样本
	int *currentRow; //当前的样本行号（对应第几次拍）
	int detectState; //检测状态（用于界面显示和程序调试）

	int defectNum = 0;//缺陷编号

public:
	DefectDetecter();
	~DefectDetecter();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig* ptr) { detectConfig = ptr; } //来自配置文件或参数设置界面
	inline void setDetectParams(pcb::DetectParams* ptr) { detectParams = ptr; } //临时变量或临时参数
	inline void setSampleImages(pcb::CvMatVector* ptr) { samples = ptr; } //当前正在检测的一行样本
	inline void setDetectResult(pcb::DetectResult* ptr) { detectResult = ptr; } //检测结果

	void doDetect();

private:
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void sig_detectState_detectCore(int state);
};

