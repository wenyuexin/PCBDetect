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
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
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
	if (userConfig->matchingAccuracyLevel == 2)//�;���
	{
		pyrDown(pyr, pyr);
	}
     

	detector->detectAndCompute(pyr, Mat(), keypoints_2, descriptors_2);

	double t2 = clock();
	cout << "��ȡ������ʱ��" << double(t2 - t1) / CLOCKS_PER_SEC << endl;

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

	else //��Ӧ�Ծ���ļ������ٵ�ʹ��4����
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
		cout << "ƥ�䲢��ȡ�任����ʱ��" << double(t3 - t2) / CLOCKS_PER_SEC << endl;


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
	int totalCol = runtimeParams->nCamera - 1;//��0��ʼ
	int totalRow = runtimeParams->nPhotographing - 1;
	Mat res = Mat::zeros(grayImg.size(), CV_8UC1);
	Rect rect;//roi����
	if (curCol == 0 && curRow == 0)//����
	{
		rect.x = point_left.x;
		rect.y = point_right.y;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows - point_right.y;

	}
	else if (curCol == 0 && curRow == totalRow)//����
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = point_left.y;
	}

	else if (curCol == totalCol && curRow == 0)//����
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = point_right.x;
		rect.height = grayImg.rows - point_right.y;
	}
	else if (curCol == totalCol && curRow == totalRow)//����
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = point_left.y;
	}
	else if (curCol == 0 && 0 < curRow && curRow < totalRow)//���
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows;
	}

	else if (curCol == totalCol && 0 < curRow && curRow < totalRow)//�ұ�
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = grayImg.rows;
	}

	else if (curRow == 0 && 0 < curCol && curCol < totalCol)//�ϱ�
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = grayImg.cols;
		rect.height = grayImg.rows - point_right.y;
	}

	else if (curRow == totalRow && 0 < curCol && curCol < totalCol)//�±�
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = grayImg.cols;
		rect.height = point_left.y;
	}
	else if (0 < curCol && curCol < totalCol && 0 < curRow && curRow < totalRow)//����
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
	int totalCol = runtimeParams->nCamera - 1;//��0��ʼ
	int totalRow = runtimeParams->nPhotographing - 1;
	Mat res = Mat::zeros(grayImg.size(), CV_8UC1);
	Rect rect;//roi����
	if (curCol == 0 && curRow == 0)//����
	{
		rect.x = point_left.x;
		rect.y = point_right.y;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows - point_right.y;

	}
	else if (curCol == 0 && curRow == totalRow)//����
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = point_left.y;
	}

	else if (curCol == totalCol && curRow == 0)//����
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = point_right.x;
		rect.height = grayImg.rows - point_right.y;
	}
	else if (curCol == totalCol && curRow == totalRow)//����
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = point_left.y;
	}
	else if (curCol == 0 && 0 < curRow && curRow < totalRow)//���
	{
		rect.x = point_left.x;
		rect.y = 0;
		rect.width = grayImg.cols - point_left.x;
		rect.height = grayImg.rows;
	}

	else if (curCol == totalCol && 0 < curRow && curRow < totalRow)//�ұ�
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = grayImg.rows;
	}

	else if (curRow == 0 && 0 < curCol && curCol < totalCol)//�ϱ�
	{
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = grayImg.cols;
		rect.height = grayImg.rows - point_right.y;
	}

	else if (curRow == totalRow && 0 < curCol && curCol < totalCol)//�±�
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = grayImg.cols;
		rect.height = point_left.y;
	}
	else if (0 < curCol && curCol < totalCol && 0 < curRow && curRow < totalRow)//����
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

	// Define the motion model �����˶�ģ��
	const int warp_mode =cv:: MOTION_EUCLIDEAN;

	// Set a 2x3 or 3x3 warp matrix depending on the motion model. �任����
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
	// Specify the number of iterations. �㷨��������
	int number_of_iterations = 5000;

	// Specify the threshold of the increment
	// in the correlation coefficient between two iterations �趨��ֵ
	double termination_eps = 1e-10;

	// Define termination criteria ������ֹ����
	cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, number_of_iterations, termination_eps);

	// Run the ECC algorithm. The results are stored in warp_matrix. ECC�㷨
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
	if(userConfig->matchingAccuracyLevel==2)//�;���
		pyrDown(pyrTemp, pyrTemp);	

	pyrDown(image_sample_gray, pyrSamp);
	pyrDown(pyrSamp, pyrSamp);
	if (userConfig->matchingAccuracyLevel==2)//�;���
		pyrDown(pyrSamp, pyrSamp);

	detector->detectAndCompute(pyrTemp, Mat(), keypoints_1, descriptors_1);
	detector->detectAndCompute(pyrSamp, Mat(), keypoints_2, descriptors_2);

	//detector->detectAndCompute(image_template_gray, Mat(), keypoints_1, descriptors_1);
	//detector->detectAndCompute(image_sample_gray, Mat(), keypoints_2, descriptors_2);

	double t2 = clock();
	cout << "��ȡ������ʱ��" << double(t2 - t1) / CLOCKS_PER_SEC << endl;

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
		cout << "ƥ���ȡ�任����ʱ��" << double(t3 - t2) / CLOCKS_PER_SEC << endl;

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
 *�Ƚ�����ֵ�����ٽ��в�ֵ����
 */
