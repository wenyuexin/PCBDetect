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
	int cols;//��
	int rows;//��
	cv::Mat big_templ; //��ģ��;

private:
	cv::Point minloc, maxloc;//��Сֵλ�ã����ֵλ�ã�
	int i, j;
	cv::Mat roi, roi_gray, roi_bw, match, shape, mask, image_mask;//ƥ��ĸ���Ȥ������Ҷ�ͼ����ֵͼ��ƥ��ͼ��L�ε�ģ��ͼ����ģ�����ͼ
	cv::Point lf1, lf2, br1, br2;//���Ͻǣ����½ǣ����Ͻǣ����½�
	
};