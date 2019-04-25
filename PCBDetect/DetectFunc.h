#pragma once
#include "Configurator.h"
#include "RuntimeLib.h"
#include "direct.h"
#include <io.h>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include <qDebug>

using namespace std;
using namespace cv;


class DetectFunc {

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���
	pcb::DetectResult *detectResult; //�����

	cv::Mat big_templ; //��ģ��;


public:
	DetectFunc();
	~DetectFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; }


	bool alignImages_test_load(std::vector<KeyPoint> &keypoints_1, Mat& descriptors_1, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches);//������������
	cv::Mat sub_process_new(cv::Mat &imgTempl, cv::Mat &sampBw, Mat& mask_roi);
	void markDefect_new(Mat &diffBw, Mat &sampGrayReg, Mat &templBw, Mat &templGray, int &defectNum, int currentCol);
	void markDefect_test(Mat &diffBw, Mat &sampGrayReg, Mat &templBw, Mat &templGray, int &defectNum, int currentCol);
	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);
	Scalar getMSSIM(const Mat& i1, const Mat& i2);


	void generateBigTempl();
	inline cv::Mat getBigTempl() { return big_templ; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return big_templ(rect); }

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

	std::string batch_path, num_path, out_path;

private:

};