Mat DetectFunc::sub_process_new(Mat &templBw, Mat &sampBw, Mat& mask_roi, Mat& directFlaw, Mat& MorphFlaw,Mat& cannyFlaw) {
	Mat imgFlaw;
	
	cv::absdiff(templBw, sampBw, imgFlaw);
	directFlaw = imgFlaw.clone();
	bitwise_and(imgFlaw, mask_roi, imgFlaw);
	

	//�Բ�ֵͼ������̬ѧ�����ȿ���գ�����Ĵ�������С�߿��й�
	cv::Mat element_a = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);
	MorphFlaw = imgFlaw.clone();

	////����ģ���Ե������̬ѧ������ͼƬ��ˣ���ȡ�߽��ϵĵ�����
	//cv::Mat edges;
	//cv::Canny(templBw, edges, 150, 50);
	//cv::Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::dilate(edges, edges, element_b);
	//edges = 255 - edges;
	//cv::bitwise_and(edges, imgFlaw, imgFlaw);
	//cannyFlaw = imgFlaw.clone();


	////�ٽ���һ����̬ѧ����
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

	//�Բ�ֵͼ������̬ѧ�����ȿ���գ�����Ĵ�������С�߿��й�
	cv::Mat element_a = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);

	////����ģ���Ե������̬ѧ������ͼƬ��ˣ���ȡ�߽��ϵĵ�����
	cv::Mat edges;
	cv::Canny(templBw, edges, 150, 50);
	cv::Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(edges, edges, element_b);
	edges = 255 - edges;
	cv::bitwise_and(edges, imgFlaw, imgFlaw);


	//�ٽ���һ����̬ѧ����
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_a);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_a);


	return imgFlaw;
}


