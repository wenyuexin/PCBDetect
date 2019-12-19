#include "DetectFunc.h"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <algorithm>

using pcb::UserConfig;
using pcb::RuntimeParams;
using std::vector;
using std::string;
using std::cout;
using std::to_string;
using cv::Mat;
using cv::Point;
using cv::Point2i;
using cv::Point2f;
using cv::Vec4i;
using cv::Rect;
using cv::Size;
using cv::Scalar;
using cv::Ptr;
using cv::KeyPoint;
using cv::DMatch;
using cv::FlannBasedMatcher;
using cv::xfeatures2d::SURF;


#define M_PI       3.14159265358979323846   // pi


DetectFunc::DetectFunc()
{
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
}

DetectFunc::~DetectFunc()
{
	qDebug() << "~DetectFunc";
}



bool DetectFunc::alignImages_test_load(vector<KeyPoint> &keypoints_1, Mat& descriptors_1, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches)
{

	Ptr<SURF> detector = SURF::create(100, 4, 4, true, true);
	std::vector<KeyPoint> keypoints_2;
	Mat descriptors_2;

	double t1 = clock();
	cv::Mat pyr;
	cv::Size sz = image_sample_gray.size();
	
	pyrDown(image_sample_gray, pyr);
	if (userConfig->matchingAccuracyLevel == 2)//低精度
	{
		pyrDown(pyr, pyr);
	}
     

	detector->detectAndCompute(pyr, Mat(), keypoints_2, descriptors_2);

	double t2 = clock();
	cout << "获取特征点时间" << double(t2 - t1) / CLOCKS_PER_SEC << endl;

	Ptr<cv::flann::IndexParams> indexParams = new cv::flann::KDTreeIndexParams(5);
	Ptr<cv::flann::SearchParams> searchParams;
	FlannBasedMatcher matcher(indexParams);
	vector<DMatch> matches;
	vector<vector<DMatch>> m_knnMatches;

	/*const float minRatio = 1.f / 1.5f;*/
	const float minRatio = 0.7;
	matcher.knnMatch(descriptors_1, descriptors_2, m_knnMatches, 2);



	////Mat outImg;
	////drawKeypoints(image_template, keypoints_1, image_template,cv::Scalar::all(-1),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	////imwrite("outImg.jpg", image_template);

	std::sort(m_knnMatches.begin(), m_knnMatches.end(), [](const vector<DMatch> &m1, const vector<DMatch> &m2) {return m1[0].distance < m2[0].distance; });

	for (int i = 0; i < m_knnMatches.size(); i++)
	{
		const DMatch& bestMatch = m_knnMatches[i][0];
		const DMatch& betterMatch = m_knnMatches[i][1];

		if (bestMatch.distance < 0.7*betterMatch.distance)
		{
			matches.push_back(bestMatch);
		}
	}

	vector< DMatch > good_matches;

	if (!matches.size())
	{
		cout << "matches is empty! " << endl;

	}
	else if (matches.size() < 4)
	{
		cout << matches.size() << " points matched is not enough " << endl;
	}

	else //单应性矩阵的计算最少得使用4个点
	{

		for (int i = 0; i < matches.size(); i++)
		{
			good_matches.push_back(matches[i]);
		}

		vector<Point2f> temp_points;
		vector<Point2f> samp_points;

		for (int i = 0; i < matches.size(); i++)
		{
			temp_points.push_back(keypoints_1[matches[i].queryIdx].pt);
			samp_points.push_back(keypoints_2[matches[i].trainIdx].pt);
		}

		double t3 = clock();
		cout << "匹配并获取变换矩阵时间" << double(t3 - t2) / CLOCKS_PER_SEC << endl;


		H = findHomography(samp_points, temp_points, cv::RANSAC, 5.0);

		int matrixAdj = 2 * (userConfig->matchingAccuracyLevel);
		H.at<double>(0, 2) *= matrixAdj;
		H.at<double>(1, 2) *= matrixAdj;
		H.at<double>(2, 0) /= matrixAdj;
		H.at<double>(2, 1) /= matrixAdj;

		warpPerspective(image_sample_gray, imgReg, H, image_sample_gray.size());
	}

	return true;
}

Mat DetectFunc::myThresh(int curCol, int curRow, const cv::Mat & grayImg, cv::Point point_left, cv::Point point_right)
{
	int totalCol = runtimeParams->nCamera - 1;//从0开始
	int totalRow = runtimeParams->nPhotographing - 1;
	Mat res = Mat::zeros(grayImg.size(), CV_8UC1);
	Rect rect;//roi区域
	if (curCol == 0 && curRow == 0)//左上
	{
		rect.x = point_left.x;
		rect.y = point_right.y;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows - point_right.y;

	}
	else if (curCol == 0 && curRow == totalRow)//左下
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = point_left.y;
	}

	else if (curCol == totalCol && curRow == 0)//右上
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = point_right.x;
		rect.height = grayImg.rows - point_right.y;
	}
	else if (curCol == totalCol && curRow == totalRow)//右下
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = point_left.y;
	}
	else if (curCol == 0 && 0 < curRow && curRow < totalRow)//左边
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows;
	}

	else if (curCol == totalCol && 0 < curRow && curRow < totalRow)//右边
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = grayImg.rows;
	}

	else if (curRow == 0 && 0 < curCol && curCol < totalCol)//上边
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = grayImg.cols;
		rect.height = grayImg.rows - point_right.y;
	}

	else if (curRow == totalRow && 0 < curCol && curCol < totalCol)//下边
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = grayImg.cols;
		rect.height = point_left.y;
	}
	else if (0 < curCol && curCol < totalCol && 0 < curRow && curRow < totalRow)//中央
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = grayImg.cols;
		rect.height = grayImg.rows;
	}

	//int longSize = std::max(rect.width, rect.height);
	//int blockSize = longSize / 4 * 2 + 1;
	//cv::adaptiveThreshold(grayImg(rect), res(rect), 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, blockSize, 0);

	int revise = 20;
	int grayImgMean = mean(grayImg(rect))[0];
	cv::threshold(grayImg(rect), res(rect), grayImgMean, 255, cv::THRESH_BINARY);

	//Mat grayImgCopy = grayImg.clone();
	//int threshold_otsu = cv::threshold(grayImgCopy(rect), res(rect), 150, 255, cv::THRESH_BINARY|cv::THRESH_OTSU) + 10;
	//cv::threshold(grayImg(rect), res(rect), threshold_otsu , 255, cv::THRESH_BINARY);
	return res;
}

cv::Rect DetectFunc::getRect(int curCol, int curRow, const cv::Mat& grayImg, cv::Point point_left, cv::Point point_right)
{
	int totalCol = runtimeParams->nCamera - 1;//从0开始
	int totalRow = runtimeParams->nPhotographing - 1;
	Mat res = Mat::zeros(grayImg.size(), CV_8UC1);
	Rect rect;//roi区域
	if (curCol == 0 && curRow == 0)//左上
	{
		rect.x = point_left.x;
		rect.y = point_right.y;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows - point_right.y;

	}
	else if (curCol == 0 && curRow == totalRow)//左下
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = point_left.y;
	}

	else if (curCol == totalCol && curRow == 0)//右上
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = point_right.x;
		rect.height = grayImg.rows - point_right.y;
	}
	else if (curCol == totalCol && curRow == totalRow)//右下
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = point_left.y;
	}
	else if (curCol == 0 && 0 < curRow && curRow < totalRow)//左边
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows;
	}

	else if (curCol == totalCol && 0 < curRow && curRow < totalRow)//右边
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = grayImg.rows;
	}

	else if (curRow == 0 && 0 < curCol && curCol < totalCol)//上边
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = grayImg.cols;
		rect.height = grayImg.rows - point_right.y;
	}

	else if (curRow == totalRow && 0 < curCol && curCol < totalCol)//下边
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = grayImg.cols;
		rect.height = point_left.y;
	}
	else if (0 < curCol && curCol < totalCol && 0 < curRow && curRow < totalRow)//中央
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = grayImg.cols;
		rect.height = grayImg.rows;
	}

	return rect;
}

