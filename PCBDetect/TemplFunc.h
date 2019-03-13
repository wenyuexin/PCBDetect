#pragma once
#include "opencv2/opencv.hpp"
#include <string>
#include "direct.h"
#include <io.h>



class TemplFunc {
public:
	TemplFunc() = default;
	~TemplFunc(){};

	cv::Mat find1(int num_cols, int num_rows, cv::Mat &image);
	int cols;//列
	int rows;//行
	cv::Mat big_templ; //大模板;

private:
	cv::Point minloc, maxloc;//最小值位置，最大值位置，
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//匹配的感兴趣区域及其灰度图，二值图，匹配图，L形的模板图，掩模，结果图
	cv::Point lf1, lf2, br1, br2;//左上角，左下角，右上角，右下角
	
};