#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include <string>
#include "direct.h"
#include <io.h>
#include <qDebug>


class TemplFunc {

private:
	pcb::AdminConfig *adminConfig;
	pcb::DetectConfig *detectConfig;
	pcb::DetectParams *detectParams;

	cv::Point minloc, maxloc;//最小值位置，最大值位置，
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//匹配的感兴趣区域及其灰度图，二值图，匹配图，L形的模板图，掩模，结果图
	cv::Point lf1, lf2, br1, br2;//左上角，左下角，右上角，右下角
	cv::Mat big_templ; //大模板;

public:
	TemplFunc();
	~TemplFunc() = default;

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	void generateBigTempl();
	cv::Mat find1(int currentCol, cv::Mat &image);

	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }
};