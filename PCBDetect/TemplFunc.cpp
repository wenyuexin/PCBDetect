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

//字符串转整数
void TemplFunc::str2int(int &int_temp, const std::string &string_temp)
{
	std::stringstream stream(string_temp);
	stream >> int_temp;
}

/**
*功能：寻找pcb板的左下角的L型标志的大致坐标
*输入：pcb左下角图像image，pcb板左下角的x坐标lf_x和y坐标lf_y
*输出：在pcb板的左下角所截取的L型标志的矩形框的左下角坐标值，返回point_left
*/
Point TemplFunc::corner_lf(cv::Mat &image, int lf_x, int lf_y)
{

	shape = Mat::zeros(Size(100, 100), CV_8UC1);//创建一个像素值全为0的图像，位深8位无符号整数，单通道的灰度图
	for (int i = 50; i < 100; i++)
	{
		for (int j = 50; j < 52; j++)
		{
			shape.at<uchar>(j, i) = 255;
		}
	}
	for (int i = 50; i > 0; i--)
	{
		for (int j = 50; j < 52; j++)
		{
			shape.at<uchar>(i, j) = 255;//访问像素点用at
		}

	}
	roi_shape = shape(Rect(46, 0, 50, 54));//得到模板的感兴趣区域

	roi_x = lf_x;
	roi_y = lf_y - length;//截取距pcb板length距离的矩形
	roi_image = image(Rect(roi_x,roi_y, length, length));//Rect的参数有x,y,width,height,101-4

	cvtColor(roi_image, roi_gray, COLOR_BGR2GRAY);//灰度化
	threshold(roi_gray, roi_bw, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);//二值化
	matchTemplate(roi_bw, roi_shape, match, TM_CCOEFF);//模板匹配
	minMaxLoc(match, NULL, NULL, &minloc, &maxloc);

	//lf2.x = maxloc.x;
	//lf2.y = maxloc.y + 2500 + shape.rows;
	//cout << "201_lf2:" << lf2 << endl;//输出左下角坐标

	point_leftup = maxloc;//左上角（lf）
	point_left.x = point_leftup.x + lf_x;
	point_left.y = point_leftup.y + shape.rows + roi_y;
	return point_left;

}
/**
*功能：寻找pcb板的右上角的L型标志的大致坐标
*输入：pcb板右上角图像image，pcb板左下角的x坐标br_x和y坐标br_y
*输出：在pcb板的左下角所截取的L型标志的矩形框的右上角坐标值，返回point_left
*/
Point TemplFunc::corner_br(cv:: Mat &image, int br_x, int br_y)
{
	shape = Mat::zeros(Size(100, 100), CV_8UC1);//创建一个像素值全为0的图像，位深8位无符号整数，单通道的灰度图
	for (int i = 50; i < 100; i++)
	{
		for (int j = 50; j < 52; j++)
		{
			shape.at<uchar>(i, j) = 255;//访问像素点用at
		}
	}
	for (int i = 50; i > 0; i--)
	{
		for (int j = 50; j < 52; j++)
		{
			shape.at<uchar>(j, i) = 255;//访问像素点用at
		}
	}

	roi_shape = shape(Rect(0, 48, 54, 50));
	//imwrite("roi1_lf2.jpg", roi1);

	roi_x = br_x - length;
	roi_y = br_y;//截取距pcb板length距离的矩形
	roi_image = image(Rect(roi_x, roi_y, length, length));//Rect的参数有x,y,width,height,101-4

	cvtColor(roi_image, roi_gray, COLOR_BGR2GRAY);//灰度化
	threshold(roi_gray, roi_bw, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);//二值化
	matchTemplate(roi_bw, roi_shape, match, TM_CCOEFF);//模板匹配
	minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
	point_rightup = maxloc;//右上角（lf）
	point_right.x = point_rightup.x + shape.cols + roi_x;
	point_right.y = point_rightup.y + br_y;
	return point_right;

}

/**
*功能：根据L型标志（角点位置）对图像进行切割（除去边角）
*输入：图像所在列数num_cols,图像所在行数num_cols,代表图像位置的编号num，图像总列数cols,图像总行数rows,原始图像image，图像左下角和右上角的L型坐标
*输出：对所切割的结果图保存，返回mask
*/
Mat TemplFunc::cutting(int num_cols, int num_rows, int cols, int rows, cv::Mat &image, Point point_left, Point point_right)
{
	if (num_cols == 1 && num_rows == 1)
		/*if (num_cols == 1 && num_rows == 2)*/
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = point_left.x;
		rect.y = point_right.y;
		rect.width = image.cols - point_left.x;
		rect.height = image.rows - point_right.y;
		mask(rect).setTo(255);//制作合适的掩模，矩形区域内像素值为255
	}
	if (num_cols == 1 && num_rows == rows)
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = image.cols - point_left.x;
		rect.height = point_left.y;
		mask(rect).setTo(255);
	}
	if (num_cols == cols && num_rows == 1)
		/*if (num_cols == 3 && num_rows == 2)*/
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = point_right.x;
		rect.height = image.rows - point_right.y;
		mask(rect).setTo(255);
	}
	if (num_cols == cols && num_rows == rows)
		/*if (num_cols == 3 && num_rows == rows)*/
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = point_left.y;
		mask(rect).setTo(255);
	}
	if (num_cols == 1 && 1 < num_rows && num_rows < rows)
		/*if (num_cols == 1 && 2 < num_rows && num_rows < rows)*/
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = image.cols - point_left.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (num_cols == cols && 1 < num_rows && num_rows < rows)
		//if (num_cols == 3 && 2 < num_rows && num_rows < rows)
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (num_rows == 1 && 1 < num_cols && num_cols < cols)
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = image.cols;
		rect.height = image.rows - point_right.y;
		mask(rect).setTo(255);
	}

	if (num_rows == rows && 1 < num_cols && num_cols < cols)
	{
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = image.cols;
		rect.height = point_left.y;
		mask(rect).setTo(255);
	}

	if (1 < num_cols && num_cols < cols && 1 < num_rows && num_rows < rows)
	{
		mask = Mat::ones(image.size(), CV_8UC1)*255;//原始掩模
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