bool  DetectFunc::alignImagesECC(Mat &image_template_gray, Mat &image_sample_gray, Mat &imgReg, Mat &warp_matrix) {
	Mat pyrTemp, pyrSamp;
	pyrDown(image_template_gray, pyrTemp);
	pyrDown(pyrTemp, pyrTemp);
	pyrDown(pyrTemp, pyrTemp);
	pyrDown(pyrTemp, pyrTemp);

	pyrDown(image_sample_gray, pyrSamp);
	pyrDown(pyrSamp, pyrSamp);
	pyrDown(pyrSamp, pyrSamp);
	pyrDown(pyrSamp, pyrSamp);

	// Define the motion model 定义运动模型
	const int warp_mode =cv:: MOTION_EUCLIDEAN;

	// Set a 2x3 or 3x3 warp matrix depending on the motion model. 变换矩阵
	//Mat warp_matrix;

	// Initialize the matrix to identity
	if (warp_mode ==cv:: MOTION_HOMOGRAPHY)
	{
		warp_matrix = Mat::eye(3, 3, CV_32F);
	}
	else
	{
		warp_matrix = Mat::eye(2, 3, CV_32F);
	}
	imwrite("D:\\0000000_project\\000_PCBData_0729\\PCBData\\warp_matrix.bmp", warp_matrix);
	// Specify the number of iterations. 算法迭代次数
	int number_of_iterations = 5000;

	// Specify the threshold of the increment
	// in the correlation coefficient between two iterations 设定阈值
	double termination_eps = 1e-10;

	// Define termination criteria 定义终止条件
	cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, number_of_iterations, termination_eps);

	// Run the ECC algorithm. The results are stored in warp_matrix. ECC算法
	//imwrite("D:\\0000000_project\\000_PCBData_0729\\PCBData\\image_template_gray.bmp", image_template_gray);
	//imwrite("D:\\0000000_project\\000_PCBData_0729\\PCBData\\image_sample_gray.bmp", image_sample_gray);
	int typeTemplate = image_template_gray.type();
	int typeSample = image_sample_gray.type();
	try {
		findTransformECC(pyrSamp, pyrTemp, warp_matrix, warp_mode, criteria, cv::noArray());
	}
	catch (cv::Exception& e) {
		const char* err_msg = e.what();
		cout << err_msg;
		return false;
	}

	// Storage for warped image.

	if (warp_mode != cv::MOTION_HOMOGRAPHY)
	{
		// Use warpAffine for Translation, Euclidean and Affine
		warpAffine(image_sample_gray, imgReg, warp_matrix, image_template_gray.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);
	}
	else
	{
		// Use warpPerspective for Homography
		warpPerspective(image_sample_gray, imgReg, warp_matrix, image_template_gray.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);
	}

	// Show final result
	//imshow("Image 1", im1);
	//imshow("Image 2", im2);
	//imshow("Image 2 Aligned", im2_aligned);
	//waitKey(0);

	return true;
}

bool DetectFunc::alignImages_test(Mat &image_template_gray, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches) {
	/*Ptr<SURF> detector = SURF::create(3500, 4, 3, true, true);*/
	Ptr<SURF> detector = SURF::create(100, 4, 4, true, true);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;

	double t1 = clock();
	Mat pyrTemp, pyrSamp;
	pyrDown(image_template_gray, pyrTemp);
	pyrDown(pyrTemp, pyrTemp);
	if(userConfig->matchingAccuracyLevel==2)//低精度
		pyrDown(pyrTemp, pyrTemp);	

	pyrDown(image_sample_gray, pyrSamp);
	pyrDown(pyrSamp, pyrSamp);
	if (userConfig->matchingAccuracyLevel==2)//低精度
		pyrDown(pyrSamp, pyrSamp);

	detector->detectAndCompute(pyrTemp, Mat(), keypoints_1, descriptors_1);
	detector->detectAndCompute(pyrSamp, Mat(), keypoints_2, descriptors_2);

	//detector->detectAndCompute(image_template_gray, Mat(), keypoints_1, descriptors_1);
	//detector->detectAndCompute(image_sample_gray, Mat(), keypoints_2, descriptors_2);

	double t2 = clock();
	cout << "获取特征点时间" << double(t2 - t1) / CLOCKS_PER_SEC << endl;

	Ptr<cv::flann::IndexParams> indexParams = new cv::flann::KDTreeIndexParams(5);
	Ptr<cv::flann::SearchParams> searchParams;
	FlannBasedMatcher matcher(indexParams);
	vector<DMatch> matches;
	vector<vector<DMatch>> m_knnMatches;

	const float minRatio = 0.7;
	matcher.knnMatch(descriptors_1, descriptors_2, m_knnMatches, 2);


	std::sort(m_knnMatches.begin(), m_knnMatches.end(), [](const vector<DMatch> &m1, const vector<DMatch> &m2) {return m1[0].distance < m2[0].distance; });

	for (int i = 0; i < m_knnMatches.size(); i++)
	{
		const DMatch& bestMatch = m_knnMatches[i][0];
		const DMatch& betterMatch = m_knnMatches[i][1];

		if (bestMatch.distance < 0.7*betterMatch.distance)
		{
			matches.push_back(bestMatch);
		}
	}
	//-- Draw matches
	//Mat img_matches;
	//drawMatches(image_template_gray, keypoints_1, image_sample_gray, keypoints_2, matches, img_matches, Scalar::all(-1),
	//	Scalar::all(-1), std::vector<char>(),cv:: DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//imwrite("D:\\PCBData\\drawMatches.jpg", img_matches);


	if (!matches.size())
	{
		cout << "matches is empty! " << endl;

	}
	else if (matches.size() < 4)
	{
		cout << matches.size() << " points matched is not enough " << endl;
	}

	else 
	{
		vector<Point2f> temp_points;
		vector<Point2f> samp_points;

		for (int i = 0; i < matches.size(); i++)
		{
			temp_points.push_back(keypoints_1[matches[i].queryIdx].pt);
			samp_points.push_back(keypoints_2[matches[i].trainIdx].pt);
		}
		double t3 = clock();
		cout << "匹配获取变换矩阵时间" << double(t3 - t2) / CLOCKS_PER_SEC << endl;

		H = findHomography(samp_points, temp_points, cv::RANSAC, 5.0);
		int matrixAdj = 4 * (userConfig->matchingAccuracyLevel);
		H.at<double>(0, 2) *= matrixAdj;
		H.at<double>(1, 2) *= matrixAdj;
		H.at<double>(2, 0) /= matrixAdj;
		H.at<double>(2, 1) /= matrixAdj; 
		warpPerspective(image_sample_gray, imgReg, H, image_sample_gray.size());

		/*H = Mat::eye(3, 3, CV_8U);*/
		//H = findHomography(samp_points, temp_points, cv::RANSAC, 5.0);
		//std::vector<Point2f> obj_corners(4);
		//obj_corners[0] = Point2f(0, 0);
		//obj_corners[1] = Point2f(0, image_template_gray.rows - 1);
		//obj_corners[2] = Point2f(image_template_gray.cols - 1, image_template_gray.rows - 1);
		//obj_corners[3] = Point2f(image_template_gray.cols - 1, 0);
		//std::vector<Point2f> scene_corners(4);
		//perspectiveTransform(obj_corners, scene_corners,H);
		//Mat warpPerspective_mat(3, 3, CV_8U);
		//warpPerspective_mat = getPerspectiveTransform(scene_corners, obj_corners);
		//warpPerspective(image_sample_gray, imgReg, warpPerspective_mat, image_sample_gray.size());
	}
	return true;
}

/**
 *先进行阈值处理，再进行差值处理
 */
Mat DetectFunc::sub_process_new(Mat &templBw, Mat &sampBw, Mat& mask_roi, Mat& directFlaw, Mat& MorphFlaw,Mat& cannyFlaw) {
	Mat imgFlaw;
	
	cv::absdiff(templBw, sampBw, imgFlaw);
	directFlaw = imgFlaw.clone();
	bitwise_and(imgFlaw, mask_roi, imgFlaw);
	

	//对差值图像做形态学处理，先开后闭，这里的处理与最小线宽有关
	cv::Mat element_a = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);
	MorphFlaw = imgFlaw.clone();

	////膨胀模板边缘，与形态学处理后的图片相乘，获取边界上的点消除
	//cv::Mat edges;
	//cv::Canny(templBw, edges, 150, 50);
	//cv::Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::dilate(edges, edges, element_b);
	//edges = 255 - edges;
	//cv::bitwise_and(edges, imgFlaw, imgFlaw);
	//cannyFlaw = imgFlaw.clone();


	////再进行一次形态学处理
	//cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	//cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);


	return imgFlaw;
}

