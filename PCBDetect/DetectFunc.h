#pragma once
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "direct.h"
#include <io.h>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include <qDebug>


class DetectFunc {

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	pcb::DetectResult *detectResult; //�����

	double scalingFactor; //��������
	cv::Size scaledFullImageSize; //�������ź����ͼ�ĳߴ�
	cv::Size scaledSubImageSize; //�������ź�ķ�ͼ�ĳߴ�
	cv::Mat big_templ; //��ģ��;

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

public:
	DetectFunc();
	~DetectFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; }

	bool alignImages_test(cv::Mat &image_template_gray, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &H, cv::Mat &imMatches);
	bool alignImages_test_load(std::vector<cv::KeyPoint> &keypoints_1, cv::Mat& descriptors_1, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &H, cv::Mat &imMatches);//������������
	
	cv::Mat sub_process_new(cv::Mat &templBw, cv::Mat &sampBw, cv::Mat& mask_roi);
	cv::Mat sub_process_new_small(cv::Mat &templBw, cv::Mat &sampBw, cv::Mat& mask_roi);
	cv::Mat sub_process_direct(cv::Mat &templBw, cv::Mat &sampBw, cv::Mat &templGray, cv::Mat &sampGray, cv::Mat& mask_roi);
	void markDefect_test(cv::Mat &diffBw, cv::Mat &sampGrayReg, cv::Mat &templBw, cv::Mat &templGray, int &defectNum, int currentCol);
	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);
	cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2);





	void generateBigTempl();
	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }

	cv::Mat myThresh(int curCol, int curRow, const cv::Mat& grayImg, cv::Point point_left, cv::Point point_right);
	cv::Rect getRect(int curCol, int curRow, const cv::Mat& grayImg, cv::Point point_left, cv::Point point_right);
	cv::Point get_bl() {
		return bl;
	}
	cv::Point get_tr() {
		return tr;
	}
	void set_bl(cv::Point pt) {
		bl = pt;
	}
	void set_tr(cv::Point pt) {
		tr = pt;
	}
private:
	cv::Point bl;
	cv::Point tr;

};