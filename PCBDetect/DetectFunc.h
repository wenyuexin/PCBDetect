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
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	pcb::DetectResult *detectResult; //检测结果

	double widthWholeImg = 1500;
	double heightWholeImg = 1000;
	double widthUnit = 1500;
	double heightUnit = 1000;
	double widthZoom = 0.25;
	double heightZoom = 0.25;
	cv::Mat big_templ; //大模板;

public:
	DetectFunc();
	~DetectFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; }

	bool alignImages_test(Mat &image_template_gray, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches);
	bool alignImages_test_load(std::vector<KeyPoint> &keypoints_1, Mat& descriptors_1, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches);//测试载入特征
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