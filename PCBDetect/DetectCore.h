#pragma once
#include "Configurator.h"
#include "opencv2/opencv.hpp"
#include "DetectFunc.h"


namespace Ui {
#ifndef TYPE_CV_MAT_VECTOR
#define TYPE_CV_MAT_VECTOR 
	typedef std::vector<cv::Mat> CvMatVector;
#endif //TYPE_CV_MAT_VECTOR
}


//检测核心类
class DetectCore : public QObject
{
	Q_OBJECT

private:
	Ui::DetectConfig *config; //用于检测的参数配置
	Ui::DetectParams *params; //用于检测的参数配置
	Ui::DetectResult *result; //检测结果
	Ui::CvMatVector *samples; //正在检测的一行样本
	int *currentRow; //当前的样本行号（对应第几次拍）
	int detectState; //检测状态（用于界面显示和程序调试）

	int defectNum = 0;//缺陷编号

public:
	DetectCore();
	~DetectCore();

	void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR);
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR);
	void setDetectResult(Ui::DetectResult *ptr = Q_NULLPTR);
	void setSampleImages(Ui::CvMatVector *ptr = Q_NULLPTR);
	void doDetect();

private:
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void sig_detectState_detectCore(int state);
};

