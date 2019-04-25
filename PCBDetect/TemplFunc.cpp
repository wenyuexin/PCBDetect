#include "TemplFunc.h"
#include <qDebug>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

using pcb::DetectConfig;
using pcb::DetectParams;
using cv::Mat;
using namespace std;
using namespace cv;
using cv::Size;
using cv::Rect;
using namespace cv::xfeatures2d;


TemplFunc::TemplFunc()
{
	adminConfig = Q_NULLPTR;
	detectConfig = Q_NULLPTR;
	detectParams = Q_NULLPTR;
}

TemplFunc::~TemplFunc()
{
	qDebug() << "~TemplFunc";
}

/***************** 提取 ******************/

//生成完整尺寸的模板图像
void TemplFunc::generateBigTempl()
{
	Size templSize = Size(adminConfig->ImageSize_W * detectParams->nCamera,
		adminConfig->ImageSize_H * detectParams->nPhotographing);
	big_templ = Mat(templSize, CV_8UC3);
}


/**
*功能：通过图像匹配找到图像的四个角点的位置，分别为左上，左下，右上，右下,并根据角点位置对图像进行切割（除去边角）
*输入：图像所在列数num_cols,图像所在行数num_cols,代表图像位置的编号num，原始图像image
*输出：显示L型角点位置的坐标，并对所切割的结果图保存，返回mask
*/

Mat TemplFunc::find1(int col, cv::Mat &image) {
	int currentCol = col + 1;
	int currentRow = detectParams->currentRow_extract + 1;
	int nCamera = detectParams->nCamera;
	int nPhotographing = detectParams->nPhotographing;

	shape = Mat::zeros(Size(100, 100), CV_8UC1);//创建一个像素值全为0的图像，位深8位无符号整数，单通道的灰度图

	if (currentCol == 1 && currentRow == 1) { //左上
		for (i = 10; i < 60; i++) {
			for (j = 10; j < 12; j++) {
				shape.at<uchar>(i, j) = 255;//访问像素点用at
				shape.at<uchar>(j, i) = 255;
			}
		}
		roi = image(Rect(100, 100, 200, 200));//Rect的参数有x,y,width,height,101-1,称作匹配的感兴趣区域，roi不要写成定值
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配，TM_CCOEFF系数匹配法，相关值
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);//求得匹配图的最小值，最大值的位置
		lf1.x = maxloc.x + 100;//在原始图像中的位置
		lf1.y = maxloc.y + 100;
		//cout << "n1_lf1:" << lf1 << endl;//输出左上角坐标

		//得到感兴趣区域
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = lf1.x;
		rect.y = lf1.y;
		rect.width = image.cols - lf1.x;
		rect.height = image.rows - lf1.y;
		mask(rect).setTo(255);//制作合适的掩模，矩形区域内像素值为255	
	}

	if (currentCol == 1 && currentRow == nPhotographing) { //左下
		for (i = 50; i < 100; i++) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(j, i) = 255;
			}
		}
		for (i = 50; i > 0; i--) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(i, j) = 255;//访问像素点用at
			}
		}
		roi = image(Rect(30, 2600, 200, 200));//Rect的参数有x,y,width,height,101-4
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
		lf2.x = minloc.x + 30;
		lf2.y = minloc.y + 2600 + shape.rows;
		//cout << "n1_lf2:" << lf2 << endl;//输出左下角坐标

		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = lf2.x;
		rect.y = 0;
		rect.width = image.cols - lf2.x;
		rect.height = lf2.y;
		mask(rect).setTo(255);

	}

	if (currentCol == nCamera && currentRow == 1) { //右上
		for (i = 50; i < 100; i++) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(i, j) = 255;//访问像素点用at
			}
		}
		for (i = 50; i > 0; i--) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(j, i) = 255;//访问像素点用at
			}
		}
		roi = image(Rect(3600, 100, 200, 200));//Rect的参数有x,y,width,height,105-1
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
		br1.x = minloc.x + 3600 + shape.cols;
		br1.y = minloc.y + 100;
		//cout << "n1_br1:" << br1 << endl;//输出右上角坐标

		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = br1.y;
		rect.width = br1.x;
		rect.height = image.rows - br1.y;
		mask(rect).setTo(255);

	}

	if (currentCol == nCamera && currentRow == nPhotographing) { //右下角
		for (i = 50; i > 0; i--) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(i, j) = 255;//访问像素点用at
				shape.at<uchar>(j, i) = 255;//访问像素点用at
			}
		}
		roi = image(Rect(3650, 2650, 200, 200));//Rect的参数有x,y,width,height,105-4
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
		br2.x = minloc.x + 3650 + shape.cols;
		br2.y = minloc.y + 2650 + shape.rows;
		//cout << "n1_br2:" << br2 << endl;//输出右下角坐标

		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = br2.x;
		rect.height = br2.y;
		mask(rect).setTo(255);

	}

	if (currentCol == 1 && 1 < currentRow && currentRow < nPhotographing)//左边
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = lf1.x;
		rect.y = 0;
		rect.width = image.cols - lf1.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (currentCol == nCamera && 1 < currentRow && currentRow< nPhotographing)//右边
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = br1.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (currentRow == 1 && 1 < currentCol && currentCol < nCamera)//上边
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = lf1.y;
		rect.width = image.cols;
		rect.height = image.rows - lf1.y;
		mask(rect).setTo(255);
	}

	if (currentRow == nPhotographing && 1 < currentCol && currentCol < nCamera)//下边
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = image.cols;
		rect.height = lf2.y;
		mask(rect).setTo(255);
	}

	if (1 < currentCol && currentCol < nCamera && 1 < currentRow && currentRow < nPhotographing) {
		mask = Mat::ones(image.size(), CV_8UC1) * 255;//原始掩模
	}
	return mask;
}


void TemplFunc::save(const std::string& path, Mat& image_template_gray) {
	Mat temp;
	cv::pyrDown(image_template_gray, temp);
	cv::pyrDown(temp, temp);
	cv::pyrDown(temp, temp);
	Ptr<SURF> detector = SURF::create(500, 4, 4, true, true);
	detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
	cv::FileStorage store(path, cv::FileStorage::WRITE);
	cv::write(store, "keypoints", keypoints);
	cv::write(store, "descriptors", descriptors);
	store.release();

}
void TemplFunc::load(const std::string& path) {
	cv::FileStorage store(path, cv::FileStorage::READ);
	cv::FileNode n1 = store["keypoints"];
	cv::read(n1, keypoints);
	cv::FileNode n2 = store["descriptors"];
	cv::read(n2, descriptors);
	store.release();
}