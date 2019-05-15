#include "DetectFunc.h"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <qDebug>


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
//using namespace cv::flann;


DetectFunc::DetectFunc()
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	detectResult = Q_NULLPTR; //�����
}

DetectFunc::~DetectFunc()
{
	qDebug() << "~DetectFunc";
}

//���������ߴ��ȱ�ݼ��ͼ��
void DetectFunc::generateBigTempl()
{
	Size originalfullImgSize = Size(adminConfig->ImageSize_W * runtimeParams->nCamera, 
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); //��ͼ��ԭʼ�ߴ�
	
	double factorW = 1.0 * runtimeParams->ScreenRect.width() / originalfullImgSize.width;
	double factorH = 1.0 * runtimeParams->ScreenRect.height() / originalfullImgSize.height;
	scalingFactor = qMin(factorW, factorH); //��������

	scaledFullImageSize = Size(scalingFactor * originalfullImgSize.width,
		scalingFactor * originalfullImgSize.height); //��ͼ�������ź�ĳߴ�

	scaledSubImageSize = Size(scalingFactor * adminConfig->ImageSize_W,
		scalingFactor * adminConfig->ImageSize_H); //��ͼ�������ź�ĳߴ�

	big_templ = Mat(scaledFullImageSize, CV_8UC3); //�������ڼ�¼ȱ�ݵ���ͼ
}


bool DetectFunc::alignImages_test_load(std::vector<KeyPoint> &keypoints_1, Mat& descriptors_1, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches)
{

	Ptr<SURF> detector = SURF::create(500, 4, 4, true, true);
	std::vector<KeyPoint> keypoints_2;
	Mat descriptors_2;

	double t1 = clock();
	cv::Mat pyr;
	cv::Size sz = image_sample_gray.size();
	//pyrDown(image_sample_gray, pyr, cv::Size(int(sz.width*0.125), int(sz.height*0.125)));
	pyrDown(image_sample_gray, pyr);
	pyrDown(pyr, pyr);
	pyrDown(pyr, pyr);

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



	//Mat outImg;
	//drawKeypoints(image_template, keypoints_1, image_template,cv::Scalar::all(-1),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//imwrite("outImg.jpg", image_template);

	std::sort(m_knnMatches.begin(), m_knnMatches.end(), [](const vector<DMatch> m1, const vector<DMatch> m2) {return m1[0].distance < m2[0].distance; });

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
		H.at<double>(0, 2) *= 8;
		H.at<double>(1, 2) *= 8;
		H.at<double>(2, 0) /= 8;
		H.at<double>(2, 1) /= 8;
		warpPerspective(image_sample_gray, imgReg, H, image_sample_gray.size());
	}

	return true;
}

bool DetectFunc::alignImages_test(Mat &image_template_gray, Mat &image_sample_gray, Mat &imgReg, Mat &H, Mat &imMatches) {
	//Ptr<SURF> detector = SURF::create(3500, 3, 3, true, true);
	Ptr<SURF> detector = SURF::create(100, 4, 4, true, true);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;

	double t1 = clock();
	Mat pyrTemp, pyrSamp;
	pyrDown(image_template_gray, pyrTemp);
	pyrDown(pyrTemp, pyrTemp);
	//pyrDown(pyrTemp, pyrTemp);

	pyrDown(image_sample_gray, pyrSamp);
	pyrDown(pyrSamp, pyrSamp);
	//pyrDown(pyrSamp, pyrSamp);

	detector->detectAndCompute(pyrTemp, Mat(), keypoints_1, descriptors_1);
	detector->detectAndCompute(pyrSamp, Mat(), keypoints_2, descriptors_2);

	double t2 = clock();
	cout << "��ȡ������ʱ��" << double(t2 - t1) / CLOCKS_PER_SEC << endl;

	Ptr<cv::flann::IndexParams> indexParams = new cv::flann::KDTreeIndexParams(5);
	Ptr<cv::flann::SearchParams> searchParams;
	FlannBasedMatcher matcher(indexParams);
	vector<DMatch> matches;
	vector<vector<DMatch>> m_knnMatches;

	const float minRatio = 0.7;
	matcher.knnMatch(descriptors_1, descriptors_2, m_knnMatches, 2);


	std::sort(m_knnMatches.begin(), m_knnMatches.end(), [](const vector<DMatch> m1, const vector<DMatch> m2) {return m1[0].distance < m2[0].distance; });

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
		H.at<double>(0, 2) *= 4;
		H.at<double>(1, 2) *= 4;
		H.at<double>(2, 0) /= 4;
		H.at<double>(2, 1) /= 4;
		warpPerspective(image_sample_gray, imgReg, H, image_sample_gray.size());
	}
	return true;
}

