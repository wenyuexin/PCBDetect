#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "DetectUnit.h"
#include "pcbFuncLib.h"
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
		Unchecked = 0x900,
		LoadTemplMaskRoiError = 0x901,
		Default = 0x9FF
	};

private:
	ErrorCode errorCode; //错误代码
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	std::vector<DetectUnit *> detectUnits; //检测单元（线程），一个单元一次可以检测一张分图
	const int MAX_DETECT_UNITS_NUM = 10;

	DetectFunc *detectFunc;
	pcb::CvMatArray *cvmatSamples; //正在检测的样本
	pcb::DetectResult *detectResult; //检测结果
	int detectState; //检测状态（用于界面显示和程序调试）
	int totalDefectNum; //单个PCB板中的缺陷总数

	double scalingFactor; //缩放因子
	cv::Size scaledFullImageSize; //经过缩放后的整图的尺寸
	cv::Size scaledSubImageSize; //经过缩放后的分图的尺寸
	cv::Mat bigTempl; //大模板

	std::map<cv::Point3i, cv::Mat, cmp_point3i> allDetailImage;

	cv::Point maskRoi_bl; //掩模区域的坐标，一张PCB对应一个坐标 
	cv::Point maskRoi_tr;

public:
	DefectDetecter();
	~DefectDetecter();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //系统参数
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //用户参数
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //运行参数
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; } //样本图
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; } //检测结果

	void detect();

private:
	void initDetectFunc();
	void generateBigTempl();

	void makeCurrentSampleDir(std::vector<QString> &subFolders = std::vector<QString>());
	void makeCurrentOutputDir(std::vector<QString> &subFolders = std::vector<QString>());

	void initDetectUnits();
	void deleteDetectUnits();

	inline cv::Mat getBigTempl() { return bigTempl; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return bigTempl(rect); }
	

	//inline cv::Point getMaskRoi_bl() { return maskRoi_bl; }
	//inline cv::Point getMaskRoi_tr() { return maskRoi_tr; }
	inline void setMaskRoi_bl(cv::Point pt) { maskRoi_bl = pt; }
	inline void setMaskRoi_tr(cv::Point pt) { maskRoi_tr = pt; }


Q_SIGNALS:
	void updateDetectState_detecter(int state);
	void detectFinished_detectThread(bool qualified);
};
