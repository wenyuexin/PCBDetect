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

	cv::Point minloc, maxloc;//��Сֵλ�ã����ֵλ�ã�
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//ƥ��ĸ���Ȥ������Ҷ�ͼ����ֵͼ��ƥ��ͼ��L�ε�ģ��ͼ����ģ�����ͼ
	cv::Point lf1, lf2, br1, br2;//���Ͻǣ����½ǣ����Ͻǣ����½�
	cv::Mat big_templ; //��ģ��;

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