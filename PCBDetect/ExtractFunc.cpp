#include "ExtractFunc.h"
#include <qDebug>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

using pcb::UserConfig;
using pcb::RuntimeParams;
using std::string;
using cv::Mat;
using cv::Size;
using cv::Rect;
using cv::Ptr;
using cv::xfeatures2d::SURF;
using cv::xfeatures2d::SIFT;


ExtractFunc::ExtractFunc()
{
	adminConfig = Q_NULLPTR;
	userConfig = Q_NULLPTR;
	runtimeParams = Q_NULLPTR;
}

ExtractFunc::~ExtractFunc()
{
	qDebug() << "~ExtractFunc";
}

/***************** 提取 ******************/

//生成完整尺寸的模板图像
void ExtractFunc::generateBigTempl()
{
	Size originalfullImgSize(adminConfig->ImageSize_W * runtimeParams->nCamera,
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); //整图的原始尺寸
	big_templ = Mat(originalfullImgSize, CV_8UC3);
}


/*
 * 功能：通过图像匹配找到图像的四个角点的位置，分别为左上，左下，右上，右下,并根据角点位置对图像进行切割（除去边角）
 * 输入：图像所在列数num_cols,图像所在行数num_cols,代表图像位置的编号num，原始图像image
 * 输出：显示L型角点位置的坐标，并对所切割的结果图保存，返回mask
 */
