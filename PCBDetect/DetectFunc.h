#pragma once
#include <string>
#include "direct.h"
#include <io.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include <qDebug>


class DetectFunc {

private:
	Ui::DetectConfig* config;
	Ui::DetectParams* params;
	
	//��׼������Ҫ�õ��Ĳ���
	int MAX_FEATURES = 500;
	float GOOD_MATCH_PERCENT = 0.15f;//����������ƥ��ı���

public:
	DetectFunc() = default;
	~DetectFunc() {};

	void setDetectConfig(Ui::DetectConfig *ptr);
	void setDetectParams(Ui::DetectParams *ptr);

	void alignImages(cv::Mat &im1Gray, cv::Mat &im2Gray, cv::Mat &im1Reg, cv::Mat &h, cv::Mat &imMatches);
	cv::Mat sub_process(cv::Mat &imgOut, cv::Mat &imgOut2);
	void markDefect(cv::Mat &diffBw, cv::Mat &src, cv::Mat &temp_bw, cv::Mat &templ_reg, int &defectNum,int currentCol);

private:
	float correlationCoefficient(const std::vector<double> &X, const std::vector<double> &Y);

	
	

};