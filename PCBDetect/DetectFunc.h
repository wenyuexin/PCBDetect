#pragma once
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "direct.h"
#include <io.h>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include <qDebug>
#include <map>

struct cmp_point3i
{
	bool operator()(const cv::Point3i &p1, const cv::Point3i &p2)const
	{
		
		if (p1.x != p2.x)
			return p1.x < p2.x;
		else if (p1.y != p2.y)
			return p1.y < p2.y;
		else
			return p1.z < p2.z;
	}
};


class DetectFunc {

private:
	//以下参数均为只读参数
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数

	//const std::vector<std::string> defect_str { "","断路","缺失","短路","凸起" }; //缺陷类型
	const std::vector<std::string> defect_str{ "","kai","que","duan","tu" }; //缺陷类型

public:
	DetectFunc();
	~DetectFunc();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	bool alignImagesECC(cv::Mat &image_template_gray, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &warp_matrix);
	bool alignImages_test(cv::Mat &image_template_gray, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &H, cv::Mat &imMatches);
	bool alignImages_surf_load(std::vector<cv::KeyPoint> &keypoints_1, cv::Mat& descriptors_1, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &H, cv::Mat &imMatches);//测试载入特征
	bool alignImages_sift_load(std::vector<cv::KeyPoint> &keypoints_1, cv::Mat& descriptors_1, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &H, cv::Mat &imMatches);//测试载入特征

	
	cv::Mat sub_process_new(cv::Mat &templBw, cv::Mat &sampBw, cv::Mat& mask_roi, cv::Mat& directFlaw, cv::Mat& MorphFlaw, cv::Mat& cannyFlaw);
	cv::Mat sub_process_new_small(cv::Mat &templBw, cv::Mat &sampBw, cv::Mat& mask_roi);
	cv::Mat sub_process_direct(cv::Mat &templBw, cv::Mat &sampBw, cv::Mat &templGray, cv::Mat &sampGray, cv::Mat& mask_roi);
	cv::Mat DetectFunc::markDefect_test(int currentCol, cv::Mat &diffBw, cv::Mat &sampGrayReg, double scalingFactor, cv::Mat &templBw, cv::Mat &templGray,cv::Mat sampBw, int &defectNum, std::map<cv::Point3i, cv::Mat, cmp_point3i> &detailImage, cv::Mat rectBlack);
	
	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);

	cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2);

	cv::Mat myThresh(int curCol, int curRow, const cv::Mat& grayImg, cv::Point point_left, cv::Point point_right,int segThresh, bool threshFlag);
	cv::Rect getRect(int curCol, int curRow, const cv::Mat& grayImg, cv::Point point_left, cv::Point point_right);

	float bulge_missing_line_width(cv::Mat &templBw, std::vector<cv::Point> change_point, int angle);//计算缺失线路宽度
	float bulge_missing_percentage(cv::Mat &templBw, std::vector<cv::Point> change_point, cv::Point2f *rectPointsOfPart);//计算缺失百分比

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

	std::vector<std::vector<cv::Mat>> templateVec,maskVec;



};