//Mat ExtractFunc::findLocationMark(int col, Mat &image) {
//	int currentCol = col + 1;
//	int currentRow = runtimeParams->currentRow_extract + 1;
//	int nCamera = runtimeParams->nCamera;
//	int nPhotographing = runtimeParams->nPhotographing;
//
//	shape = Mat::zeros(Size(100, 100), CV_8UC1);//创建一个像素值全为0的图像，位深8位无符号整数，单通道的灰度图
//
//	if (currentCol == 1 && currentRow == 1) { //左上
//		for (i = 10; i < 60; i++) {
//			for (j = 10; j < 12; j++) {
//				shape.at<uchar>(i, j) = 255;//访问像素点用at
//				shape.at<uchar>(j, i) = 255;
//			}
//		}
//		roi = image(Rect(100, 100, 200, 200));//Rect的参数有x,y,width,height,101-1,称作匹配的感兴趣区域，roi不要写成定值
//		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
//		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
//		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配，TM_CCOEFF系数匹配法，相关值
//		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);//求得匹配图的最小值，最大值的位置
//		lf1.x = maxloc.x + 100;//在原始图像中的位置
//		lf1.y = maxloc.y + 100;
//		//cout << "n1_lf1:" << lf1 << endl;//输出左上角坐标
//
//		//得到感兴趣区域
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = lf1.x;
//		rect.y = lf1.y;
//		rect.width = image.cols - lf1.x;
//		rect.height = image.rows - lf1.y;
//		mask(rect).setTo(255);//制作合适的掩模，矩形区域内像素值为255	
//	}
//
//	if (currentCol == 1 && currentRow == nPhotographing) { //左下
//		for (i = 50; i < 100; i++) {
//			for (j = 50; j < 52; j++) {
//				shape.at<uchar>(j, i) = 255;
//			}
//		}
//		for (i = 50; i > 0; i--) {
//			for (j = 50; j < 52; j++) {
//				shape.at<uchar>(i, j) = 255;//访问像素点用at
//			}
//		}
//		roi = image(Rect(30, 2600, 200, 200));//Rect的参数有x,y,width,height,101-4
//		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
//		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
//		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配
//		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
//		lf2.x = minloc.x + 30;
//		lf2.y = minloc.y + 2600 + shape.rows;
//		//cout << "n1_lf2:" << lf2 << endl;//输出左下角坐标
//
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = lf2.x;
//		rect.y = 0;
//		rect.width = image.cols - lf2.x;
//		rect.height = lf2.y;
//		mask(rect).setTo(255);
//
//	}
//
//	if (currentCol == nCamera && currentRow == 1) { //右上
//		for (i = 50; i < 100; i++) {
//			for (j = 50; j < 52; j++) {
//				shape.at<uchar>(i, j) = 255;//访问像素点用at
//			}
//		}
//		for (i = 50; i > 0; i--) {
//			for (j = 50; j < 52; j++) {
//				shape.at<uchar>(j, i) = 255;//访问像素点用at
//			}
//		}
//		roi = image(Rect(3600, 100, 200, 200));//Rect的参数有x,y,width,height,105-1
//		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
//		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
//		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配
//		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
//		br1.x = minloc.x + 3600 + shape.cols;
//		br1.y = minloc.y + 100;
//		//cout << "n1_br1:" << br1 << endl;//输出右上角坐标
//
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = 0;
//		rect.y = br1.y;
//		rect.width = br1.x;
//		rect.height = image.rows - br1.y;
//		mask(rect).setTo(255);
//
//	}
//
//	if (currentCol == nCamera && currentRow == nPhotographing) { //右下角
//		for (i = 50; i > 0; i--) {
//			for (j = 50; j < 52; j++) {
//				shape.at<uchar>(i, j) = 255;//访问像素点用at
//				shape.at<uchar>(j, i) = 255;//访问像素点用at
//			}
//		}
//		roi = image(Rect(3650, 2650, 200, 200));//Rect的参数有x,y,width,height,105-4
//		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
//		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
//		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//模板匹配
//		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
//		br2.x = minloc.x + 3650 + shape.cols;
//		br2.y = minloc.y + 2650 + shape.rows;
//		//cout << "n1_br2:" << br2 << endl;//输出右下角坐标
//
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = 0;
//		rect.y = 0;
//		rect.width = br2.x;
//		rect.height = br2.y;
//		mask(rect).setTo(255);
//
//	}
//
//	if (currentCol == 1 && 1 < currentRow && currentRow < nPhotographing)//左边
//	{
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = lf1.x;
//		rect.y = 0;
//		rect.width = image.cols - lf1.x;
//		rect.height = image.rows;
//		mask(rect).setTo(255);
//	}
//
//	if (currentCol == nCamera && 1 < currentRow && currentRow< nPhotographing)//右边
//	{
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = 0;
//		rect.y = 0;
//		rect.width = br1.x;
//		rect.height = image.rows;
//		mask(rect).setTo(255);
//	}
//
//	if (currentRow == 1 && 1 < currentCol && currentCol < nCamera)//上边
//	{
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = 0;
//		rect.y = lf1.y;
//		rect.width = image.cols;
//		rect.height = image.rows - lf1.y;
//		mask(rect).setTo(255);
//	}
//
//	if (currentRow == nPhotographing && 1 < currentCol && currentCol < nCamera)//下边
//	{
//		mask = Mat::zeros(image.size(), image.type());//原始掩模
//		Rect rect;
//		rect.x = 0;
//		rect.y = 0;
//		rect.width = image.cols;
//		rect.height = lf2.y;
//		mask(rect).setTo(255);
//	}
//
//	if (1 < currentCol && currentCol < nCamera && 1 < currentRow && currentRow < nPhotographing) {
//		mask = Mat::ones(image.size(), CV_8UC1) * 255;//原始掩模
//	}
//	return mask;
//}


void ExtractFunc::save(const std::string& path, Mat& image_template_gray, bool usingSURF) {
	//Mat temp;
	//cv::pyrDown(image_template_gray, temp);
	//cv::pyrDown(temp, temp);
	//if (userConfig->matchingAccuracyLevel == 2)//低精度
	//	cv::pyrDown(temp, temp);
	//Ptr<SURF> detector = SURF::create(100, 4, 4, true, true);
	//detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
	//cv::FileStorage store(path, cv::FileStorage::WRITE);
	//cv::write(store, "keypoints", keypoints);
	//cv::write(store, "descriptors", descriptors);
	//store.release();

	Mat temp;
	cv::pyrDown(image_template_gray, temp);
	if (userConfig->matchingAccuracyLevel == 2)//低精度
	{
		cv::pyrDown(temp, temp);
	}
	if (usingSURF) {
		Ptr<SURF> detector = SURF::create(100, 4, 4, true, true);
		detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
	}
	else {
		Ptr<SIFT> detector = SIFT::create();
		detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
	}

	cv::FileStorage store(path, cv::FileStorage::WRITE);
	cv::write(store, "keypoints", keypoints);
	cv::write(store, "descriptors", descriptors);
	store.release();

}

