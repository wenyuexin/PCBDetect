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

	int roi_x, roi_y, length = 1000;
	cv::Point minloc, maxloc, point_leftup, point_rightup, point_left, point_right;//��Сֵλ�ã����ֵλ�ã�
	cv::Mat roi_image, roi_gray, roi_bw, match, mask, image_mask,shape, roi_shape;//ƥ��ĸ���Ȥ������Ҷ�ͼ����ֵͼ��ƥ��ͼ��L�ε�ģ��ͼ����ģ�����ͼ
	cv::Mat big_templ; //��ģ��;

public:
	TemplFunc();
	~TemplFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	void generateBigTempl();
	void str2int(int &int_temp, const std::string &string_temp);
	cv::Point corner_lf(cv::Mat &image, int lf_x, int lf_y);
	cv::Point corner_br(cv::Mat &image, int br_x, int br_y);
	cv::Mat cutting(int num_cols, int num_rows, int cols, int rows, cv::Mat &image, cv::Point point_left, cv::Point point_right);
	//cv::Mat find1(int currentCol, cv::Mat &image);

	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);
};