#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include <string>
#include "direct.h"
#include <io.h>



class TemplFunc {

private:
	Ui::DetectConfig* config;
	Ui::DetectParams* params;

	cv::Point minloc, maxloc;//最小值位置，最大值位置，
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//匹配的感兴趣区域及其灰度图，二值图，匹配图，L形的模板图，掩模，结果图
	cv::Point lf1, lf2, br1, br2;//左上角，左下角，右上角，右下角
	cv::Mat big_templ; //大模板;

public:
	TemplFunc() = default;
	~TemplFunc(){};

	void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }
	void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR) { params = ptr; }

	void generateBigTempl();
	cv::Mat find1(int currentCol, cv::Mat &image);

	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }
};