void ExtractFunc::load(const std::string& path) {
	cv::FileStorage store(path, cv::FileStorage::READ);
	cv::FileNode n1 = store["keypoints"];
	cv::read(n1, keypoints);
	cv::FileNode n2 = store["descriptors"];
	cv::read(n2, descriptors);
	store.release();
}

void ExtractFunc::str2int(int &int_temp, const std::string &string_temp) {
	std::stringstream stream(string_temp);
	stream >> int_temp;
}

/**
*功能：寻找pcb板的左下角的L型标志的大致坐标
*输入：pcb左下角图像image，pcb板左下角的x坐标lf_x和y坐标lf_y
*输出：在pcb板的左下角所截取的L型标志的矩形框的左下角坐标值，返回point_left
*/
cv::Point ExtractFunc::corner_lf(cv::Mat &image, int lf_x, int lf_y) {
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
	//imwrite("roi1_lf2.jpg", roi1);

	roi_x = lf_x;
	roi_y = lf_y - length;//截取距pcb板length距离的矩形
	roi_image = image(Rect(roi_x, roi_y, length, length));//Rect的参数有x,y,width,height,101-4

	cvtColor(roi_image, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
	threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
	matchTemplate(roi_bw, roi_shape, match, cv::TM_CCOEFF);//模板匹配
	minMaxLoc(match, NULL, NULL, &minloc, &maxloc);

	//lf2.x = maxloc.x;
	//lf2.y = maxloc.y + 2500 + shape.rows;
	//cout << "201_lf2:" << lf2 << endl;//输出左下角坐标
	left = maxloc;//左上角（lf）
	right.x = left.x + shape.cols;//右下角(br)
	right.y = left.y + shape.rows;
	rectangle(roi_image, left, right, (255, 255, 255), 2);//画矩形
	//imwrite("E:\\project\\left.jpg", roi_image);


	point_leftup = maxloc;//左上角（lf）
	//point_leftdown.x = point_leftup.x ;//左下角(br)
	//point_leftdown.y = point_leftup.y + shape.rows;
	point_left.x = point_leftup.x + lf_x;
	point_left.y = point_leftup.y + shape.rows + roi_y;
	return point_left;
}


/**
*功能：寻找pcb板的右上角的L型标志的大致坐标
*输入：pcb板右上角图像image，pcb板左下角的x坐标br_x和y坐标br_y
*输出：在pcb板的左下角所截取的L型标志的矩形框的右上角坐标值，返回point_left
*/
cv::Point  ExtractFunc::corner_br(cv::Mat &image, int br_x, int br_y) {
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

	cvtColor(roi_image, roi_gray, cv::COLOR_BGR2GRAY);//灰度化
	threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//二值化
	matchTemplate(roi_bw, roi_shape, match, cv::TM_CCOEFF);//模板匹配
	minMaxLoc(match, NULL, NULL, &minloc, &maxloc);

	//lf2.x = maxloc.x;
	//lf2.y = maxloc.y + 2500 + shape.rows;
	//cout << "201_lf2:" << lf2 << endl;//输出左下角坐标
	left = maxloc;//左上角（lf）
	right.x = left.x + shape.cols;//右下角(br)
	right.y = left.y + shape.rows;
	rectangle(roi_image, left, right, (255, 255, 255), 2);//画矩形
	//imwrite("E:\\project\\right.jpg", roi_image);


	point_rightup = maxloc;//右上角（lf）
	//point_leftdown.x = point_leftup.x ;//左下角(br)
	//point_leftdown.y = point_leftup.y + shape.rows;
	point_right.x = point_rightup.x + shape.cols + roi_x;
	point_right.y = point_rightup.y + br_y;
	return point_right;
}

cv::Mat ExtractFunc::cutting(int num_cols, int num_rows, int cols, int rows, cv::Mat &image, cv::Point point_left, cv::Point point_right) {
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
		mask = Mat::zeros(image.size(), image.type());//原始掩模
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = image.cols;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	return mask;
}