cv::Mat DetectFunc::sub_process_new_small(cv::Mat & templBw, cv::Mat & sampBw, cv::Mat & mask_roi)
{
	Mat imgFlaw;
	cv::absdiff(templBw, sampBw, imgFlaw);
	int meanSamp = mean(imgFlaw)[0] + 25;
	threshold(imgFlaw, imgFlaw, meanSamp, 255, cv::THRESH_BINARY);
	bitwise_and(imgFlaw, mask_roi, imgFlaw);
	cv::Mat element_a = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);

	return imgFlaw;
}

cv::Mat DetectFunc::sub_process_direct(cv::Mat & templBw, cv::Mat & sampBw, cv::Mat & templGray, cv::Mat & sampGray, cv::Mat & mask_roi)
{

	Mat imgFlaw;
	cv::absdiff(templGray, sampGray, imgFlaw);
	cv::threshold(imgFlaw, imgFlaw, 30, 200, cv::THRESH_BINARY);
	bitwise_and(imgFlaw, mask_roi, imgFlaw);

	//对差值图像做形态学处理，先开后闭，这里的处理与最小线宽有关
	cv::Mat element_a = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);

	////膨胀模板边缘，与形态学处理后的图片相乘，获取边界上的点消除
	cv::Mat edges;
	cv::Canny(templBw, edges, 150, 50);
	cv::Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(edges, edges, element_b);
	edges = 255 - edges;
	cv::bitwise_and(edges, imgFlaw, imgFlaw);


	//再进行一次形态学处理
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);


	return imgFlaw;
}


