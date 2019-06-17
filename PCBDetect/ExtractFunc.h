#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include <string>
#include "direct.h"
#include <io.h>
#include <qDebug>


class ExtractFunc 
{
private:
	pcb::AdminConfig *adminConfig;
	pcb::UserConfig *userConfig;
	pcb::RuntimeParams *runtimeParams;

	cv::Point minloc, maxloc, point_leftup, point_rightup, point_left, point_right, left, right;//最小值位置，最大值位置，
	int i, j, roi_x, roi_y, length = 1000;
	cv::Mat roi_image, roi_gray, roi_bw, match, mask, image_mask, shape, roi_shape;//匹配的感兴趣区域及其灰度图，二值图，匹配图，L形的模板图，掩模，结果图
	cv::Point lf1, lf2, br1, br2;//左上角，左下角，右上角，右下角
	cv::Mat big_templ; //大模板;

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

public:
	ExtractFunc();
	~ExtractFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void generateBigTempl();
	//cv::Mat findLocationMark(int currentCol, cv::Mat &image);

	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }



	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);

	//str2int(字符串转整数)；corner_lf，corner_br（找左下角，右上角的L型的坐标），cutting(提取掩模)
	void str2int(int &int_temp, const std::string &string_temp);
	cv::Point corner_lf(cv::Mat &image, int lf_x, int lf_y);
	cv::Point corner_br(cv::Mat &image, int br_x, int br_y);
	cv::Mat cutting(int num_cols, int num_rows, int cols, int rows, cv::Mat &image, cv::Point point_left, cv::Point point_right);
};