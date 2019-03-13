#include "DetectFunc.h"


using Ui::DetectConfig;
using Ui::DetectParams;
using std::vector;
using std::string;
using std::to_string;
using cv::Mat;
using cv::Point;
using cv::Point2i;
using cv::Point2f;
using cv::Vec4i;
using cv::Rect;
using cv::Size;
using cv::imwrite;


/***************** 检测 ******************/
/**
*功能：使用BRISK算法获取配准点做配准
*输入：
*	im1Gray:样本灰度图
*	im2Gray:模板灰度图
*	im1Reg:样本配准后的灰度图
*   h:单应性矩阵
*   imMatched:绘制的匹配点
*/

void DetectFunc::alignImages(Mat &im1Gray, Mat &im2Gray, Mat &im1Reg, Mat &h, Mat &imMatches)
{
	time_t t0 = clock();
	// Variables to store keypoints and descriptors
	vector<cv::KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;
	// Detect BRISK features and compute descriptors.
	cv::Ptr<cv::Feature2D> orb = cv::BRISK::create(50);
	//cv::Ptr<Feature2D> orb = ORB::create(MAX_FEATURES);
	orb->detectAndCompute(im1Gray, Mat(), keypoints1, descriptors1);
	orb->detectAndCompute(im2Gray, Mat(), keypoints2, descriptors2);

	time_t t1 = clock();
	qDebug() << QString::fromLocal8Bit("特征查找：") << double(t1 - t0) / CLOCKS_PER_SEC << endl;

	// Match features.
	std::vector<cv::DMatch> matches;
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::NORM_HAMMING);


	matcher->match(descriptors1, descriptors2, matches, Mat());

	time_t t2 = clock();
	qDebug() << QString::fromLocal8Bit("特征匹配：") << double(t2 - t1) / CLOCKS_PER_SEC << endl;

	// Sort matches by score
	std::sort(matches.begin(), matches.end());

	// Remove not so good matches
	const int numGoodMatches = matches.size() * GOOD_MATCH_PERCENT;
	matches.erase(matches.begin() + numGoodMatches, matches.end());

	time_t t3 = clock();
	qDebug() << QString::fromLocal8Bit("匹配筛选：") << double(t3 - t2) / CLOCKS_PER_SEC << endl;

	// Draw top matches
	Mat im1, im2;
	cvtColor(im1Gray, im1, CV_GRAY2BGR);
	cvtColor(im2Gray, im2, CV_GRAY2BGR);
	drawMatches(im1, keypoints1, im2, keypoints2, matches, imMatches);

	time_t t4 = clock();
	qDebug() << QString::fromLocal8Bit("匹配绘制：") << double(t4 - t3) / CLOCKS_PER_SEC << endl;
	//imwrite("matches.jpg", imMatches);


	// Extract location of good matches
	std::vector<cv::Point2f> points1, points2;
	for (size_t i = 0; i < matches.size(); i++)
	{
		points1.push_back(keypoints1[matches[i].queryIdx].pt);
		points2.push_back(keypoints2[matches[i].trainIdx].pt);
	}

	time_t t5 = clock();
	qDebug() << QString::fromLocal8Bit("提取匹配坐标：") << double(t5 - t4) / CLOCKS_PER_SEC << endl;

	// Find homography
	//至少需要四对点
	h = findHomography(points1, points2, cv::LMEDS);

	time_t t6 = clock();
	qDebug() << QString::fromLocal8Bit("计算单应性矩阵：") << double(t6 - t5) / CLOCKS_PER_SEC << endl;

	// Use homography to warp image
	warpPerspective(im1Gray, im1Reg, h, im2Gray.size());

	cv::imwrite("imgReg.bmp", im1Reg);

	time_t t7 = clock();
	qDebug() << QString::fromLocal8Bit("变换：") << double(t7 - t6) / CLOCKS_PER_SEC << endl;

}


/**
 *
 * 功能：计算向量X和向量Y的相关系数
 * 输入：X 向量1
 *       Y 向量2
 * 返回：相关系数[-1,1],越靠近0越不相关
 *
 */