/**
*	功能：标记并存储缺陷图片和位置信息
*	输入：
*		diffBw: 差值图像二值图
*		sampGrayReg:配准后的样本灰度图
*		templBw: 模板二值图像
*		templGray:模板灰度图
*       defectNum:缺陷序号
*       currentCol:检测的列
*/
cv::Mat DetectFunc::markDefect_test(int currentCol, Mat &diffBw, Mat &sampGrayReg, double scalingFactor, Mat &templBw, Mat &templGray,Mat sampBw, int &defectNum, std::map<cv::Point3i, cv::Mat, cmp_point3i> &detailImage,cv::Mat rectBlack) {
	Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//threshold(diffBw, diffBw, 0, 255, cv::THRESH_BINARY);
	//dilate(diffBw, diffBw, kernel_small);//对差值图像做膨胀，方便对类型进行判断
	
	//模板二值图边缘做平滑处理
	medianBlur(templBw, templBw, 3);
	blur(templBw, templBw, Size(3, 3));
	threshold(templBw, templBw, 0, 255, cv::THRESH_BINARY);

	cv::Size originalSubImageSize = sampGrayReg.size(); //分图的原始尺寸
	cv::Size scaledSubImageSize(originalSubImageSize.width*scalingFactor, originalSubImageSize.height*scalingFactor); //放缩后的分图尺寸

	Mat sampGrayRegCopy = sampGrayReg.clone();
	cvtColor(sampGrayRegCopy, sampGrayRegCopy, cv::COLOR_GRAY2BGR);
	cv::Mat sampGrayRegCopyZoom;
	cv::resize(sampGrayRegCopy, sampGrayRegCopyZoom, scaledSubImageSize, (0, 0), (0, 0), cv::INTER_LINEAR);

	Mat diffBw_copy = diffBw.clone();
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i>   hierarchy;
	cv::findContours(diffBw_copy, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//如果存在缺陷，则按照给定PCB型号，批次号，编号建立目录，存储图片,不存在缺陷则结束
	if (contours.size() == 0) {
		return sampGrayRegCopyZoom;//不存在缺陷，直接返回缩放后的样本
	}

	for (int i = 0; i < contours.size(); i++) {
		int conArea = contourArea(contours[i], false);
		if (conArea <= 2) continue; //缺陷最小面积
		
		//轮廓外接矩形rect_out为轮廓外接矩形往外括20个像素
		Rect rectCon = boundingRect(Mat(contours[i]));
		int larger_width = 10;//稍微扩大缺陷所在的矩形区域
		int larger_height = 10;
		Rect rect_out = Rect(rectCon.x - larger_width, rectCon.y - larger_height, rectCon.width + 2 * larger_width, rectCon.height + 2 * larger_height);
		Mat temp_area;
		Mat samp_area;
		Mat tempBwPart;
		Mat diffBwPart;


		//防止分图越界
		int width_sub = 0, height_sub = 0;
		if (rect_out.x < 0)
			rect_out.x = 0;
		if (rect_out.y < 0)
			rect_out.y = 0;
		if ((rect_out.x + 2 * larger_width + rectCon.width) > (sampGrayReg.cols - 1)) {
			width_sub = sampGrayReg.cols - 1 - rect_out.x - rectCon.width;
			larger_width = (width_sub - 1) / 2;
		}//最后一个减一是由于四舍五入的误差造成的
		if ((rect_out.y + 2 * larger_height + rectCon.height) > (sampGrayReg.rows - 1)) {
			height_sub = sampGrayReg.rows - 1 - rect_out.y - rectCon.height;
			larger_height = (height_sub - 1) / 2;
		}
		rect_out = Rect(rect_out.x, rect_out.y, rectCon.width + 2 * larger_width, rectCon.height + 2 * larger_height);

		Point rect_out_rb = Point(rect_out.x + rect_out.width, rect_out.y + rect_out.height);
		int boundary_x = sampGrayReg.cols - 1;
		int boundary_y = sampGrayReg.rows - 1;
		if (rect_out.x<0 || rect_out.y <0 || rect_out_rb.x > boundary_x || rect_out_rb.y> boundary_y) {
			cout << "rect_out越界" << endl;
		}

		try {
			temp_area = templGray(rect_out);
			samp_area = sampGrayReg(rect_out);
			tempBwPart = templBw(rect_out).clone();
			diffBwPart = diffBw(rect_out).clone();
		}
		catch (std::exception e) {
			qDebug() << QString::fromLocal8Bit("矩形越界");
		};

		vector<cv::Point2i> locations;
		cv::findNonZero(diffBwPart, locations);

		if (locations.size()<9/* <= 10*/) {
			Size szDiff = diffBwPart.size();
			continue;
		}
		/**************************计算缺陷判断次数*****************************/
		int transNum = 0;
		//寻找状态变换点的个数
		Point2f rectPointsOfPart[4];
		vector<Point> rectPoints;//外接矩形上的点
		vector<Point> change_point;
		int size = 0;
		while (/*change_point.empty()&&*/size<=6) {
			vector<Point> change_point_temp;
			transNum = 0;
			rectPoints.clear();
			//获取缺陷轮廓最小外接矩形上的所有点
			cv::RotatedRect minRectOfPart = cv::minAreaRect(contours[i]);
			minRectOfPart.size += cv::Size2f(size, size);
			size += 2;			
			minRectOfPart.points(rectPointsOfPart);

		/*	for (int i = 0; i < 4; i++) {
				if (abs(rectPointsOfPart[i].x - 2131) < 5&&size==4)
					cout << "test" << endl;
			}*/	
			//Point2i testPoint;
			//for (int i = 0; i < 4; i++) {
			//	if (abs(rectPointsOfPart[i].x - 3701) <3 && size == 4) {
			//		testPoint.x = 3700;
			//		testPoint.y = 3161;
			//		vector<Point2i> neighbors{ testPoint,
			//							Point2i(testPoint.x + 1,testPoint.y),//右
			//							Point2i(testPoint.x - 1,testPoint.y),//左
			//							Point2i(testPoint.x ,testPoint.y + 1),//下
			//							Point2i(testPoint.x ,testPoint.y - 1) };//变化点的右上左下邻域
			//		for (int i = 0; i < 5; i++) {
			//			if ((int)templBw.at<uchar>(neighbors[i]) == 0) {
			//				cout << "test" << endl;
			//			}
			//		}
			//	}
			//		
			//}
			//
			vector<vector<Point>> fourLines(4);//四条线上的点
			for (int i = 0; i < 4; i++) {
				cv::LineIterator it(diffBw, rectPointsOfPart[i], rectPointsOfPart[(i + 1) % 4], 8);
				for (int j = 0; j < it.count; j++, ++it)
				{
					fourLines[i].push_back(it.pos());
				}
				fourLines[i].pop_back();//删除最后一个点
				//将整条线的坐标插入矩形坐标集合
				rectPoints.insert(rectPoints.end(), fourLines[i].begin(), fourLines[i].end());
			}
			rectPoints.push_back(rectPoints[0]);//作用：外接矩形的最后一个点与第一点需要进行比较

			//绘制变换矩形
			for (int j = 0; j < 4; j++)
			{
				line(rectBlack, Point2f(rectPointsOfPart[j].x, rectPointsOfPart[j].y), Point2f(rectPointsOfPart[(j + 1) % 4].x, rectPointsOfPart[(j + 1) % 4].y), (255, 255, 255));
			}

			//寻找变化点
			try {
				auto temp_iterator = rectPoints.begin();
				auto pre_iterator = rectPoints.begin();
				Point pre, temp;
				for (int k = 1; k < rectPoints.size() && temp_iterator != rectPoints.end(); k++)
				{
					pre = rectPoints[k - 1];
					temp = rectPoints[k];
					++temp_iterator;
					/*	if (pre.x == 1940-rect_out.x && pre.y == 965-rect_out.y)
							int a = 0;*/
							/*if (abs(pre.x - 1749) < 5 && abs(pre.y - 2107) < 5)
								std::cout << "test" << endl;*/
					if (transNum < 1 && (int)templBw.at<uchar>(pre) != (int)templBw.at<uchar>(temp))
					{
						if (templBw.at<uchar>(pre) > 0)
						{
							change_point_temp.push_back(pre);//保存变换点
							//change_point.push_back(temp);//保存变换点
							rectBlack.at<cv::Vec3b>(pre) = { 255, 255, 0 };//标记变换点
						}
						else if (templBw.at<uchar>(temp) > 0)
						{
							change_point_temp.push_back(temp);//保存变换点
							//change_point.push_back(temp);//保存变换点
							rectBlack.at<cv::Vec3b>(temp) = { 255, 255, 0 };//标记变换点
						}
						++transNum;
						pre_iterator = temp_iterator;
					/*	if (pre.x == 3021 && pre.y == 925)
							cout << "test" << endl;*/

							/*cout << i << endl;*/
							//cout << pre.x + rect_out.x << " " << pre.y + rect_out.y << endl;

					}
					else if (transNum >= 1 && (int)templBw.at<uchar>(pre) != (int)templBw.at<uchar>(temp) && (temp_iterator - pre_iterator) > 5)
					{
						if (templBw.at<uchar>(pre) > 0)
						{
							change_point_temp.push_back(pre);//保存变换点
							//change_point.push_back(temp);//保存变换点
							rectBlack.at<cv::Vec3b>(pre) = { 255, 255, 0 };//标记变换点
						}
						else if (templBw.at<uchar>(temp) > 0)
						{
							change_point_temp.push_back(temp);//保存变换点
							//change_point.push_back(temp);//保存变换点
							rectBlack.at<cv::Vec3b>(temp) = { 255, 255, 0 };//标记变换点
						}

						/*	if (pre.x == 3021 && pre.y == 925)
								cout << "test" << endl;*/
						++transNum;
						pre_iterator = temp_iterator;
					}
				}

				if (change_point_temp.size() > change_point.size()) {
					change_point = change_point_temp;
					transNum = change_point.size();
				}
			}
			catch (std::exception)
			{
				qDebug() << QString::fromLocal8Bit("旋转矩形坐标计算错误");
				//cout << "旋转矩形坐标计算错误";
			}
		}
       



		//获取缺陷最小外接矩形的宽和高，用宽边代表缺陷长度
		double minAreaRect_width, minAreaRect_height, minAreaRect_length;
		minAreaRect_width = sqrt((rectPointsOfPart[1].x - rectPointsOfPart[0].x)*(rectPointsOfPart[1].x - rectPointsOfPart[0].x) + (rectPointsOfPart[1].y - rectPointsOfPart[0].y)*(rectPointsOfPart[1].y - rectPointsOfPart[0].y));
		minAreaRect_height = sqrt((rectPointsOfPart[2].x - rectPointsOfPart[1].x)*(rectPointsOfPart[2].x - rectPointsOfPart[1].x) + (rectPointsOfPart[2].y - rectPointsOfPart[1].y)*(rectPointsOfPart[2].y - rectPointsOfPart[1].y));
		minAreaRect_length = minAreaRect_width > minAreaRect_height ? minAreaRect_width : minAreaRect_height;


		/************根据轮廓中心点位置，判断是否有线路缺失*************/
		cv::Moments m = cv::moments(contours[i]);
		cv::Point2f contour_center = Point2f(static_cast<float>(m.m10 / (m.m00 + 1e-5)),
			static_cast<float>(m.m01 / (m.m00 + 1e-5)));//通过缺陷轮廓矩计算缺陷轮廓中心

		vector<Point2i> neighbors{ Point2i(contour_center.x - 1,contour_center.y - 1),Point2i(contour_center.x,contour_center.y - 1),Point2i(contour_center.x + 1,contour_center.y - 1),
						Point2i(contour_center.x - 1,contour_center.y),Point2i(contour_center.x,contour_center.y),Point2i(contour_center.x + 1,contour_center.y),
						Point2i(contour_center.x - 1,contour_center.y + 1),Point2i(contour_center.x,contour_center.y + 1),Point2i(contour_center.x + 1,contour_center.y + 1)
		};

		int neighbors_sum = 0;
		try {
			for (int i = 0; i < 9; i++) {
				neighbors_sum += (int)templBw.at<uchar>(neighbors[i]) > 0 ? 255 : 0;
			}
		}
		catch (std::exception e) {
			qDebug() << QString::fromLocal8Bit("缺陷中心坐标计算错误");
			//cout << "缺陷中心坐标计算错误";
		}

		


	/********************根据状态变换次数和是否存在缺失对缺陷进行分类**********************************/
	classfinish:
		bool convexDetectFlag = (userConfig->defectTypeToBeProcessed)[2];//凸起检测标志
		bool concaveDetectFlag = (userConfig->defectTypeToBeProcessed)[3];//缺失检测标志

		int lack_flag = 0;//0表示不缺线路，1表示缺线路
		if (neighbors_sum >= 255 * 2) lack_flag = 1;

		int defect_flag = 0;
		//int trans = 0;
		//int boundaryX = sampBw.cols-1;
		//minRectOfPart.size -= cv::Size2f(8, 8);
		//minRectOfPart.points(rectPointsOfPart);
		if (lack_flag) {
			defect_flag = transNum > 2 ? 1 : 2;//断路:缺失
			//for (int i = 0; i < 4; i++) {
			//	for (int j = 1; j < 2; j++) {
			//		Point tempVaule = rectPointsOfPart[(i + 1) % 4];
			//		rectPointsOfPart[(i + 1) % 4].x += j;
			//		if (rectPointsOfPart[(i + 1) % 4].x > boundaryX) { 
			//			trans++;
			//			continue; 
			//		}else if (rectPointsOfPart[(i + 1) % 4].x== boundaryX ||(int)sampBw.at<uchar>(rectPointsOfPart[(i + 1) % 4]) != (int)sampBw.at<uchar>(tempVaule)) {
			//			trans++;
			//		}
			//	}
			//}
			//defect_flag = trans >= 4 ? 1 : 2;//断路:缺失
		}
		else {
			defect_flag = transNum > 2 ? 3 : 4;//短路:凸起
			//for (int i = 0; i < 4; i++) {
			//	for (int j = 1; j < 2; j++) {
			//		Point tempVaule = rectPointsOfPart[(i + 1) % 4];
			//		rectPointsOfPart[(i + 1) % 4].x += j;
			//		if (rectPointsOfPart[(i + 1) % 4].x > boundaryX) {
			//			trans++;
			//			continue;
			//		}else if (rectPointsOfPart[(i + 1) % 4].x == boundaryX || (int)sampBw.at<uchar>(rectPointsOfPart[(i + 1) % 4]) != (int)sampBw.at<uchar>(tempVaule)) {
			//			trans++;
			//		}
			//	}
			//}
			//defect_flag = trans >= 4 ? 3 : 4;//断路:缺失
		}


		////对判断为缺失、凸起的缺陷进行二次判断，避免发生误检，检测为开路、短路
		//if (defect_flag == 2) {
		//	cv::RotatedRect minRect = minRectOfPart;
		//	minRect.size -= cv::Size2f(8, 8);
		//	int isNoLineDrop = 0;
		//	for (int k = 0; k < 2; k++) {
		//		Point2f minRectPoints[4];
		//		minRect.points(minRectPoints);
		//		vector<vector<Point>> minRectFourLines(4);//旋转矩形四条线上的点
		//		for (int i = 0; i < 4; i++) {
		//			cv::LineIterator minIt(diffBw, minRectPoints[i], minRectPoints[(i + 1) % 4], 8);
		//			for (int j = 0; j < minIt.count; j++, ++minIt)
		//			{
		//				minRectFourLines[i].push_back(minIt.pos());
		//			}
		//		}

		//		//获取最小旋转矩阵四条边非线路部分的变化情况，如果存在有一条边有下降的趋势，说明该缺陷为缺失否则为凸起
		//		vector<int> NoLineNumTemp(4, 0);
		//		int count = 0;
		//		for (vector<vector<Point>>::iterator iter = minRectFourLines.begin(); iter != minRectFourLines.end(); ++iter) {
		//			for (int m = 0; m < (*iter).size(); ++m) {
		//				if ((int)sampBw.at<uchar>((*iter)[m]) == 0) {
		//					NoLineNumTemp[count]++;
		//				}
		//			}
		//			count++;
		//		}
		//		vector<int> NoLineNumPre;
		//		NoLineNumPre.insert(NoLineNumPre.end(), NoLineNumTemp.begin(), NoLineNumTemp.end());
		//		for (int i = 0; i < 4; i++) {
		//			if (NoLineNumTemp[i] < NoLineNumPre[i]) {
		//				isNoLineDrop++;
		//			}
		//		}
		//		minRect.size += cv::Size2f(1, 1);
		//	}
		//	if (isNoLineDrop == 0) {
		//		defect_flag = 1;
		//	}
		//}

		//if (defect_flag == 4) {
		//	cv::RotatedRect minRect = minRectOfPart;
		//	minRect.size -= cv::Size2f(8, 8);
		//	int isNoLineRise = 0;
		//	for (int k = 0; k < 2; k++) {
		//		Point2f minRectPoints[4];
		//		minRect.points(minRectPoints);
		//		vector<vector<Point>> minRectFourLines(4);//旋转矩形四条线上的点
		//		for (int i = 0; i < 4; i++) {
		//			cv::LineIterator minIt(diffBw, minRectPoints[i], minRectPoints[(i + 1) % 4], 8);
		//			for (int j = 0; j < minIt.count; j++, ++minIt)
		//			{
		//				minRectFourLines[i].push_back(minIt.pos());
		//			}
		//		}

		//		//获取最小旋转矩阵四条边非线路部分的变化情况，如果存在有一条边有下降的趋势，说明该缺陷为缺失否则为凸起
		//		vector<int> NoLineNumTemp(4, 0);
		//		int count = 0;
		//		for (vector<vector<Point>>::iterator iter = minRectFourLines.begin(); iter != minRectFourLines.end(); ++iter) {
		//			for (int m = 0; m < (*iter).size(); ++m) {
		//				if ((int)sampBw.at<uchar>((*iter)[m]) == 0) {
		//					NoLineNumTemp[count]++;
		//				}
		//			}
		//			count++;
		//		}
		//		vector<int> NoLineNumPre;
		//		NoLineNumPre.insert(NoLineNumPre.end(), NoLineNumTemp.begin(), NoLineNumTemp.end());
		//		for (int i = 0; i < 4; i++) {
		//			if (NoLineNumTemp[i] > NoLineNumPre[i]) {
		//				isNoLineRise++;
		//			}
		//		}
		//		minRect.size += cv::Size2f(1, 1);
		//	}
		//	if (isNoLineRise == 0) {
		//		defect_flag = 3;
		//	}
		//}
		
		
		

		/*for (int i = 0; i < change_point.size(); i++) {
			if(abs(change_point[i].x - 1749) < 5&& abs(change_point[i].y - 2107) < 5)
				std::cout << "test" << endl;
		}*/


		//将缺失判断成断路，最小外接矩形跨越两条线路
		if(transNum ==4 && defect_flag ==1 )
		{
			Point2f center;
			center.x = (change_point[0].x+ change_point[1].x+ change_point[2].x+ change_point[3].x) / 4;
			center.y = (change_point[0].y + change_point[1].y + change_point[2].y + change_point[3].y) / 4;
			

			if ((int)templBw.at<uchar>(center) > 0 )

			{
				defect_flag = 1;
			}
			else
			{
				defect_flag = 2;
				vector<cv::Point2f> line{ contour_center,center };
				float x1 = contour_center.x > center.x ? center.x : contour_center.x;
				float x2 = contour_center.x < center.x ? center.x : contour_center.x;
				float y1 = contour_center.y > center.y ? center.y : contour_center.y;
				float y2 = contour_center.y < center.y ? center.y : contour_center.y;
				

				for (int j = 0;j<4;j++)
				{
					if (change_point[j].x>=x1&&change_point[j].x<=x2 && change_point[(j+1)%4].x >= x1 && change_point[(j+1)%4].x <= x2)
					{
						Point2f temp0 = change_point[j];
						Point2f temp1 = change_point[(j+1)%4];
						change_point.clear();
						change_point.push_back(temp0);
						change_point.push_back(temp1);
						break;
					}
					else if (change_point[j].y >= y1 && change_point[j].y <= y2 && change_point[(j + 1) % 4].y >= y1 && change_point[(j + 1) % 4].y <= y2)
					{
						Point2f temp0 = change_point[j];
						Point2f temp1 = change_point[(j + 1) % 4];
						change_point.clear();
						change_point.push_back(temp0);
						change_point.push_back(temp1);
						break;
					}

				}
				transNum = 2;
			}
		}

		if (!convexDetectFlag&&defect_flag == 4)
			continue;//不检测凸起
		if (!concaveDetectFlag&&defect_flag == 2)
			continue;//不检测缺失
		if (transNum == 0 && defect_flag == 4)//如果是残铜忽略
			continue;

		bool percentFlag = false;
		float percentage;
		if (transNum == 2/*&& defect_str[defect_flag] == "2"*/)//缺陷类型为缺失同时变化次数为2时计算缺失百分比
		{
			change_point[0] = Point(change_point[0].x, change_point[0].y);
			change_point[1] = Point(change_point[1].x, change_point[1].y);

			if (change_point[0].x != change_point[1].x || change_point[0].y != change_point[1].y)
			{
				percentFlag = true;
				//for (int k = 1;k< contoursRect[0].size();k++)
				//{
				//   cout << "contoursRect[0]: "<<contoursRect[0][k - 1].x+ rect_out.x << " "<<contoursRect[0][k - 1].y + rect_out.y << endl;
				//}
				percentage = bulge_missing_percentage(templBw, change_point, rectPointsOfPart);;//计算缺失凸起百分比
				string unit = "%";
				/*putText(sampGrayRegCopyZoom, to_string(i), Point(rectCon.x, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);*/
				//putText(sampGrayRegCopyZoom, to_string(minAreaRect_length), Point(rectCon.x, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//在图片上写文字
				//putText(sampGrayRegCopyZoom, to_string(int(percentage)), Point(rectCon.x, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//在图片上写文字
				//putText(sampGrayRegCopyZoom, unit, Point(rectCon.x + 100, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//在图片上写文字

				//如果缺失或者凸起的百分比小于阈值
				//if (defect_flag == 2) {//缺失
				//	if (percentage < userConfig->concaveRateThresh)
				//		continue;
				//}
				//if (defect_flag == 4) {//凸起
				//	if (percentage < userConfig->convexRateThresh)
				//		continue;
				//}
			}
			//continue;
		}
		
		////for (int i = 0; i < 4; i++) {
		////	Point2f p1(3765, 1011),p2(3820,1011);
		////	if (p1 == rectPointsOfPart[i]||p2== rectPointsOfPart[i])
		////		cout << "test" << endl;
		////}
		if ((transNum == 0||transNum==1) && defect_flag == 2) {
			vector<int> angle = { 0,45,90,135,180,-180,-135,-90,-45 };
			percentFlag = true;
			Point start = rectPoints[0];
			vector<Point> change_points{ start,start };
			float line_0 = bulge_missing_line_width(templBw, change_points, 0);
			float line_0r = bulge_missing_line_width(templBw, change_points, 180);
			float line_90 = bulge_missing_line_width(templBw, change_points, 90);
			float line_90r= bulge_missing_line_width(templBw, change_points, -90);
			float line_hor = line_0 + line_0r;
			float line_ver = line_90 + line_90r;
			float long_side = line_hor > line_ver ? line_hor : line_ver;
			
			percentage = minAreaRect_length / (line_ver + line_hor - long_side);
			if (percentage > 1)
				percentage = minAreaRect_length / long_side;

			percentage *= 100;
		}


		//int concaveRateThresh; //线路缺失率的阈值
		//int convexRateThresh; //线路凸起率的阈值

		//percentFlag只代表有两个变化点的缺失或凸起
		if (convexDetectFlag&&defect_flag == 4 && percentFlag&&percentage >= userConfig->convexRateThresh)
			defect_flag = 3;//凸起超过阈值
		if (concaveDetectFlag&&defect_flag == 2 && percentFlag&&percentage >= userConfig->concaveRateThresh)
			defect_flag = 1;//缺失超过阈值

	
		/********************截取缺陷所在分图，并在上面标记************************/
		int w_b = 300, h_b = 300;//缺陷分图的大小
		Rect rect = boundingRect(Mat(contours[i]));
		Point pt1, pt2, pt3, pt4;
		pt1.x = rect.x;
		pt1.y = rect.y;
		pt2.x = rect.x + rect.width;
		pt2.y = rect.y + rect.height;
		pt3.x = (pt2.x + pt1.x) / 2 - w_b / 2;//pt3是300×300分图在diffBw中的左上角坐标
		pt3.y = (pt2.y + pt1.y) / 2 - h_b / 2;

		//防止分图越界,如果分图imgSeg超出src边界则往回收缩
		if (pt3.x < 0) pt3.x = 0;
		if (pt3.y < 0) pt3.y = 0;
		if (pt3.x + w_b > sampGrayReg.cols - 1) w_b = sampGrayReg.cols - 1 - pt3.x;
		if (pt3.y + h_b > sampGrayReg.rows - 1) h_b = sampGrayReg.rows - 1 - pt3.y;

		Mat imgSeg;//配准后样本分图
		sampGrayReg(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(imgSeg);//分图的左上角点为pt3,宽和高为w_b,h_b
		Mat templSeg;//模板分图
		templGray(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(templSeg);
		Mat diffSeg;//差异分图
		diffBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(diffSeg);

		int pos_x = pt3.x + w_b / 2;
		int pos_y = pt3.y + h_b / 2;

		
		Rect rect1;//缺陷标记方框
		int w_s = 10, h_s = 10;//缺陷矩形框扩展的宽和高
		rect1.x = rect.x - pt3.x - w_s / 2;
		rect1.y = rect.y - pt3.y - h_s / 2;
		rect1.height = rect.height + h_s;
		rect1.width = rect.width + w_s;

		//防止方框越界
		if (rect1.x < 0) rect1.x = 0;
		if (rect1.y < 0) rect1.y = 0;
		if (rect1.x + rect1.width > imgSeg.cols - 1) rect1.width = imgSeg.cols - 1 - rect1.x;
		if (rect1.y + rect1.height > imgSeg.rows - 1) rect1.height = imgSeg.rows - 1 - rect1.y;

		cvtColor(imgSeg, imgSeg, cv::COLOR_GRAY2BGR);
		rectangle(imgSeg, rect1, CV_RGB(255, 0, 0), 2);
		Size sz = diffBw.size();
		defectNum++;//增加缺陷计数
		pos_x = sz.width*currentCol + pos_x;//缺陷在整体图像中的横坐标
		pos_y = sz.height*runtimeParams->currentRow_detect + pos_y;//缺陷在整体图像中的纵坐标


		//在配准后的样本图的克隆上绘制缺陷(排除所有伪缺陷后再绘制
		Rect rec = Rect((int)(rect_out.x*scalingFactor), (int)(rect_out.y*scalingFactor), (int)(rect_out.width*scalingFactor), (int)(rect_out.height*scalingFactor));
		rectangle(sampGrayRegCopyZoom, rec, Scalar(0, 0, 255), 1);

		string defectInfo = defect_str[defect_flag];
		if (percentFlag)
			defectInfo += to_string(int(percentage));
	
		cv::putText(sampGrayRegCopyZoom, defectInfo, Point(rect.x, rect.y), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0), 1, 8, 0);

		QChar fillChar = '0'; //当字符串长度不够时使用此字符进行填充
		QString outPath = runtimeParams->currentOutputDir + "/"; //当前序号对应的输出目录
		outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(pos_x, 5, 10, fillChar).arg(pos_y, 5, 10, fillChar).arg(defect_flag);
		outPath += userConfig->ImageFormat; //添加图像格式的后缀
		//cv::putText(imgSeg, to_string(factor),Point(0,imgSeg.rows-1), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0, 255, 255), 1, 8, 0);


		//保存缺陷坐标，缺陷类型，及缺陷分图
		cv::Point3i detailPoints{ pos_x,pos_y,defect_flag };
		detailImage.insert(std::make_pair(detailPoints, imgSeg));
		//imwrite(outPath.toStdString(), imgSeg); //存图
		
	}
	return sampGrayRegCopyZoom;
}


//void DetectFunc::save(const std::string& path, Mat& image_templ_gray)
//{
//	Mat temp;
//	cv::pyrDown(image_templ_gray, temp);
//	cv::pyrDown(temp, temp);
//	cv::pyrDown(temp, temp);
//	Ptr<SURF> detector = SURF::create(3500, 3, 3, true, true);
//	detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
//	cv::FileStorage store(path, cv::FileStorage::WRITE);
//	cv::write(store, "keypoints", keypoints);
//	cv::write(store, "descriptors", descriptors);
//	store.release();
//
//}

//void DetectFunc::load(const std::string& path) 
//{
//	cv::FileStorage store(path, cv::FileStorage::READ);
//	cv::FileNode n1 = store["keypoints"];
//	cv::read(n1, keypoints);
//	cv::FileNode n2 = store["descriptors"];
//	cv::read(n2, descriptors);
//	store.release();
//}


Scalar DetectFunc::getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	// INITS
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d); // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2 = I2.mul(I2); // I2^2
	Mat I1_2 = I1.mul(I1); // I1^2
	Mat I1_I2 = I1.mul(I2); // I1 * I2
	// END INITS

	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	// FORMULA
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}

