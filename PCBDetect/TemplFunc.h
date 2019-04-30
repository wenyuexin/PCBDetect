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

	cv::Point minloc, maxloc;//��Сֵλ�ã����ֵλ�ã�
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//ƥ��ĸ���Ȥ������Ҷ�ͼ����ֵͼ��ƥ��ͼ��L�ε�ģ��ͼ����ģ�����ͼ
	cv::Point lf1, lf2, br1, br2;//���Ͻǣ����½ǣ����Ͻǣ����½�
	cv::Mat big_templ; //��ģ��;

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