float DetectFunc::correlationCoefficient(const vector<double> &X, const vector<double> & Y)
{
	int n = X.size();
	int sum_X = 0, sum_Y = 0, sum_XY = 0;
	int squareSum_X = 0, squareSum_Y = 0;

	for (int i = 0; i < n; i++)
	{
		// sum of elements of array X. 
		sum_X = sum_X + X[i];

		// sum of elements of array Y. 
		sum_Y = sum_Y + Y[i];

		// sum of X[i] * Y[i]. 
		sum_XY = sum_XY + X[i] * Y[i];

		// sum of square of array elements. 
		squareSum_X = squareSum_X + X[i] * X[i];
		squareSum_Y = squareSum_Y + Y[i] * Y[i];
	}

	// use formula for calculating correlation coefficient. 
	float corr = (float)(n * sum_XY - sum_X * sum_Y)
		/ sqrt((n * squareSum_X - sum_X * sum_X)
			* (n * squareSum_Y - sum_Y * sum_Y));

	return corr;
}

/**
 * imgOut:模板灰度图
 * imgOut2:样本灰度图
 */
cv::Mat DetectFunc::sub_process(cv::Mat &imgOut, cv::Mat &imgOut2) {
	//阈值处理
	Mat imgFlaw;
	cv::absdiff(imgOut2, imgOut, imgFlaw);
	cv::threshold(imgFlaw, imgFlaw, 30, 255, cv::THRESH_BINARY);

	//形态学处理，先开后闭，这里的处理与最小线宽有关
	cv::Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_b);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_b);


	//膨胀模板边缘，与形态学处理后的图片相乘，获取边界上的点消除
	cv::Mat edges;
	cv::Canny(imgOut, edges, 150, 50);
	cv::Mat element_a = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::dilate(edges, edges, element_a);
	cv::Mat points_Edge;
	cv::bitwise_and(edges, imgFlaw, points_Edge);

	vector<cv::Point2i> morph_num;
	cv::findNonZero(imgFlaw, morph_num);
	//cout << "总像素:" << morph_num.size() << endl;


	//返回形态学处理后的非0坐标
	vector<cv::Point2i> locations;
	cv::findNonZero(points_Edge, locations);
	//cout << "位于边缘的像素:" << locations.size() << endl;;
	int cal = 0;
	for (auto xy : locations) {
		vector<cv::Point2i> neighbors{ cv::Point2i(xy.x - 1,xy.y - 1),cv::Point2i(xy.x,xy.y - 1),cv::Point2i(xy.x + 1,xy.y - 1),
									cv::Point2i(xy.x - 1,xy.y),cv::Point2i(xy.x,xy.y),cv::Point2i(xy.x + 1,xy.y),
									cv::Point2i(xy.x - 1,xy.y + 1),cv::Point2i(xy.x,xy.y + 1),cv::Point2i(xy.x + 1,xy.y + 1)
		};
		vector<double> vec1;
		vector<double> vec2;
		for (int i = 0; i < 9; i++) {
			cv::Point2i p = neighbors[i];
			if (p.x < 0 || p.x >= imgOut.cols || p.y < 0 || p.y >= imgOut.rows)
				continue;
			vec1.push_back(imgOut.at<uchar>(p.y, p.x));
			vec2.push_back(imgOut2.at<uchar>(p.y, p.x));
		}
		if (vec1.size() < 4 || vec2.size() < 4)
			continue;

		double cor = abs(correlationCoefficient(vec1, vec2));
		if (cor > 0.5) {
			imgFlaw.at<uchar>(xy.y, xy.x) = 0;
			cal++;
		}
	}

	//再进行一次形态学处理
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_OPEN, element_b);
	cv::morphologyEx(imgFlaw, imgFlaw, cv::MORPH_CLOSE, element_b);

	return imgFlaw;
}
/**
*	功能：标记并存储缺陷图片和位置信息
*	输入：
*		diff_Bw: 差值图像，覆盖回到与样本等大的纯黑图像对应位置
*		src:配准后的样本灰度图
*		temp_bw: 模板二值图像，覆盖到与样本等大的纯黑图像对应位置
*		model_num,batch_num,pcb_num:型号，批次号，样本编号
*		cal:调用次数
*		off_x，off_y:样本相对模板的偏移量
*/
void DetectFunc::markDefect(Mat &diffBw, Mat &src, Mat &temp_bw,Mat &templ_reg,int &defectNum,int currentCol) {
	Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	dilate(diffBw, diffBw, kernel_small);//对差值图像做膨胀，方便对类型进行判断

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i>   hierarchy;
	cv::findContours(diffBw, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//如果存在缺陷，则按照给定PCB型号，批次号，编号建立目录，存储图片,不存在缺陷则结束
	if (contours.size() == 0)
		return;
	string batch_path = (config->OutputDirPath).toStdString() + "\\" + params->sampleModelNum.toStdString();//检查输出文件夹中型号文件是否存在
	if (0 != _access(batch_path.c_str(), 0))
		_mkdir(batch_path.c_str());
	string num_path = batch_path + "\\" + params->sampleBatchNum.toStdString();//检查批次号文件夹是否存在
	if (0 != _access(num_path.c_str(), 0) && contours.size() > 0)
		_mkdir(num_path.c_str());
	string out_path = num_path + "\\" + params->sampleNum.toStdString();//检查编号文件夹是否存在
	if (0 != _access(out_path.c_str(), 0) && contours.size() > 0)
		_mkdir(out_path.c_str());


	for (int i = 0; i < contours.size(); i++) {

		int w_b = 300, h_b = 300;//缺陷分图的大小
		if (contourArea(contours[i], false) <= 50 && contourArea(contours[i], false) >= 200)//可判定为缺陷的最小面积
			continue;
		else {
			//pt3是300×300分图在diffBw中的左上角坐标
			Rect rect = boundingRect(Mat(contours[i]));
			Point pt1, pt2, pt3, pt4;
			pt1.x = rect.x;
			pt1.y = rect.y;
			pt2.x = rect.x + rect.width;
			pt2.y = rect.y + rect.height;
			pt3.x = (pt2.x + pt1.x) / 2 - w_b / 2;
			pt3.y = (pt2.y + pt1.y) / 2 - h_b / 2;

			//防止分图越界,如果分图imgSeg超出src边界则往回收缩
			if (pt3.x < 0)
				pt3.x = 0;
			if (pt3.y < 0)
				pt3.y = 0;
			if (pt3.x + w_b > src.cols - 1)
				w_b = src.cols - 1 - pt3.x;
			if (pt3.y + h_b > src.rows - 1)
				h_b = src.rows - 1 - pt3.y;

			Mat imgSeg;//最后要保存和显示的分图
			src(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(imgSeg);//分图的左上角点为pt3,宽和高为w_b,h_b
			Mat templSeg;//模板分图
			templ_reg(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(templSeg);
			Mat diffSeg;//差异分图
			diffBw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(diffSeg);

			int pos_x = pt3.x + w_b / 2;
			int pos_y = pt3.y + h_b / 2;

			//对缺陷进行分类
			Mat temp_part;
			temp_bw(Rect(pt3, Point(pt3.x + w_b, pt3.y + h_b))).copyTo(temp_part);//模板
			Mat diff_part = Mat::zeros(Size(temp_part.cols, temp_part.rows), CV_8UC1);
			cv::RotatedRect minRect = minAreaRect(contours[i]);//轮廓的外接旋转矩形
			Point2f rect_points[4];
			minRect.points(rect_points);//获取旋转矩形的4个顶点
			for (int j = 0; j < 4; j++)
			{
				line(diff_part, Point2f(rect_points[j].x - pt3.x, rect_points[j].y - pt3.y), Point2f(rect_points[(j + 1) % 4].x - pt3.x, rect_points[(j + 1) % 4].y - pt3.y), (255, 255, 255));
			}

			//变换次数
			int trans_num = 0;
			vector<vector<Point>> contours_rect;
			vector<Vec4i>   hierarchy_rect;
			findContours(diff_part, contours_rect, hierarchy_rect, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, Point(0, 0));

			for (int k = 1; k < contours_rect[0].size(); k++) {
				Point pre = contours_rect[0][k - 1];
				Point temp = contours_rect[0][k];
				//cout << "(" << temp << "):" << (int)temp_part.at<uchar>(temp) << endl;
				if ((int)temp_part.at<uchar>(pre) != (int)temp_part.at<uchar>(temp))
					trans_num++;
			}


			if (trans_num == 0)//缺陷不跨越线路直接忽略
				continue;

			//是否有缺失
			cv::Moments m = cv::moments(contours_rect[0]);
			cv::Point2f contour_center = Point2f(static_cast<float>(m.m10 / (m.m00 + 1e-5)),
				static_cast<float>(m.m01 / (m.m00 + 1e-5)));//通过缺陷轮廓矩计算缺陷轮廓中心


			vector<Point2i> neighbors{ Point2i(contour_center.x - 1,contour_center.y - 1),Point2i(contour_center.x,contour_center.y - 1),Point2i(contour_center.x + 1,contour_center.y - 1),
							Point2i(contour_center.x - 1,contour_center.y),Point2i(contour_center.x,contour_center.y),Point2i(contour_center.x + 1,contour_center.y),
							Point2i(contour_center.x - 1,contour_center.y + 1),Point2i(contour_center.x,contour_center.y + 1),Point2i(contour_center.x + 1,contour_center.y + 1)
			};
			int neighbors_sum = 0;
			for (int i = 0; i < 9; i++) {
				neighbors_sum += (int)temp_part.at<uchar>(neighbors[i]);
			}
			int lack_flag = 0;//0表示无缺失，1表示有缺失
			if (neighbors_sum >= 255 * 2)
				lack_flag = 1;

			int defect_flag = 0;
			if (lack_flag) {
				defect_flag = trans_num > 2 ? 1 : 2;
			}
			else {
				defect_flag = trans_num > 2 ? 3 : 4;
			}

			vector<string> defect_str{ "","断路","缺失","短路","凸起" };


			//imwrite(out_path + "\\" + to_string(cal) + "_" + to_string(pos_x) + "_" + to_string(pos_y) + "_diff_part.bmp", diff_part);
			//imwrite(out_path + "\\" + to_string(cal) + "_" + to_string(pos_x) + "_" + to_string(pos_y) + "_temp_part.bmp", temp_part);

			//在分图上标记
			Rect rect1;
			int w_s = 10, h_s = 10;//缺陷矩形框扩展的宽和高
			rect1.x = rect.x - pt3.x - w_s / 2;
			rect1.y = rect.y - pt3.y - h_s / 2;
			rect1.height = rect.height + h_s;
			rect1.width = rect.width + w_s;

			//防止方框越界
			if (rect1.x < 0)
				rect1.x = 0;
			if (rect1.y < 0)
				rect1.y = 0;
			if (rect1.x + rect1.width > imgSeg.cols - 1)
				rect1.width = imgSeg.cols - 1 - rect1.x;
			if (rect1.y + rect1.height > imgSeg.rows - 1)
				rect1.height = imgSeg.rows - 1 - rect1.y;

			rectangle(imgSeg, rect1, CV_RGB(255, 0, 0), 2);
			defectNum++;//增加缺陷计数
			pos_x = (params->imageSize).width()*currentCol + pos_x;//缺陷在整体图像中的横坐标
			pos_y = (params->imageSize).height()*params->currentRow_detect + pos_y;//缺陷在整体图像中的纵坐标

			//imwrite(out_path + "\\" + to_string(defectNum) + "_" + to_string(pos_x) + "_" + to_string(pos_y) + "_" + defect_str[defect_flag] + "." + detectConfig->ImageFormat.toStdString(), diffSeg);
			//imwrite(out_path + "\\" + to_string(defectNum) + "_" + to_string(pos_x) + "_" + to_string(pos_y) + "_" + to_string(trans_num) + "_模板." + detectConfig->ImageFormat.toStdString(), templSeg);
			imwrite(out_path + "\\" + to_string(defectNum) + "_" + to_string(pos_x) + "_" + to_string(pos_y) + "_" + to_string(defect_flag) + config->ImageFormat.toStdString(), imgSeg);

		}
	}
}