/**
*	功能：计算凸起或者缺失长度
*	输入：
*		templBw: 模板二值图像
*		change_point:变化点
*       angle:遍历角度
*   输出：凸起或缺失长度
*/
float DetectFunc::bulge_missing_line_width(cv::Mat &templBw, std::vector<cv::Point> change_point, int angle)
{
	Point correspond0, correspond1;//线路上对称的变化点
	float line0_length = 0, line1_length = 0, line_length = 0;//缺陷所在线宽
	int traverse_number0 = 0, traverse_number1 = 0;
	//Point change0_pos = Point(change_point[0].x + rect_out.x, change_point[0].y + rect_out.y);//变化点在分图上的坐标
	//Point change1_pos = Point(change_point[1].x + rect_out.x, change_point[1].y + rect_out.y);
	int boundary_x = templBw.cols - 1, boundary_y = templBw.rows - 1;//遍历时如果到达分图边界就停止遍历，同时得到对应的线宽
	int k, line_width = 2500;
	if (angle == 0)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x++, change_point[0].y);
			Point temp0 = Point(pre0.x + 1, pre0.y);
			/*cout << "pre0:" << pre0 << "temp0:" << temp0 << endl;*/
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y||(int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = traverse_number0;
				break;
			}
			//Point pre1 = Point(change_point[1].x++, change_point[1].y);
			//Point temp1 = Point(pre1.x + 1, pre1.y);
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = traverse_number1;
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == 45)
	{
		/*	Point pre0 = Point(change0_pos.x, change0_pos.y);
			Point temp0 = Point(change0_pos.x + 1, change0_pos.y - 1);*/
			///*Point pre0 = Point(change0_pos.x + 1, change0_pos.y+1);*/
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x++, change_point[0].y++);
			Point temp0 = Point(pre0.x + 1, pre0.y + 1);
			/*Point pre0 = Point(change0_pos.x++, change0_pos.y--);
			Point temp0 = Point(change0_pos.x+1, change0_pos.y-1);
			temp0 = Point(change0_pos.x++, change0_pos.y--);
		/*	Point pre1 = change1_pos;
			Point temp1 = Point(change1_pos.x++, change1_pos.y++);*/
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = sqrt(traverse_number0*traverse_number0 + traverse_number0 * traverse_number0);
				break;
			}
			//Point pre1 = Point(change_point[1].x++, change_point[1].y++);
			//Point temp1 = Point(pre1.x + 1, pre1.y + 1);
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = sqrt(traverse_number1*traverse_number1 + traverse_number1 * traverse_number1);
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == 90)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x, change_point[0].y++);
			Point temp0 = Point(pre0.x, pre0.y + 1);
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = traverse_number0;
				break;
			}
			//Point pre1 = Point(change_point[1].x, change_point[1].y++);
			//Point temp1 = Point(pre1.x, pre1.y + 1);
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = traverse_number1;
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == 135)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x--, change_point[0].y++);
			Point temp0 = Point(pre0.x - 1, pre0.y + 1);
			/*cout << "pre0:" << pre0 << "temp0:" << temp0 << endl;*/
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = sqrt(traverse_number0*traverse_number0 + traverse_number0 * traverse_number0);
				break;
			}
			//Point pre1 = Point(change_point[1].x--, change_point[1].y++);
			//Point temp1 = Point(pre1.x - 1, pre1.y + 1);
			///*	cout << "pre1:" << pre0 << "temp1:" << temp0 << endl;*/
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = sqrt(traverse_number1*traverse_number1 + traverse_number1 * traverse_number1);
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == -180 || angle == 180)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x--, change_point[0].y);
			Point temp0 = Point(pre0.x - 1, pre0.y);
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = traverse_number0;
				break;
			}
			//Point pre1 = Point(change_point[1].x--, change_point[1].y);
			//Point temp1 = Point(pre1.x - 1, pre1.y);
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = traverse_number1;
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == -135)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x--, change_point[0].y--);
			Point temp0 = Point(pre0.x - 1, pre0.y - 1);
			//Point pre1 = change1_pos;
			//Point temp1 = Point(change1_pos.x--, change1_pos.y--);

			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = temp0;
				traverse_number0 = k + 1;
				line0_length = sqrt(traverse_number0*traverse_number0 + traverse_number0 * traverse_number0);
				break;
			}
			//Point pre1 = Point(change_point[1].x--, change_point[1].y--);
			//Point temp1 = Point(pre1.x - 1, pre1.y - 1);
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = sqrt(traverse_number1*traverse_number1 + traverse_number1 * traverse_number1);
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == -90)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x, change_point[0].y--);
			Point temp0 = Point(pre0.x, pre0.y - 1);
			/*cout << "pre0:" << pre0 << "temp0:" << temp0 << endl;*/
			/*for (int i = 0; i <= 2; i++)*/
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = traverse_number0;
				break;
			}
			//Point pre1 = Point(change_point[1].x, change_point[1].y--);
			//Point temp1 = Point(pre1.x, pre1.y - 1);
			///*cout << "pre1:" << pre1 << "temp1:" << temp1 << endl;*/
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = traverse_number1;
			//	/*	break;*/
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	if (angle == -45)
	{
		for (k = 0; k < line_width; k++)
		{
			Point pre0 = Point(change_point[0].x++, change_point[0].y--);
			Point temp0 = Point(pre0.x + 1, pre0.y - 1);
			if (temp0.x <= 0 || temp0.x >= boundary_x || temp0.y <= 0 || temp0.y >= boundary_y || (int)templBw.at<uchar>(pre0) != (int)templBw.at<uchar>(temp0))
			{
				correspond0 = pre0;
				traverse_number0 = k + 1;
				line0_length = sqrt(traverse_number0*traverse_number0 + traverse_number0 * traverse_number0);
				break;
			}
			//Point pre1 = Point(change_point[1].x++, change_point[1].y--);
			//Point temp1 = Point(pre1.x + 1, pre1.y - 1);
			//if ((int)templBw.at<uchar>(pre1) != (int)templBw.at<uchar>(temp1) || temp1.x == 0 || temp1.x == boundary_x || temp1.y == 0 || temp1.y == boundary_y)
			//{
			//	correspond1 = pre1;
			//	traverse_number1 = k + 1;
			//	line1_length = sqrt(traverse_number1*traverse_number1 + traverse_number1 * traverse_number1);
			//}
			//if (line0_length != 0 || line1_length != 0)
			//{
			//	break;
			//}
		}
	}
	/*cout << "line0_length: " << line0_length << endl;*/
	//cout << "line1_length: " << line1_length << endl;
	/*if (line_length < line1_length)
	{*/
	return line_length = line0_length;
}

