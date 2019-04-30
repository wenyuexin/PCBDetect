#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLib.h"
#include <string>
#include "direct.h"
#include <io.h>
#include <qDebug>


class TemplFunc {

private:
	pcb::AdminConfig *adminConfig;
	pcb::UserConfig *userConfig;
	pcb::RuntimeParams *runtimeParams;

	cv::Point minloc, maxloc;//最小值位置，最大值位置，
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//匹配的感兴趣区域及其灰度图，二值图，匹配图，L形的模板图，掩模，结果图
	cv::Point lf1, lf2, br1, br2;//左上角，左下角，右上角，右下角
	cv::Mat big_templ; //大模板;

public:
	TemplFunc();
	~TemplFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void generateBigTempl();
	cv::Mat find1(int currentCol, cv::Mat &image);

	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);
};