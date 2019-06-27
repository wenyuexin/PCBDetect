#pragma once
#include <QThread>
#include <QDebug>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "DetectFunc.h"


//用于检测单个分图的线程
class DetectUnit : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	DetectFunc *detectFunc; //检测函数类

	cv::Mat samp; //正在检测的分图
	int curRow; //分图所在的行，只读
	int curCol; //分图所在的列，只读
	cv::Point *maskRoi_bl; //掩模区域左下角坐标，只读
	cv::Point *maskRoi_tr; //掩模区域右上角坐标，只读

	int defectNum; //分图的缺陷数
	cv::Mat markedSubImage; //标记后的分图

public:
	DetectUnit(QObject *parent = Q_NULLPTR);
	~DetectUnit();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //设置系统参数
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //设置用户参数
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //设置运行参数

	inline void setDetectFunc(DetectFunc *ptr) { detectFunc = ptr; } //设置检测函数类
	inline void setSubImage(cv::Mat &src) { samp = src; } //设置分图

	inline void setMaskRoi(cv::Point *bl, cv::Point *tr) { maskRoi_bl = bl; maskRoi_bl = tr; }

	inline int getDefectNum() { return defectNum; } //获取分图的缺陷数
	inline cv::Mat getMarkedSubImage() { return markedSubImage; }


protected:
	void run();
};