/**
*	���ܣ���ǲ��洢ȱ��ͼƬ��λ����Ϣ
*	���룺
*		diffBw: ��ֵͼ���ֵͼ
*		sampGrayReg:��׼��������Ҷ�ͼ
*		templBw: ģ���ֵͼ��
*		templGray:ģ��Ҷ�ͼ
*       defectNum:ȱ�����
*       currentCol:������
*/
cv::Mat DetectFunc::markDefect_test(int currentCol, Mat &diffBw, Mat &sampGrayReg, double scalingFactor, Mat &templBw, Mat &templGray,Mat sampBw, int &defectNum, std::map<cv::Point3i, cv::Mat, cmp_point3i> &detailImage,cv::Mat rectBlack) {
	Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//threshold(diffBw, diffBw, 0, 255, cv::THRESH_BINARY);
	//dilate(diffBw, diffBw, kernel_small);//�Բ�ֵͼ�������ͣ���������ͽ����ж�
	
	//ģ���ֵͼ��Ե��ƽ������
	medianBlur(templBw, templBw, 3);
	blur(templBw, templBw, Size(3, 3));
	threshold(templBw, templBw, 0, 255, cv::THRESH_BINARY);

	cv::Size originalSubImageSize = sampGrayReg.size(); //��ͼ��ԭʼ�ߴ�
	cv::Size scaledSubImageSize(originalSubImageSize.width*scalingFactor, originalSubImageSize.height*scalingFactor); //������ķ�ͼ�ߴ�

	Mat sampGrayRegCopy = sampGrayReg.clone();
	cvtColor(sampGrayRegCopy, sampGrayRegCopy, cv::COLOR_GRAY2BGR);
	cv::Mat sampGrayRegCopyZoom;
	cv::resize(sampGrayRegCopy, sampGrayRegCopyZoom, scaledSubImageSize, (0, 0), (0, 0), cv::INTER_LINEAR);

	Mat diffBw_copy = diffBw.clone();
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i>   hierarchy;
	cv::findContours(diffBw_copy, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//�������ȱ�ݣ����ո���PCB�ͺţ����κţ���Ž���Ŀ¼���洢ͼƬ,������ȱ�������
	if (contours.size() == 0) {
		return sampGrayRegCopyZoom;//������ȱ�ݣ�ֱ�ӷ������ź������
	}

	for (int i = 0; i < contours.size(); i++) {
		int conArea = contourArea(contours[i], false);
		if (conArea <= 2) continue; //ȱ����С���
		
		//������Ӿ���rect_outΪ������Ӿ���������20������
		Rect rectCon = boundingRect(Mat(contours[i]));
		int larger_width = 10;//��΢����ȱ�����ڵľ�������
		int larger_height = 10;
		Rect rect_out = Rect(rectCon.x - larger_width, rectCon.y - larger_height, rectCon.width + 2 * larger_width, rectCon.height + 2 * larger_height);
		Mat temp_area;
		Mat samp_area;
		Mat tempBwPart;
		Mat diffBwPart;


		//��ֹ��ͼԽ��
		int width_sub = 0, height_sub = 0;
		if (rect_out.x < 0)
			rect_out.x = 0;
		if (rect_out.y < 0)
			rect_out.y = 0;
		if ((rect_out.x + 2 * larger_width + rectCon.width) > (sampGrayReg.cols - 1)) {
			width_sub = sampGrayReg.cols - 1 - rect_out.x - rectCon.width;
			larger_width = (width_sub - 1) / 2;
		}//���һ����һ��������������������ɵ�
		if ((rect_out.y + 2 * larger_height + rectCon.height) > (sampGrayReg.rows - 1)) {
			height_sub = sampGrayReg.rows - 1 - rect_out.y - rectCon.height;
			larger_height = (height_sub - 1) / 2;
		}
		rect_out = Rect(rect_out.x, rect_out.y, rectCon.width + 2 * larger_width, rectCon.height + 2 * larger_height);

		Point rect_out_rb = Point(rect_out.x + rect_out.width, rect_out.y + rect_out.height);
		int boundary_x = sampGrayReg.cols - 1;
		int boundary_y = sampGrayReg.rows - 1;
		if (rect_out.x<0 || rect_out.y <0 || rect_out_rb.x > boundary_x || rect_out_rb.y> boundary_y) {
			cout << "rect_outԽ��" << endl;
		}

		try {
			temp_area = templGray(rect_out);
			samp_area = sampGrayReg(rect_out);
			tempBwPart = templBw(rect_out).clone();
			diffBwPart = diffBw(rect_out).clone();
		}
		catch (std::exception e) {
			qDebug() << QString::fromLocal8Bit("����Խ��");
		};

		vector<cv::Point2i> locations;
		cv::findNonZero(diffBwPart, locations);

		if (locations.size()<9/* <= 10*/) {
			Size szDiff = diffBwPart.size();
			continue;
		}
		/**************************����ȱ���жϴ���*****************************/
		int transNum = 0;
		//Ѱ��״̬�任��ĸ���
		Point2f rectPointsOfPart[4];
		vector<Point> rectPoints;//��Ӿ����ϵĵ�
		vector<Point> change_point;
		int size = 0;
		while (/*change_point.empty()&&*/size<=6) {
			vector<Point> change_point_temp;
			transNum = 0;
			rectPoints.clear();
			//��ȡȱ��������С��Ӿ����ϵ����е�
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
			//							Point2i(testPoint.x + 1,testPoint.y),//��
			//							Point2i(testPoint.x - 1,testPoint.y),//��
			//							Point2i(testPoint.x ,testPoint.y + 1),//��
			//							Point2i(testPoint.x ,testPoint.y - 1) };//�仯���������������
			//		for (int i = 0; i < 5; i++) {
			//			if ((int)templBw.at<uchar>(neighbors[i]) == 0) {
			//				cout << "test" << endl;
			//			}
			//		}
			//	}
			//		
			//}
			//
			vector<vector<Point>> fourLines(4);//�������ϵĵ�
			for (int i = 0; i < 4; i++) {
				cv::LineIterator it(diffBw, rectPointsOfPart[i], rectPointsOfPart[(i + 1) % 4], 8);
				for (int j = 0; j < it.count; j++, ++it)
				{
					fourLines[i].push_back(it.pos());
				}
				fourLines[i].pop_back();//ɾ�����һ����
				//�������ߵ��������������꼯��
				rectPoints.insert(rectPoints.end(), fourLines[i].begin(), fourLines[i].end());
			}
			rectPoints.push_back(rectPoints[0]);//���ã���Ӿ��ε����һ�������һ����Ҫ���бȽ�

			//���Ʊ任����
			for (int j = 0; j < 4; j++)
			{
				line(rectBlack, Point2f(rectPointsOfPart[j].x, rectPointsOfPart[j].y), Point2f(rectPointsOfPart[(j + 1) % 4].x, rectPointsOfPart[(j + 1) % 4].y), (255, 255, 255));
			}

			//Ѱ�ұ仯��
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
							change_point_temp.push_back(pre);//����任��
							//change_point.push_back(temp);//����任��
							rectBlack.at<cv::Vec3b>(pre) = { 255, 255, 0 };//��Ǳ任��
						}
						else if (templBw.at<uchar>(temp) > 0)
						{
							change_point_temp.push_back(temp);//����任��
							//change_point.push_back(temp);//����任��
							rectBlack.at<cv::Vec3b>(temp) = { 255, 255, 0 };//��Ǳ任��
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
							change_point_temp.push_back(pre);//����任��
							//change_point.push_back(temp);//����任��
							rectBlack.at<cv::Vec3b>(pre) = { 255, 255, 0 };//��Ǳ任��
						}
						else if (templBw.at<uchar>(temp) > 0)
						{
							change_point_temp.push_back(temp);//����任��
							//change_point.push_back(temp);//����任��
							rectBlack.at<cv::Vec3b>(temp) = { 255, 255, 0 };//��Ǳ任��
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
				qDebug() << QString::fromLocal8Bit("��ת��������������");
				//cout << "��ת��������������";
			}
		}
       



		//��ȡȱ����С��Ӿ��εĿ�͸ߣ��ÿ�ߴ���ȱ�ݳ���
		double minAreaRect_width, minAreaRect_height, minAreaRect_length;
		minAreaRect_width = sqrt((rectPointsOfPart[1].x - rectPointsOfPart[0].x)*(rectPointsOfPart[1].x - rectPointsOfPart[0].x) + (rectPointsOfPart[1].y - rectPointsOfPart[0].y)*(rectPointsOfPart[1].y - rectPointsOfPart[0].y));
		minAreaRect_height = sqrt((rectPointsOfPart[2].x - rectPointsOfPart[1].x)*(rectPointsOfPart[2].x - rectPointsOfPart[1].x) + (rectPointsOfPart[2].y - rectPointsOfPart[1].y)*(rectPointsOfPart[2].y - rectPointsOfPart[1].y));
		minAreaRect_length = minAreaRect_width > minAreaRect_height ? minAreaRect_width : minAreaRect_height;


		/************�����������ĵ�λ�ã��ж��Ƿ�����·ȱʧ*************/
		cv::Moments m = cv::moments(contours[i]);
		cv::Point2f contour_center = Point2f(static_cast<float>(m.m10 / (m.m00 + 1e-5)),
			static_cast<float>(m.m01 / (m.m00 + 1e-5)));//ͨ��ȱ�������ؼ���ȱ����������

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
			qDebug() << QString::fromLocal8Bit("ȱ����������������");
			//cout << "ȱ����������������";
		}

		


	/********************����״̬�任�������Ƿ����ȱʧ��ȱ�ݽ��з���**********************************/
	classfinish:
		bool convexDetectFlag = (userConfig->defectTypeToBeProcessed)[2];//͹�����־
		bool concaveDetectFlag = (userConfig->defectTypeToBeProcessed)[3];//ȱʧ����־

		int lack_flag = 0;//0��ʾ��ȱ��·��1��ʾȱ��·
		if (neighbors_sum >= 255 * 2) lack_flag = 1;

		int defect_flag = 0;
		//int trans = 0;
		//int boundaryX = sampBw.cols-1;
		//minRectOfPart.size -= cv::Size2f(8, 8);
		//minRectOfPart.points(rectPointsOfPart);
		if (lack_flag) {
			defect_flag = transNum > 2 ? 1 : 2;//��·:ȱʧ
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
			//defect_flag = trans >= 4 ? 1 : 2;//��·:ȱʧ
		}
		else {
			defect_flag = transNum > 2 ? 3 : 4;//��·:͹��
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
			//defect_flag = trans >= 4 ? 3 : 4;//��·:ȱʧ
		}


		////���ж�Ϊȱʧ��͹���ȱ�ݽ��ж����жϣ����ⷢ����죬���Ϊ��·����·
		//if (defect_flag == 2) {
		//	cv::RotatedRect minRect = minRectOfPart;
		//	minRect.size -= cv::Size2f(8, 8);
		//	int isNoLineDrop = 0;
		//	for (int k = 0; k < 2; k++) {
		//		Point2f minRectPoints[4];
		//		minRect.points(minRectPoints);
		//		vector<vector<Point>> minRectFourLines(4);//��ת�����������ϵĵ�
		//		for (int i = 0; i < 4; i++) {
		//			cv::LineIterator minIt(diffBw, minRectPoints[i], minRectPoints[(i + 1) % 4], 8);
		//			for (int j = 0; j < minIt.count; j++, ++minIt)
		//			{
		//				minRectFourLines[i].push_back(minIt.pos());
		//			}
		//		}

		//		//��ȡ��С��ת���������߷���·���ֵı仯��������������һ�������½������ƣ�˵����ȱ��Ϊȱʧ����Ϊ͹��
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
		//		vector<vector<Point>> minRectFourLines(4);//��ת�����������ϵĵ�
		//		for (int i = 0; i < 4; i++) {
		//			cv::LineIterator minIt(diffBw, minRectPoints[i], minRectPoints[(i + 1) % 4], 8);
		//			for (int j = 0; j < minIt.count; j++, ++minIt)
		//			{
		//				minRectFourLines[i].push_back(minIt.pos());
		//			}
		//		}

		//		//��ȡ��С��ת���������߷���·���ֵı仯��������������һ�������½������ƣ�˵����ȱ��Ϊȱʧ����Ϊ͹��
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


		//��ȱʧ�жϳɶ�·����С��Ӿ��ο�Խ������·
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
			continue;//�����͹��
		if (!concaveDetectFlag&&defect_flag == 2)
			continue;//�����ȱʧ
		if (transNum == 0 && defect_flag == 4)//����ǲ�ͭ����
			continue;

		bool percentFlag = false;
		float percentage;
		if (transNum == 2/*&& defect_str[defect_flag] == "2"*/)//ȱ������Ϊȱʧͬʱ�仯����Ϊ2ʱ����ȱʧ�ٷֱ�
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
				percentage = bulge_missing_percentage(templBw, change_point, rectPointsOfPart);;//����ȱʧ͹��ٷֱ�
				string unit = "%";
				/*putText(sampGrayRegCopyZoom, to_string(i), Point(rectCon.x, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);*/
				//putText(sampGrayRegCopyZoom, to_string(minAreaRect_length), Point(rectCon.x, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//��ͼƬ��д����
				//putText(sampGrayRegCopyZoom, to_string(int(percentage)), Point(rectCon.x, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//��ͼƬ��д����
				//putText(sampGrayRegCopyZoom, unit, Point(rectCon.x + 100, rectCon.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//��ͼƬ��д����

				//���ȱʧ����͹��İٷֱ�С����ֵ
				//if (defect_flag == 2) {//ȱʧ
				//	if (percentage < userConfig->concaveRateThresh)
				//		continue;
				//}
				//if (defect_flag == 4) {//͹��
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


		//int concaveRateThresh; //��·ȱʧ�ʵ���ֵ
		//int convexRateThresh; //��·͹���ʵ���ֵ

		//percentFlagֻ�����������仯���ȱʧ��͹��
		if (convexDetectFlag&&defect_flag == 4 && percentFlag&&percentage >= userConfig->convexRateThresh)
			defect_flag = 3;//͹�𳬹���ֵ
		if (concaveDetectFlag&&defect_flag == 2 && percentFlag&&percentage >= userConfig->concaveRateThresh)
			defect_flag = 1;//ȱʧ������ֵ

	
		/********************��ȡȱ�����ڷ�ͼ������������************************/
		int w_b = 300, h_b = 300;//ȱ�ݷ�ͼ�Ĵ�С
		Rect rect = boundingRect(Mat(contours[i]));
		Point pt1, pt2, pt3, pt4;
		pt1.x = rect.x;
		pt1.y = rect.y;
		pt2.x = rect.x + rect.width;
		pt2.y = rect.y + rect.height;
		pt3.x = (pt2.x + pt1.x) / 2 - w_b / 2;//pt3��300��300��ͼ��diffBw�е����Ͻ�����
		pt3.y = (pt2.y + pt1.y) / 2 - h_b / 2;

		//��ֹ��ͼԽ��,�����ͼimgSeg����src�߽�����������
		if (pt3.x < 0) pt3.x = 0;
		if (pt3.y < 0) pt3.y = 0;
		if (pt3.x + w_b > sampGrayReg.cols - 1) w_b = sampGrayReg.cols - 1 - pt3.x;
		if (pt3.y + h_b > sampGrayReg.rows - 1) h_b = sampGrayReg.rows - 1 - pt3.y;

		Mat imgSeg;//��׼��������ͼ
		sampGrayReg(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(imgSeg);//��ͼ�����Ͻǵ�Ϊpt3,��͸�Ϊw_b,h_b
		Mat templSeg;//ģ���ͼ
		templGray(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(templSeg);
		Mat diffSeg;//�����ͼ
		diffBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(diffSeg);

		int pos_x = pt3.x + w_b / 2;
		int pos_y = pt3.y + h_b / 2;

		
		Rect rect1;//ȱ�ݱ�Ƿ���
		int w_s = 10, h_s = 10;//ȱ�ݾ��ο���չ�Ŀ�͸�
		rect1.x = rect.x - pt3.x - w_s / 2;
		rect1.y = rect.y - pt3.y - h_s / 2;
		rect1.height = rect.height + h_s;
		rect1.width = rect.width + w_s;

		//��ֹ����Խ��
		if (rect1.x < 0) rect1.x = 0;
		if (rect1.y < 0) rect1.y = 0;
		if (rect1.x + rect1.width > imgSeg.cols - 1) rect1.width = imgSeg.cols - 1 - rect1.x;
		if (rect1.y + rect1.height > imgSeg.rows - 1) rect1.height = imgSeg.rows - 1 - rect1.y;

		cvtColor(imgSeg, imgSeg, cv::COLOR_GRAY2BGR);
		rectangle(imgSeg, rect1, CV_RGB(255, 0, 0), 2);
		Size sz = diffBw.size();
		defectNum++;//����ȱ�ݼ���
		pos_x = sz.width*currentCol + pos_x;//ȱ��������ͼ���еĺ�����
		pos_y = sz.height*runtimeParams->currentRow_detect + pos_y;//ȱ��������ͼ���е�������


		//����׼�������ͼ�Ŀ�¡�ϻ���ȱ��(�ų�����αȱ�ݺ��ٻ���
		Rect rec = Rect((int)(rect_out.x*scalingFactor), (int)(rect_out.y*scalingFactor), (int)(rect_out.width*scalingFactor), (int)(rect_out.height*scalingFactor));
		rectangle(sampGrayRegCopyZoom, rec, Scalar(0, 0, 255), 1);

		string defectInfo = defect_str[defect_flag];
		if (percentFlag)
			defectInfo += to_string(int(percentage));
	
		cv::putText(sampGrayRegCopyZoom, defectInfo, Point(rect.x, rect.y), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0), 1, 8, 0);

		QChar fillChar = '0'; //���ַ������Ȳ���ʱʹ�ô��ַ��������
		QString outPath = runtimeParams->currentOutputDir + "/"; //��ǰ��Ŷ�Ӧ�����Ŀ¼
		outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(pos_x, 5, 10, fillChar).arg(pos_y, 5, 10, fillChar).arg(defect_flag);
		outPath += userConfig->ImageFormat; //���ͼ���ʽ�ĺ�׺
		//cv::putText(imgSeg, to_string(factor),Point(0,imgSeg.rows-1), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0, 255, 255), 1, 8, 0);


		//����ȱ�����꣬ȱ�����ͣ���ȱ�ݷ�ͼ
		cv::Point3i detailPoints{ pos_x,pos_y,defect_flag };
		detailImage.insert(std::make_pair(detailPoints, imgSeg));
		//imwrite(outPath.toStdString(), imgSeg); //��ͼ
		
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
*	���ܣ�����͹�����ȱʧ����
*	���룺
*		templBw: ģ���ֵͼ��
*		change_point:�仯��
*       angle:�����Ƕ�
*   �����͹���ȱʧ����
*/
float DetectFunc::bulge_missing_line_width(cv::Mat &templBw, std::vector<cv::Point> change_point, int angle)
{
	Point correspond0, correspond1;//��·�϶ԳƵı仯��
	float line0_length = 0, line1_length = 0, line_length = 0;//ȱ�������߿�
	int traverse_number0 = 0, traverse_number1 = 0;
	//Point change0_pos = Point(change_point[0].x + rect_out.x, change_point[0].y + rect_out.y);//�仯���ڷ�ͼ�ϵ�����
	//Point change1_pos = Point(change_point[1].x + rect_out.x, change_point[1].y + rect_out.y);
	int boundary_x = templBw.cols - 1, boundary_y = templBw.rows - 1;//����ʱ��������ͼ�߽��ֹͣ������ͬʱ�õ���Ӧ���߿�
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
*	���ܣ�����͹�����ȱʧ�ٷֱ�
*	���룺
*		templBw: ģ���ֵͼ��
*		change_point:�仯��
*       minAreaRect_length:ȱ����ת���εĳ���
*   �����͹���ȱʧ�ٷֱ�
*/
float DetectFunc::bulge_missing_percentage(cv::Mat &templBw, std::vector<cv::Point> change_point, cv::Point2f *rectPointsOfPart)
{
	vector<Point2i> neighbors{ Point2i(change_point[0].x + 1,change_point[0].y),/*Point2i(change_point[0].x + 2,change_point[0].y),Point2i(change_point[0].x + 3,change_point[0].y ),*//*Point2i(change_point[0].x - 4,change_point[0].y - 4),*/
			Point2i(change_point[0].x - 1,change_point[0].y)/*,Point2i(change_point[0].x - 2,change_point[0].y ),Point2i(change_point[0].x -3,change_point[0].y )*//*,Point2i(change_point[0].x + 4,change_point[0].y + 4)*/ ,
		 Point2i(change_point[0].x ,change_point[0].y - 1),/*Point2i(change_point[0].x ,change_point[0].y - 2),Point2i(change_point[0].x ,change_point[0].y - 3),*/
			Point2i(change_point[0].x ,change_point[0].y + 1)/*,Point2i(change_point[0].x ,change_point[0].y + 2),Point2i(change_point[0].x ,change_point[0].y + 3)*/ };//�仯���������������
	float change_line = 0, vertical_line = 0, vertical_angle = 0, change_angle = 0, vertical_rangle = 0;//��仯��б�ʣ�����б�ʣ����߽Ƕȣ��Լ�ȷ���෨������ı�������
	float rect_line0, rect_angle0, rect_line1, rect_angle1;
	float flaw_width, flaw_height, flaw_length;//ȱ�ݵĿ���
	flaw_width = sqrt((rectPointsOfPart[1].x - rectPointsOfPart[0].x)*(rectPointsOfPart[1].x - rectPointsOfPart[0].x) + (rectPointsOfPart[1].y - rectPointsOfPart[0].y)*(rectPointsOfPart[1].y - rectPointsOfPart[0].y)) - 10;//��ת��������������10���أ�������ת���εı߳���Ҫ��ԭ
	flaw_height = sqrt((rectPointsOfPart[2].x - rectPointsOfPart[1].x)*(rectPointsOfPart[2].x - rectPointsOfPart[1].x) + (rectPointsOfPart[2].y - rectPointsOfPart[1].y)*(rectPointsOfPart[2].y - rectPointsOfPart[1].y)) - 10;
	if (change_point[1].y - change_point[0].y == 0)//���仯�������ƽ����ˮƽ��
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
		vertical_angle = -90;//ȷ���仯�����ߵķ��߷���
		vertical_rangle = 90;

		//}

		if (rectPointsOfPart[1].y == rectPointsOfPart[0].y || rectPointsOfPart[1].y == rectPointsOfPart[2].y)
		{
			//float flaw_width, flaw_height;//��ת���εĿ�͸�
			/*flaw_width = sqrt((rectPointsOfPart[1].x - rectPointsOfPart[0].x)*(rectPointsOfPart[1].x - rectPointsOfPart[0].x) + (rectPointsOfPart[1].y - rectPointsOfPart[0].y)*(rectPointsOfPart[1].y - rectPointsOfPart[0].y));
			flaw_height = sqrt((rectPointsOfPart[2].x - rectPointsOfPart[1].x)*(rectPointsOfPart[2].x - rectPointsOfPart[1].x) + (rectPointsOfPart[2].y - rectPointsOfPart[1].y)*(rectPointsOfPart[2].y - rectPointsOfPart[1].y));*/
			flaw_length = flaw_width;
			//�õ���ת���εĶ̱�
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
			//��ת���εĿ�͸�

			flaw_length = flaw_width;
			//�õ���ת���εĶ̱�
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


	vector<int> angle = { 0,45,90,135,180,-180,-135,-90,-45 };//��������
	vector<float> angle_sub, rangle_sub;
	int k;
	for (k = 0; k < 9; k++)
	{
		float sub = fabs(vertical_angle - angle[k]);
		angle_sub.push_back(sub);//���߷������������֮��ļ���
	}
	for (k = 0; k < 9; k++)
	{
		float sub = fabs(vertical_rangle - angle[k]);
		rangle_sub.push_back(sub);//���߷������������֮��ļ���
	}
	float min_sub = angle_sub[0];
	int min_pos = 0;
	for (k = 0; k < 9; k++)
	{
		if (min_sub > angle_sub[k])
		{
			min_sub = angle_sub[k];//���߷���������������С��ֵ
			min_pos = k;
		}
	}
	float min_rsub = rangle_sub[0];
	int min_rpos = 0;
	for (k = 0; k < 9; k++)
	{
		if (min_rsub > rangle_sub[k])
		{
			min_rsub = rangle_sub[k];//���߷���������������С��ֵ
			min_rpos = k;
		}
	}
	float  line_length = 0, rline_length = 0;//ȱ�������߿�
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
	cout << "ȱʧ�ٷֱ�: " << bulge_missing_percentage << "%" << endl;


	return bulge_missing_percentage;
}

void DetectFunc::save(const std::string& path, Mat& image_template_gray) {//ʵ����û�е��ã�ֻ��Ϊ��ͳһ
	Mat temp;
	cv::pyrDown(image_template_gray, temp);	
	if (userConfig->matchingAccuracyLevel == 2)//�;���
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
