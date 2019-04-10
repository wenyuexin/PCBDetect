#pragma once
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "direct.h"
#include <io.h>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include <qDebug>


class DetectFunc {

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���

	//��׼������Ҫ�õ��Ĳ���
	int MAX_FEATURES = 500;
	float GOOD_MATCH_PERCENT = 0.15f;//����������ƥ��ı���

public:
	DetectFunc() = default;
	~DetectFunc() {};

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void alignImages(cv::Mat &im1Gray, cv::Mat &im2Gray, cv::Mat &im1Reg, cv::Mat &h, cv::Mat &imMatches);
	cv::Mat sub_process(cv::Mat &imgOut, cv::Mat &imgOut2);
	void markDefect(cv::Mat &diffBw, cv::Mat &src, cv::Mat &temp_bw, cv::Mat &templ_reg, int &defectNum,int currentCol);

private:
	float correlationCoefficient(const std::vector<double> &X, const std::vector<double> &Y);

	
	

};