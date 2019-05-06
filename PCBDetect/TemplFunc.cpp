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

/***************** ��ȡ ******************/

//���������ߴ��ģ��ͼ��
void TemplFunc::generateBigTempl()
{
	Size templSize = Size(adminConfig->ImageSize_W * detectParams->nCamera,
		adminConfig->ImageSize_H * detectParams->nPhotographing);
	big_templ = Mat(templSize, CV_8UC3);
}

//�ַ���ת����
void TemplFunc::str2int(int &int_temp, const std::string &string_temp)
{
	std::stringstream stream(string_temp);
	stream >> int_temp;
}

/**
*���ܣ�Ѱ��pcb������½ǵ�L�ͱ�־�Ĵ�������
*���룺pcb���½�ͼ��image��pcb�����½ǵ�x����lf_x��y����lf_y
*�������pcb������½�����ȡ��L�ͱ�־�ľ��ο�����½�����ֵ������point_left
*/
Point TemplFunc::corner_lf(cv::Mat &image, int lf_x, int lf_y)
{

	shape = Mat::zeros(Size(100, 100), CV_8UC1);//����һ������ֵȫΪ0��ͼ��λ��8λ�޷�����������ͨ���ĻҶ�ͼ
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
			shape.at<uchar>(i, j) = 255;//�������ص���at
		}

	}
	roi_shape = shape(Rect(46, 0, 50, 54));//�õ�ģ��ĸ���Ȥ����

	roi_x = lf_x;
	roi_y = lf_y - length;//��ȡ��pcb��length����ľ���
	roi_image = image(Rect(roi_x,roi_y, length, length));//Rect�Ĳ�����x,y,width,height,101-4

	cvtColor(roi_image, roi_gray, COLOR_BGR2GRAY);//�ҶȻ�
	threshold(roi_gray, roi_bw, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);//��ֵ��
	matchTemplate(roi_bw, roi_shape, match, TM_CCOEFF);//ģ��ƥ��
	minMaxLoc(match, NULL, NULL, &minloc, &maxloc);

	//lf2.x = maxloc.x;
	//lf2.y = maxloc.y + 2500 + shape.rows;
	//cout << "201_lf2:" << lf2 << endl;//������½�����

	point_leftup = maxloc;//���Ͻǣ�lf��
	point_left.x = point_leftup.x + lf_x;
	point_left.y = point_leftup.y + shape.rows + roi_y;
	return point_left;

}
/**
*���ܣ�Ѱ��pcb������Ͻǵ�L�ͱ�־�Ĵ�������
*���룺pcb�����Ͻ�ͼ��image��pcb�����½ǵ�x����br_x��y����br_y
*�������pcb������½�����ȡ��L�ͱ�־�ľ��ο�����Ͻ�����ֵ������point_left
*/
Point TemplFunc::corner_br(cv:: Mat &image, int br_x, int br_y)
{
	shape = Mat::zeros(Size(100, 100), CV_8UC1);//����һ������ֵȫΪ0��ͼ��λ��8λ�޷�����������ͨ���ĻҶ�ͼ
	for (int i = 50; i < 100; i++)
	{
		for (int j = 50; j < 52; j++)
		{
			shape.at<uchar>(i, j) = 255;//�������ص���at
		}
	}
	for (int i = 50; i > 0; i--)
	{
		for (int j = 50; j < 52; j++)
		{
			shape.at<uchar>(j, i) = 255;//�������ص���at
		}
	}

	roi_shape = shape(Rect(0, 48, 54, 50));
	//imwrite("roi1_lf2.jpg", roi1);

	roi_x = br_x - length;
	roi_y = br_y;//��ȡ��pcb��length����ľ���
	roi_image = image(Rect(roi_x, roi_y, length, length));//Rect�Ĳ�����x,y,width,height,101-4

	cvtColor(roi_image, roi_gray, COLOR_BGR2GRAY);//�ҶȻ�
	threshold(roi_gray, roi_bw, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);//��ֵ��
	matchTemplate(roi_bw, roi_shape, match, TM_CCOEFF);//ģ��ƥ��
	minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
	point_rightup = maxloc;//���Ͻǣ�lf��
	point_right.x = point_rightup.x + shape.cols + roi_x;
	point_right.y = point_rightup.y + br_y;
	return point_right;

}

/**
*���ܣ�����L�ͱ�־���ǵ�λ�ã���ͼ������и��ȥ�߽ǣ�
*���룺ͼ����������num_cols,ͼ����������num_cols,����ͼ��λ�õı��num��ͼ��������cols,ͼ��������rows,ԭʼͼ��image��ͼ�����½Ǻ����Ͻǵ�L������
*����������и�Ľ��ͼ���棬����mask
*/
Mat TemplFunc::cutting(int num_cols, int num_rows, int cols, int rows, cv::Mat &image, Point point_left, Point point_right)
{
	if (num_cols == 1 && num_rows == 1)
		/*if (num_cols == 1 && num_rows == 2)*/
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = point_left.x;
		rect.y = point_right.y;
		rect.width = image.cols - point_left.x;
		rect.height = image.rows - point_right.y;
		mask(rect).setTo(255);//�������ʵ���ģ����������������ֵΪ255
	}
	if (num_cols == 1 && num_rows == rows)
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
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
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
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
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
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
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
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
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = point_right.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (num_rows == 1 && 1 < num_cols && num_cols < cols)
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = point_right.y;
		rect.width = image.cols;
		rect.height = image.rows - point_right.y;
		mask(rect).setTo(255);
	}

	if (num_rows == rows && 1 < num_cols && num_cols < cols)
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = image.cols;
		rect.height = point_left.y;
		mask(rect).setTo(255);
	}

	if (1 < num_cols && num_cols < cols && 1 < num_rows && num_rows < rows)
	{
		mask = Mat::ones(image.size(), CV_8UC1)*255;//ԭʼ��ģ
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