/**
*	功能：计算凸起或者缺失百分比
*	输入：
*		templBw: 模板二值图像
*		change_point:变化点
*       minAreaRect_length:缺陷旋转矩形的长边
*   输出：凸起或缺失百分比
*/
float DetectFunc::bulge_missing_percentage(cv::Mat &templBw, std::vector<cv::Point> change_point, cv::Point2f *rectPointsOfPart)
{
	vector<Point2i> neighbors{ Point2i(change_point[0].x + 1,change_point[0].y),/*Point2i(change_point[0].x + 2,change_point[0].y),Point2i(change_point[0].x + 3,change_point[0].y ),*//*Point2i(change_point[0].x - 4,change_point[0].y - 4),*/
			Point2i(change_point[0].x - 1,change_point[0].y)/*,Point2i(change_point[0].x - 2,change_point[0].y ),Point2i(change_point[0].x -3,change_point[0].y )*//*,Point2i(change_point[0].x + 4,change_point[0].y + 4)*/ ,
		 Point2i(change_point[0].x ,change_point[0].y - 1),/*Point2i(change_point[0].x ,change_point[0].y - 2),Point2i(change_point[0].x ,change_point[0].y - 3),*/
			Point2i(change_point[0].x ,change_point[0].y + 1)/*,Point2i(change_point[0].x ,change_point[0].y + 2),Point2i(change_point[0].x ,change_point[0].y + 3)*/ };//变化点的右上左下邻域
	float change_line = 0, vertical_line = 0, vertical_angle = 0, change_angle = 0, vertical_rangle = 0;//求变化点斜率，法线斜率，法线角度，以及确定距法线最近的遍历方向
	float rect_line0, rect_angle0, rect_line1, rect_angle1;
	float flaw_width, flaw_height, flaw_length;//缺陷的宽，高
	flaw_width = sqrt((rectPointsOfPart[1].x - rectPointsOfPart[0].x)*(rectPointsOfPart[1].x - rectPointsOfPart[0].x) + (rectPointsOfPart[1].y - rectPointsOfPart[0].y)*(rectPointsOfPart[1].y - rectPointsOfPart[0].y)) - 10;//旋转矩形向外扩张了10像素，计算旋转矩形的边长需要复原
	flaw_height = sqrt((rectPointsOfPart[2].x - rectPointsOfPart[1].x)*(rectPointsOfPart[2].x - rectPointsOfPart[1].x) + (rectPointsOfPart[2].y - rectPointsOfPart[1].y)*(rectPointsOfPart[2].y - rectPointsOfPart[1].y)) - 10;
	if (change_point[1].y - change_point[0].y == 0)//两变化点的连线平行于水平线
	{
		//if (templBw.at<uchar>(neighbors[2]) != templBw.at<uchar>(neighbors[3]))
		//{
		//	if (templBw.at<uchar>(neighbors[2]) > 0)
		//	{
		//		vertical_angle = -90;
		//		vertical_rangle = vertical_angle;
		//	}
		//	else if (templBw.at<uchar>(neighbors[3]) > 0)
		//	{
		//		vertical_angle = 90;
		//		vertical_rangle = vertical_angle;
		//	}

		//}
		//else if (templBw.at<uchar>(neighbors[2]) == templBw.at<uchar>(neighbors[3]))
		//{
		vertical_angle = -90;//确定变化点连线的法线方向
		vertical_rangle = 90;

		//}

		if (rectPointsOfPart[1].y == rectPointsOfPart[0].y || rectPointsOfPart[1].y == rectPointsOfPart[2].y)
		{
			//float flaw_width, flaw_height;//旋转矩形的宽和高
			/*flaw_width = sqrt((rectPointsOfPart[1].x - rectPointsOfPart[0].x)*(rectPointsOfPart[1].x - rectPointsOfPart[0].x) + (rectPointsOfPart[1].y - rectPointsOfPart[0].y)*(rectPointsOfPart[1].y - rectPointsOfPart[0].y));
			flaw_height = sqrt((rectPointsOfPart[2].x - rectPointsOfPart[1].x)*(rectPointsOfPart[2].x - rectPointsOfPart[1].x) + (rectPointsOfPart[2].y - rectPointsOfPart[1].y)*(rectPointsOfPart[2].y - rectPointsOfPart[1].y));*/
			flaw_length = flaw_width;
			//得到旋转矩形的短边
			cout << "flaw_length: " << flaw_length << "flaw_width: " << flaw_width << "flaw_height: " << flaw_height << endl;
			if (flaw_width > flaw_height)
			{
				flaw_length = flaw_height;

			}
		}
		else
		{
			rect_line0 = (float)(rectPointsOfPart[1].y - rectPointsOfPart[0].y) / (float)(rectPointsOfPart[1].x - rectPointsOfPart[0].x);
			rect_angle0 = atan(rect_line0) * 180 / M_PI;

			rect_line1 = (float)(rectPointsOfPart[2].y - rectPointsOfPart[1].y) / (float)(rectPointsOfPart[2].x - rectPointsOfPart[1].x);
			rect_angle1 = atan(rect_line1) * 180 / M_PI;

			cout << "rect_angle0: " << rect_angle0 << " rect_angle1: " << rect_angle1 << endl;
			float rect_angleSub0 = fabs(fabs(rect_angle0) - fabs(vertical_angle));
			float rect_angleSub1 = fabs(fabs(rect_angle1) - fabs(vertical_angle));
			cout << "rect_angleSub0: " << rect_angleSub0 << " rect_angleSub1: " << rect_angleSub1 << endl;
			if (rect_angleSub0 <= rect_angleSub1)
			{
				flaw_length = flaw_width;
			}
			else
			{
				flaw_length = flaw_height;
			}

		}
	}


	else if (change_point[1].y - change_point[0].y != 0)
	{
		change_line = (float)(change_point[1].y - change_point[0].y) / (float)(change_point[1].x - change_point[0].x);
		vertical_line = -1 / (change_line);
		vertical_angle = atan(vertical_line) * 180 / M_PI;
		change_angle = atan(change_line) * 180 / M_PI;
		cout << "change_line: " << change_line << " vertical_line: " << vertical_line << endl;
		cout << "change_angle: " << change_angle << " vertical_angle: " << vertical_angle << endl;

		cout << "rectPointsOfPart[0]: " << rectPointsOfPart[0] << " rectPointsOfPart[1]: " << rectPointsOfPart[1] << " rectPointsOfPart[2]: " << rectPointsOfPart[2] << endl;
		if (rectPointsOfPart[1].y == rectPointsOfPart[0].y || rectPointsOfPart[1].y == rectPointsOfPart[2].y)
		{
			//旋转矩形的宽和高

			flaw_length = flaw_width;
			//得到旋转矩形的短边
			cout << "flaw_length: " << flaw_length << "flaw_width: " << flaw_width << "flaw_height: " << flaw_height << endl;
			if (flaw_width > flaw_height)
			{
				flaw_length = flaw_height;

			}
		}
		else
		{
			rect_line0 = (float)(rectPointsOfPart[1].y - rectPointsOfPart[0].y) / (float)(rectPointsOfPart[1].x - rectPointsOfPart[0].x);
			rect_angle0 = atan(rect_line0) * 180 / M_PI;

			rect_line1 = (float)(rectPointsOfPart[2].y - rectPointsOfPart[1].y) / (float)(rectPointsOfPart[2].x - rectPointsOfPart[1].x);
			rect_angle1 = atan(rect_line1) * 180 / M_PI;
			//if (rect_angle0<0)
			//{
			//	rect_angle0 = fabs(rect_angle0) + 90;
			//}
			//if (rect_angle1 < 0)
			//{
			//	rect_angle1 = fabs(rect_angle1) + 90;
			//}

			cout << "rect_angle0: " << rect_angle0 << " rect_angle1: " << rect_angle1 << endl;
			float rect_angleSub0 = fabs(fabs(rect_angle0) - fabs(vertical_angle));
			float rect_angleSub1 = fabs(fabs(rect_angle1) - fabs(vertical_angle));
			cout << "rect_angleSub0: " << rect_angleSub0 << " rect_angleSub1: " << rect_angleSub1 << endl;
			if (rect_angleSub0 <= rect_angleSub1)
			{
				flaw_length = flaw_width;
			}
			else
			{
				flaw_length = flaw_height;
			}
		}
		//if (templBw.at<uchar>(neighbors[0]) != templBw.at<uchar>(neighbors[1]))
		//{
		//	if (templBw.at<uchar>(neighbors[0]) > 0)
		//	{
		//		vertical_angle = vertical_angle;
		//		vertical_rangle = vertical_angle;
		//		/*break;*/
		//	}
		//	else if (templBw.at<uchar>(neighbors[1]) > 0)
		//	{
		//		if (vertical_angle >= 0)
		//		{
		//			vertical_angle = vertical_angle - 180;
		//			vertical_rangle = vertical_angle;
		//		}

		//		else if (vertical_angle < 0)
		//		{
		//			vertical_angle = vertical_angle + 180;
		//			vertical_rangle = vertical_angle;

		//		}
		//	}

		//}
		//else if (templBw.at<uchar>(neighbors[0]) == templBw.at<uchar>(neighbors[1]))
		//{
/*			vertical_angle = vertical_angle;*/
		if (vertical_angle >= 0)
		{
			vertical_rangle = vertical_angle - 180;
		}

		else if (vertical_angle < 0)
		{
			vertical_rangle = vertical_angle + 180;

		}
		/*		}*/

	}
	cout << "flaw_length: " << flaw_length << endl;
	cout << "change_angle: " << change_angle << " vertical_angle: " << vertical_angle << endl;
	cout << " vertical_rangle: " << vertical_rangle << endl;


	vector<int> angle = { 0,45,90,135,180,-180,-135,-90,-45 };//遍历方向
	vector<float> angle_sub, rangle_sub;
	int k;
	for (k = 0; k < 9; k++)
	{
		float sub = fabs(vertical_angle - angle[k]);
		angle_sub.push_back(sub);//法线方向与遍历方向之差的集合
	}
	for (k = 0; k < 9; k++)
	{
		float sub = fabs(vertical_rangle - angle[k]);
		rangle_sub.push_back(sub);//法线方向与遍历方向之差的集合
	}
	float min_sub = angle_sub[0];
	int min_pos = 0;
	for (k = 0; k < 9; k++)
	{
		if (min_sub > angle_sub[k])
		{
			min_sub = angle_sub[k];//法线方向与遍历方向的最小差值
			min_pos = k;
		}
	}
	float min_rsub = rangle_sub[0];
	int min_rpos = 0;
	for (k = 0; k < 9; k++)
	{
		if (min_rsub > rangle_sub[k])
		{
			min_rsub = rangle_sub[k];//法线方向与遍历方向的最小差值
			min_rpos = k;
		}
	}
	float  line_length = 0, rline_length = 0;//缺陷所在线宽
	line_length = bulge_missing_line_width(templBw, change_point, angle[min_pos]);
	rline_length = bulge_missing_line_width(templBw, change_point, angle[min_rpos]);
	cout << "line_length: " << line_length << endl;
	cout << "rline_length: " << rline_length << endl;
	if (line_length < rline_length)
	{
		line_length = rline_length;

	}



	cout << "angle[min_pos] " << angle[min_pos] << endl;
	cout << "angle[min_rpos] " << angle[min_rpos] << endl;

	float bulge_missing_percentage = flaw_length / line_length;
	if (bulge_missing_percentage > 1)
	{
		flaw_length = flaw_width;
		if (flaw_length > flaw_height)
		{
			flaw_length = flaw_height;
		}
		bulge_missing_percentage = flaw_length / line_length;
	}
	bulge_missing_percentage = bulge_missing_percentage * 100;
	cout << "line_length: " << line_length << endl;
	cout << "缺失百分比: " << bulge_missing_percentage << "%" << endl;


	return bulge_missing_percentage;
}

void DetectFunc::save(const std::string& path, Mat& image_template_gray) {//实际上没有调用，只是为了统一
	Mat temp;
	cv::pyrDown(image_template_gray, temp);	
	if (userConfig->matchingAccuracyLevel == 2)//低精度
	{
		cv::pyrDown(temp, temp);
	}
		
	Ptr<SURF> detector = SURF::create(100, 4, 4, true, true);
	detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
	cv::FileStorage store(path, cv::FileStorage::WRITE);
	cv::write(store, "keypoints", keypoints);
	cv::write(store, "descriptors", descriptors);
	store.release();

}

void DetectFunc::load(const std::string& path) {
	cv::FileStorage store(path, cv::FileStorage::READ);
	cv::FileNode n1 = store["keypoints"];
	cv::read(n1, keypoints);
	cv::FileNode n2 = store["descriptors"];
	cv::read(n2, descriptors);
	store.release();
}
