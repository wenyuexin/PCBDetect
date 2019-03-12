#pragma once
#include <string>
#include "direct.h"
#include <io.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include <qDebug>

using namespace std;
using namespace cv;
class DetectFunc {
public:
	DetectFunc() = default;
	~DetectFunc() {};
	void alignImages(Mat &im1Gray, Mat &im2Gray, Mat &im1Reg, Mat &h, Mat &imMatches);
	cv::Mat sub_process(cv::Mat &imgOut, cv::Mat &imgOut2);
	void markDefect(Mat &diffBw, Mat &src, Mat &temp_bw, Mat &templ_reg, Ui::DetectConfig* detectConfig, int model_num, int batch_num, int pcb_num, int &defectNum,int cur_row,int cur_col);

private:
	float correlationCoefficient(const vector<double> &X, const vector<double> & Y);

	//配准函数需要用到的参数
	int MAX_FEATURES = 500;
	float GOOD_MATCH_PERCENT = 0.15f;//保留的良好匹配的比例
	

};