/**
 *�Ƚ�����ֵ�����ٽ��в�ֵ����
 */
Mat DetectFunc::sub_process_new(Mat &templBw, Mat &sampBw, Mat& mask_roi) {
	Mat imgFlaw;
	cv::absdiff(templBw, sampBw, imgFlaw);
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
//void DetectFunc::markDefect_new(Mat &diffBw, Mat &sampGrayReg, Mat &templBw, Mat &templGray, int &defectNum, int currentCol) {
//	Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
//	dilate(diffBw, diffBw, kernel_small);//�Բ�ֵͼ�������ͣ���������ͽ����ж�
//
//	std::vector<std::vector<cv::Point>> contours;
//	std::vector<cv::Vec4i>   hierarchy;
//	cv::findContours(diffBw, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
//
//	//�������ȱ�ݣ����ո���PCB�ͺţ����κţ���Ž���Ŀ¼���洢ͼƬ,������ȱ�������
//	if (contours.size() == 0) return;
//
//
//	for (int i = 0; i < contours.size(); i++) {
//		if (currentCol == 4 && runtimeParams->currentRow_detect == 0 && i == 87)
//			qDebug() << endl;
//		int w_b = 300, h_b = 300;//ȱ�ݷ�ͼ�Ĵ�С
//		if (contourArea(contours[i], false) <= 50 && contourArea(contours[i], false) >= 200)//ȱ�ݵ�������С���
//			continue;
//		else {
//			//pt3��300��300��ͼ��diffBw�е����Ͻ�����
//			Rect rect = boundingRect(Mat(contours[i]));
//			Point pt1, pt2, pt3, pt4;
//			pt1.x = rect.x;
//			pt1.y = rect.y;
//			pt2.x = rect.x + rect.width;
//			pt2.y = rect.y + rect.height;
//			pt3.x = (pt2.x + pt1.x) / 2 - w_b / 2;
//			pt3.y = (pt2.y + pt1.y) / 2 - h_b / 2;
//
//			//��ֹ��ͼԽ��,�����ͼimgSeg����src�߽�����������
//			if (pt3.x < 0)
//				pt3.x = 0;
//			if (pt3.y < 0)
//				pt3.y = 0;
//			if (pt3.x + w_b > sampGrayReg.cols - 1)
//				w_b = sampGrayReg.cols - 1 - pt3.x;
//			if (pt3.y + h_b > sampGrayReg.rows - 1)
//				h_b = sampGrayReg.rows - 1 - pt3.y;
//
//			Mat imgSeg;//��׼��������ͼ
//			sampGrayReg(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(imgSeg);//��ͼ�����Ͻǵ�Ϊpt3,��͸�Ϊw_b,h_b
//			Mat templSeg;//ģ���ͼ
//			templGray(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(templSeg);
//			Mat diffSeg;//�����ͼ
//			diffBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(diffSeg);
//
//			int pos_x = pt3.x + w_b / 2;
//			int pos_y = pt3.y + h_b / 2;
//
//			//��ȱ�ݽ��з���
//			Mat temp_part;//��ֵ��ģ���ͼ
//			templBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(temp_part);
//			Mat diff_part = Mat::zeros(Size(temp_part.cols, temp_part.rows), CV_8UC1);
//			cv::RotatedRect minRect = minAreaRect(contours[i]);//�����������ת����
//			Point2f rect_points[4];
//			minRect.points(rect_points);//��ȡ��ת���ε�4������
//			Point2f rect_points_move[4];
//	
//			for (int j = 0; j < 4; j++)
//			{
//				Point2f start = (rect_points[j].x - pt3.x, rect_points[j].y - pt3.y);
//				Point2f end = (rect_points[(j + 1) % 4].x - pt3.x, rect_points[(j + 1) % 4].y - pt3.y);
//				line(diff_part, Point2f(rect_points[j].x - pt3.x, rect_points[j].y - pt3.y), Point2f(rect_points[(j + 1) % 4].x - pt3.x, rect_points[(j + 1) % 4].y - pt3.y), (255, 255, 255));
//			}
//
//			//�任����
//			int trans_num = 0;
//			vector<vector<Point>> contours_rect;
//			vector<Vec4i>   hierarchy_rect;
//			findContours(diff_part, contours_rect, hierarchy_rect, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, Point(0, 0));
//
//			for (int k = 1; k < contours_rect[0].size(); k++) {
//				Point pre = contours_rect[0][k - 1];
//				Point temp = contours_rect[0][k];
//				//cout << "(" << temp << "):" << (int)temp_part.at<uchar>(temp) << endl;
//				if ((int)temp_part.at<uchar>(pre) != (int)temp_part.at<uchar>(temp))
//					trans_num++;
//			}
//
//			Mat diff_part_big = Mat::zeros(Size(temp_part.cols * 2, temp_part.rows * 2), CV_8UC1);
//
//
//			if (trans_num == 0)//ȱ�ݲ���Խ��·ֱ�Ӻ���
//				continue;
//
//			//�Ƿ���ȱʧ
//			cv::Moments m = cv::moments(contours_rect[0]);
//			cv::Point2f contour_center = Point2f(static_cast<float>(m.m10 / (m.m00 + 1e-5)),
//				static_cast<float>(m.m01 / (m.m00 + 1e-5)));//ͨ��ȱ�������ؼ���ȱ����������
//
//
//			vector<Point2i> neighbors{ Point2i(contour_center.x - 1,contour_center.y - 1),Point2i(contour_center.x,contour_center.y - 1),Point2i(contour_center.x + 1,contour_center.y - 1),
//							Point2i(contour_center.x - 1,contour_center.y),Point2i(contour_center.x,contour_center.y),Point2i(contour_center.x + 1,contour_center.y),
//							Point2i(contour_center.x - 1,contour_center.y + 1),Point2i(contour_center.x,contour_center.y + 1),Point2i(contour_center.x + 1,contour_center.y + 1)
//			};
//			int neighbors_sum = 0;
//
//
//			for (int i = 0; i < 9; i++) {
//
//				neighbors_sum += (int)temp_part.at<uchar>(neighbors[i]);
//			}
//			int lack_flag = 0;//0��ʾ��ȱʧ��1��ʾ��ȱʧ
//			if (neighbors_sum >= 255 * 2)
//				lack_flag = 1;
//
//			int defect_flag = 0;
//			if (lack_flag) {
//				//��·����ȱʧ
//				defect_flag = trans_num > 2 ? 1 : 2;
//			}
//			else {
//				//��·����͹��
//				defect_flag = trans_num > 2 ? 3 : 4;
//			}
//
//			vector<string> defect_str{ "","��·","ȱʧ","��·","͹��" };
//			//�ڷ�ͼ�ϱ��
//			Rect rect1;
//			int w_s = 10, h_s = 10;//ȱ�ݾ��ο���չ�Ŀ�͸�
//			rect1.x = rect.x - pt3.x - w_s / 2;
//			rect1.y = rect.y - pt3.y - h_s / 2;
//			rect1.height = rect.height + h_s;
//			rect1.width = rect.width + w_s;
//
//			//��ֹ����Խ��
//			if (rect1.x < 0)
//				rect1.x = 0;
//			if (rect1.y < 0)
//				rect1.y = 0;
//			if (rect1.x + rect1.width > imgSeg.cols - 1)
//				rect1.width = imgSeg.cols - 1 - rect1.x;
//			if (rect1.y + rect1.height > imgSeg.rows - 1)
//				rect1.height = imgSeg.rows - 1 - rect1.y;
//
//			rectangle(imgSeg, rect1, CV_RGB(255, 0, 0), 2);
//			Size sz = diffBw.size();
//			defectNum++;//����ȱ�ݼ���
//			pos_x = sz.width*currentCol + pos_x;//ȱ��������ͼ���еĺ�����
//			pos_y = sz.height*runtimeParams->currentRow_detect + pos_y;//ȱ��������ͼ���е�������
//			imwrite(out_path + "\\" + to_string(defectNum) + "_" + to_string(pos_x) + "_" + to_string(pos_y) + "_" + to_string(defect_flag) + userConfig->ImageFormat.toStdString(), imgSeg);
//		}
//	}
//}

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
void DetectFunc::markDefect_test(Mat &diffBw, Mat &sampGrayReg, Mat &templBw, Mat &templGray, int &defectNum, int currentCol) {
	Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	dilate(diffBw, diffBw, kernel_small);//�Բ�ֵͼ�������ͣ���������ͽ����ж�

	Mat sampGrayRegCopy = sampGrayReg.clone();
	cvtColor(sampGrayRegCopy, sampGrayRegCopy, cv::COLOR_GRAY2BGR);
	cv::Mat sampGrayRegCopyZoom;
	cv::resize(sampGrayRegCopy, sampGrayRegCopyZoom, scaledSubImageSize, (0, 0), (0, 0), cv::INTER_LINEAR);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i>   hierarchy;
	cv::findContours(diffBw, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//�������ȱ�ݣ����ո���PCB�ͺţ����κţ���Ž���Ŀ¼���洢ͼƬ,������ȱ�������
	if (contours.size() == 0) {
		Point pos(currentCol*scaledSubImageSize.width, runtimeParams->currentRow_detect*scaledSubImageSize.height);
		Rect roiRect = Rect(pos, scaledSubImageSize);
		Mat roi = getBigTempl(roiRect);
		sampGrayRegCopy.copyTo(roi);
		return;
	}

	//batch_path = (userConfig->OutputDirPath).toStdString() + "\\" + runtimeParams->sampleModelNum.toStdString();//�������ļ������ͺ��ļ��Ƿ����
	//if (0 != _access(batch_path.c_str(), 0))
	//	_mkdir(batch_path.c_str());
	//num_path = batch_path + "\\" + runtimeParams->sampleBatchNum.toStdString();//������κ��ļ����Ƿ����
	//if (0 != _access(num_path.c_str(), 0) && contours.size() > 0)
	//	_mkdir(num_path.c_str());
	//out_path = num_path + "\\" + runtimeParams->sampleNum.toStdString();//������ļ����Ƿ����
	//if (0 != _access(out_path.c_str(), 0) && contours.size() > 0)
	//	_mkdir(out_path.c_str());


	for (int i = 0; i < contours.size(); i++) {
		if (contourArea(contours[i], false) <= 60){//ȱ����С���
			continue;
		}	
		Rect rectCon = boundingRect(Mat(contours[i]));
		int larger = 20;//��΢����ȱ�����ڵľ�������
		Rect rect_out = Rect(rectCon.x - larger, rectCon.y - larger, rectCon.width + 2 * larger, rectCon.height + 2 * larger);
		Mat temp_area;
		Mat samp_area;
		try {
			temp_area = templGray(rect_out);
			samp_area = sampGrayReg(rect_out);
		}
		catch (std::exception e) {
			qDebug()<<QString::fromLocal8Bit("����Խ��");
		};

		//�ṹ������
		auto msssim = getMSSIM(temp_area, samp_area);
		if (msssim[0] >= 0.85)
			continue;


		//��ȱ�����ڵ�С��ͼ���д���
		int meanTemp = mean(temp_area)[0];
		Mat tempAreaBw;
		threshold(temp_area, tempAreaBw, meanTemp, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
		int meanSamp = mean(samp_area)[0];
		Mat sampAreaBw;
		threshold(samp_area, sampAreaBw, meanSamp, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
		Mat diffPart;
		Mat smallRoi = cv::Mat::ones(sampAreaBw.size(), sampAreaBw.type()) * 255;
		diffPart = sub_process_new(tempAreaBw, sampAreaBw, smallRoi);
		cv::Mat partMask = cv::Mat::zeros(sampAreaBw.size(), CV_8UC1);
		drawContours(partMask, contours, i, 255, -1, 8, Mat(), 0, Point2f(-rect_out.x, -rect_out.y));
		cv::bitwise_and(partMask, diffPart, diffPart);
		vector<cv::Point2i> locations;
		cv::findNonZero(diffPart, locations);

		if (locations.size() <= 60)
			continue;
	
		//����ȱ�ݷ�ͼ������ȱ�ݷ���
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
		if (pt3.x < 0)
			pt3.x = 0;
		if (pt3.y < 0)
			pt3.y = 0;
		if (pt3.x + w_b > sampGrayReg.cols - 1)
			w_b = sampGrayReg.cols - 1 - pt3.x;
		if (pt3.y + h_b > sampGrayReg.rows - 1)
			h_b = sampGrayReg.rows - 1 - pt3.y;

		Mat imgSeg;//��׼��������ͼ
		sampGrayReg(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(imgSeg);//��ͼ�����Ͻǵ�Ϊpt3,��͸�Ϊw_b,h_b
		Mat templSeg;//ģ���ͼ
		templGray(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(templSeg);
		Mat diffSeg;//�����ͼ
		diffBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(diffSeg);

		int pos_x = pt3.x + w_b / 2;
		int pos_y = pt3.y + h_b / 2;


		//�����ֵͼ���ܲ����ɿ�����������¶�ֵ��
		//��ȱ�ݽ��з���
		Mat temp_part;//��ֵ��ģ���ͼ
		templBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(temp_part);
		Mat diff_part = Mat::zeros(Size(temp_part.cols, temp_part.rows), CV_8UC1);
		cv::RotatedRect minRect = minAreaRect(contours[i]);//�����������ת����
		Point2f rect_points[4];
		minRect.points(rect_points);//��ȡ��ת���ε�4������
		Point2f rect_points_move[4];

		//������ת���ο�����bug,���ǲ�Ӱ����
		for (int j = 0; j < 4; j++)
		{
			Point2f start = (rect_points[j].x - pt3.x, rect_points[j].y - pt3.y);
			Point2f end = (rect_points[(j + 1) % 4].x - pt3.x, rect_points[(j + 1) % 4].y - pt3.y);
			line(diff_part, Point2f(rect_points[j].x - pt3.x, rect_points[j].y - pt3.y), Point2f(rect_points[(j + 1) % 4].x - pt3.x, rect_points[(j + 1) % 4].y - pt3.y), (255, 255, 255));
		}

		//��ת��������·���ϻҶȱ任����
		int trans_num = 0;
		vector<vector<Point>> contours_rect;
		vector<Vec4i>   hierarchy_rect;
		findContours(diff_part, contours_rect, hierarchy_rect, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, Point(0, 0));

		int neighbors_sum = 0;
		if (contours_rect.size() == 0)
			goto classfinish;
		//continue;
		{
			try {
				for (int k = 1; k < contours_rect[0].size(); k++) {
					Point pre = contours_rect[0][k - 1];
					Point temp = contours_rect[0][k];
					if ((int)temp_part.at<uchar>(pre) != (int)temp_part.at<uchar>(temp))
						trans_num++;
				}
			}
			catch (std::exception) {
				qDebug() << QString::fromLocal8Bit("��ת��������������");
				//cout << "��ת��������������";
			}


			Mat diff_part_big = Mat::zeros(Size(temp_part.cols * 2, temp_part.rows * 2), CV_8UC1);
			//if (currentCol == 4 && params->currentRow_detect == 0&&i==87) {
			//	int idx = 0;
			//	for (; idx >= 0; idx = hierarchy_rect[idx][0])
			//	{
			//		Scalar color(rand() & 255, rand() & 255, rand() & 255);
			//		drawContours(diff_part, contours_rect, idx, color, 1, 8, hierarchy_rect);
			//		drawContours(diff_part_big, contours_rect, idx, color, 1, 8, hierarchy_rect);
			//		
			//	}
			//	imwrite("diffSeg.jpg", diffSeg);
			//	imwrite("diff_part.jpg", diff_part);
			//	imwrite("diff_part_big.jpg", diff_part_big);
			//}

			//if (trans_num == 0)//ȱ�ݲ���Խ��·ֱ�Ӻ���
			//	continue;


			//�Ƿ���ȱʧ
			cv::Moments m = cv::moments(contours[i]);
			cv::Point2f contour_center = Point2f(static_cast<float>(m.m10 / (m.m00 + 1e-5)),
				static_cast<float>(m.m01 / (m.m00 + 1e-5)));//ͨ��ȱ�������ؼ���ȱ����������
			contour_center.x -= pt3.x;
			contour_center.y -= pt3.y;

			vector<Point2i> neighbors{ Point2i(contour_center.x - 1,contour_center.y - 1),Point2i(contour_center.x,contour_center.y - 1),Point2i(contour_center.x + 1,contour_center.y - 1),
							Point2i(contour_center.x - 1,contour_center.y),Point2i(contour_center.x,contour_center.y),Point2i(contour_center.x + 1,contour_center.y),
							Point2i(contour_center.x - 1,contour_center.y + 1),Point2i(contour_center.x,contour_center.y + 1),Point2i(contour_center.x + 1,contour_center.y + 1)
			};


			try {
				for (int i = 0; i < 9; i++) {
					neighbors_sum += (int)temp_part.at<uchar>(neighbors[i]) > 0 ? 255 : 0;
				}
			}
			catch (std::exception e) {
				qDebug() << QString::fromLocal8Bit("ȱ����������������");
				//cout << "ȱ����������������";
			}
		}

		//������bug����������ֱ��goto��������ɱ�ǩ
	classfinish:

		int lack_flag = 0;//0��ʾ��ȱʧ��1��ʾ��ȱʧ
		if (neighbors_sum >= 255 * 2)
			lack_flag = 1;

		int defect_flag = 0;
		if (lack_flag) {
			defect_flag = trans_num > 2 ? 1 : 2;
		}
		else {
			defect_flag = trans_num > 2 ? 3 : 4;
		}

		vector<string> defect_str{ "","��·","ȱʧ","��·","͹��" };

		//�ڷ�ͼ�ϱ��
		Rect rect1;
		int w_s = 10, h_s = 10;//ȱ�ݾ��ο���չ�Ŀ�͸�
		rect1.x = rect.x - pt3.x - w_s / 2;
		rect1.y = rect.y - pt3.y - h_s / 2;
		rect1.height = rect.height + h_s;
		rect1.width = rect.width + w_s;

		//��ֹ����Խ��
		if (rect1.x < 0)
			rect1.x = 0;
		if (rect1.y < 0)
			rect1.y = 0;
		if (rect1.x + rect1.width > imgSeg.cols - 1)
			rect1.width = imgSeg.cols - 1 - rect1.x;
		if (rect1.y + rect1.height > imgSeg.rows - 1)
			rect1.height = imgSeg.rows - 1 - rect1.y;

		cvtColor(imgSeg, imgSeg, cv::COLOR_GRAY2BGR);
		rectangle(imgSeg, rect1, CV_RGB(255, 0, 0), 2);
		Size sz = diffBw.size();
		defectNum++;//����ȱ�ݼ���
		pos_x = sz.width*currentCol + pos_x;//ȱ��������ͼ���еĺ�����
		pos_y = sz.height*runtimeParams->currentRow_detect + pos_y;//ȱ��������ͼ���е�������


		//����׼�������ͼ�Ŀ�¡�ϻ���ȱ��(�ų�����αȱ�ݺ��ٻ���
		Rect rec = Rect((int)(rect_out.x*scalingFactor), (int)(rect_out.y*scalingFactor), (int)(rect_out.width*scalingFactor), (int)(rect_out.height*scalingFactor));
		rectangle(sampGrayRegCopyZoom, rec, Scalar(0, 0, 255), 2);

		QChar fillChar = '0'; //���ַ������Ȳ���ʱʹ�ô��ַ��������
		QString outPath = runtimeParams->currentOutputDir + "/"; //��ǰ��Ŷ�Ӧ�����Ŀ¼
		outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(pos_x, 5, 10, fillChar).arg(pos_y, 5, 10, fillChar).arg(defect_flag);
		outPath += userConfig->ImageFormat; //���ͼ���ʽ�ĺ�׺
		imwrite(outPath.toStdString(), imgSeg); //��ͼ
	}

	Point roiPosition(currentCol*scaledSubImageSize.width, runtimeParams->currentRow_detect*scaledSubImageSize.height);
	Mat roi = getBigTempl(Rect(roiPosition, scaledSubImageSize));
	sampGrayRegCopyZoom.copyTo(roi);
}


void DetectFunc::save(const std::string& path, Mat& image_template_gray) 
{
	Mat temp;
	cv::pyrDown(image_template_gray, temp);
	cv::pyrDown(temp, temp);
	cv::pyrDown(temp, temp);
	Ptr<SURF> detector = SURF::create(3500, 3, 3, true, true);
	detector->detectAndCompute(temp, Mat(), keypoints, descriptors);
	cv::FileStorage store(path, cv::FileStorage::WRITE);
	cv::write(store, "keypoints", keypoints);
	cv::write(store, "descriptors", descriptors);
	store.release();

}
void DetectFunc::load(const std::string& path) 
{
	cv::FileStorage store(path, cv::FileStorage::READ);
	cv::FileNode n1 = store["keypoints"];
	cv::read(n1, keypoints);
	cv::FileNode n2 = store["descriptors"];
	cv::read(n2, descriptors);
	store.release();
}

Scalar DetectFunc::getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2

	/*************************** END INITS **********************************/

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

	///////////////////////////////// FORMULA ////////////////////////////////
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
