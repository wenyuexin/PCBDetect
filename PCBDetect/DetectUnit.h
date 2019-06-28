#pragma once
#include <QThread>
#include <QDebug>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "DetectFunc.h"
#include <vector>
#include <map>

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

	double scalingFactor; //缩放因子，只读
	cv::Size *scaledFullImageSize; //经过缩放后的整图的尺寸，只读
	cv::Size *scaledSubImageSize; //经过缩放后的分图的尺寸，只读

	int defectNum; //分图的缺陷数
	cv::Mat markedSubImage; //标记后的分图，使用完需清空
	std::map<cv::Point3i, cv::Mat> detailImage;//缺陷细节图，使用完需清空

public:
	DetectUnit(QObject *parent = Q_NULLPTR);
	~DetectUnit();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //设置系统参数
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //设置用户参数
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //设置运行参数

	inline void setDetectFunc(DetectFunc *ptr) { detectFunc = ptr; } //设置检测函数类
	inline void setSubImage(cv::Mat &src) { samp = src; } //设置分图

	inline void setCurrentRow(int row) { curRow = row; }
	inline void setCurrentCol(int col) { curCol = col; }
	inline void setMaskRoi(cv::Point *bl, cv::Point *tr) { maskRoi_bl = bl; maskRoi_bl = tr; }

	inline void setScalingFactor(double f) { scalingFactor = f; } //设置缩放因子
	inline void setScaledFullImageSize(cv::Size *sz) { scaledFullImageSize = sz; } //设置经过缩放后的整图图像尺寸
	inline void setScaledSubImageSize(cv::Size *sz) { scaledSubImageSize = sz; } //设置经过缩放后的分图图像尺寸

	inline int getDefectNum() { defectNum = detailImage.size(); return defectNum; } //获取分图的缺陷数
	inline std::map<cv::Point3i, cv::Mat> getDetailImage(){ return detailImage; }//获取缺陷信息及细节图
	inline cv::Mat getMarkedSubImage() { return markedSubImage; }


protected:
	void run();
};
