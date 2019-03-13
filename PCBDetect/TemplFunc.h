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

	cv::Point minloc, maxloc;//��Сֵλ�ã����ֵλ�ã�
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//ƥ��ĸ���Ȥ������Ҷ�ͼ����ֵͼ��ƥ��ͼ��L�ε�ģ��ͼ����ģ�����ͼ
	cv::Point lf1, lf2, br1, br2;//���Ͻǣ����½ǣ����Ͻǣ����½�
	cv::Mat big_templ; //��ģ��;

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