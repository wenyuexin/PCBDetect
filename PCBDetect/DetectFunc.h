#pragma once
#include "Configurator.h"
#include "direct.h"
#include <io.h>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include <qDebug>


class DetectFunc {

private:
	pcb::DetectConfig* config;
	pcb::DetectParams* params;
	
	//配准函数需要用到的参数
	int MAX_FEATURES = 500;
	float GOOD_MATCH_PERCENT = 0.15f;//保留的良好匹配的比例

public:
	DetectFunc() = default;
	~DetectFunc() {};

	void setDetectConfig(pcb::DetectConfig* ptr = Q_NULLPTR) { config = ptr; }
	void setDetectParams(pcb::DetectParams* ptr = Q_NULLPTR) { params = ptr; }

	void alignImages(cv::Mat &im1Gray, cv::Mat &im2Gray, cv::Mat &im1Reg, cv::Mat &h, cv::Mat &imMatches);
	cv::Mat sub_process(cv::Mat &imgOut, cv::Mat &imgOut2);
	void markDefect(cv::Mat &diffBw, cv::Mat &src, cv::Mat &temp_bw, cv::Mat &templ_reg, int &defectNum,int currentCol);

private:
	float correlationCoefficient(const std::vector<double> &X, const std::vector<double> &Y);

	
	

};