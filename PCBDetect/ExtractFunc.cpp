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

/***************** ��ȡ ******************/

//���������ߴ��ģ��ͼ��
void ExtractFunc::generateBigTempl()
{
	Size originalfullImgSize(adminConfig->ImageSize_W * runtimeParams->nCamera,
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); //��ͼ��ԭʼ�ߴ�
	big_templ = Mat(originalfullImgSize, CV_8UC3);
}


/*
 * ���ܣ�ͨ��ͼ��ƥ���ҵ�ͼ����ĸ��ǵ��λ�ã��ֱ�Ϊ���ϣ����£����ϣ�����,�����ݽǵ�λ�ö�ͼ������и��ȥ�߽ǣ�
 * ���룺ͼ����������num_cols,ͼ����������num_cols,����ͼ��λ�õı��num��ԭʼͼ��image
 * �������ʾL�ͽǵ�λ�õ����꣬�������и�Ľ��ͼ���棬����mask
 */
Mat ExtractFunc::findLocationMark(int col, Mat &image) {
	int currentCol = col + 1;
	int currentRow = runtimeParams->currentRow_extract + 1;
	int nCamera = runtimeParams->nCamera;
	int nPhotographing = runtimeParams->nPhotographing;

	shape = Mat::zeros(Size(100, 100), CV_8UC1);//����һ������ֵȫΪ0��ͼ��λ��8λ�޷�����������ͨ���ĻҶ�ͼ

	if (currentCol == 1 && currentRow == 1) { //����
		for (i = 10; i < 60; i++) {
			for (j = 10; j < 12; j++) {
				shape.at<uchar>(i, j) = 255;//�������ص���at
				shape.at<uchar>(j, i) = 255;
			}
		}
		roi = image(Rect(100, 100, 200, 200));//Rect�Ĳ�����x,y,width,height,101-1,����ƥ��ĸ���Ȥ����roi��Ҫд�ɶ�ֵ
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//�ҶȻ�
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//��ֵ��
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//ģ��ƥ�䣬TM_CCOEFFϵ��ƥ�䷨�����ֵ
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);//���ƥ��ͼ����Сֵ�����ֵ��λ��
		lf1.x = maxloc.x + 100;//��ԭʼͼ���е�λ��
		lf1.y = maxloc.y + 100;
		//cout << "n1_lf1:" << lf1 << endl;//������Ͻ�����

		//�õ�����Ȥ����
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = lf1.x;
		rect.y = lf1.y;
		rect.width = image.cols - lf1.x;
		rect.height = image.rows - lf1.y;
		mask(rect).setTo(255);//�������ʵ���ģ����������������ֵΪ255	
	}

	if (currentCol == 1 && currentRow == nPhotographing) { //����
		for (i = 50; i < 100; i++) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(j, i) = 255;
			}
		}
		for (i = 50; i > 0; i--) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(i, j) = 255;//�������ص���at
			}
		}
		roi = image(Rect(30, 2600, 200, 200));//Rect�Ĳ�����x,y,width,height,101-4
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//�ҶȻ�
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//��ֵ��
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//ģ��ƥ��
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
		lf2.x = minloc.x + 30;
		lf2.y = minloc.y + 2600 + shape.rows;
		//cout << "n1_lf2:" << lf2 << endl;//������½�����

		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = lf2.x;
		rect.y = 0;
		rect.width = image.cols - lf2.x;
		rect.height = lf2.y;
		mask(rect).setTo(255);

	}

	if (currentCol == nCamera && currentRow == 1) { //����
		for (i = 50; i < 100; i++) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(i, j) = 255;//�������ص���at
			}
		}
		for (i = 50; i > 0; i--) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(j, i) = 255;//�������ص���at
			}
		}
		roi = image(Rect(3600, 100, 200, 200));//Rect�Ĳ�����x,y,width,height,105-1
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//�ҶȻ�
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//��ֵ��
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//ģ��ƥ��
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
		br1.x = minloc.x + 3600 + shape.cols;
		br1.y = minloc.y + 100;
		//cout << "n1_br1:" << br1 << endl;//������Ͻ�����

		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = br1.y;
		rect.width = br1.x;
		rect.height = image.rows - br1.y;
		mask(rect).setTo(255);

	}

	if (currentCol == nCamera && currentRow == nPhotographing) { //���½�
		for (i = 50; i > 0; i--) {
			for (j = 50; j < 52; j++) {
				shape.at<uchar>(i, j) = 255;//�������ص���at
				shape.at<uchar>(j, i) = 255;//�������ص���at
			}
		}
		roi = image(Rect(3650, 2650, 200, 200));//Rect�Ĳ�����x,y,width,height,105-4
		cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);//�ҶȻ�
		threshold(roi_gray, roi_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);//��ֵ��
		matchTemplate(roi_bw, shape, match, cv::TM_CCOEFF);//ģ��ƥ��
		minMaxLoc(match, NULL, NULL, &minloc, &maxloc);
		br2.x = minloc.x + 3650 + shape.cols;
		br2.y = minloc.y + 2650 + shape.rows;
		//cout << "n1_br2:" << br2 << endl;//������½�����

		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = br2.x;
		rect.height = br2.y;
		mask(rect).setTo(255);

	}

	if (currentCol == 1 && 1 < currentRow && currentRow < nPhotographing)//���
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = lf1.x;
		rect.y = 0;
		rect.width = image.cols - lf1.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (currentCol == nCamera && 1 < currentRow && currentRow< nPhotographing)//�ұ�
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = br1.x;
		rect.height = image.rows;
		mask(rect).setTo(255);
	}

	if (currentRow == 1 && 1 < currentCol && currentCol < nCamera)//�ϱ�
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = lf1.y;
		rect.width = image.cols;
		rect.height = image.rows - lf1.y;
		mask(rect).setTo(255);
	}

	if (currentRow == nPhotographing && 1 < currentCol && currentCol < nCamera)//�±�
	{
		mask = Mat::zeros(image.size(), image.type());//ԭʼ��ģ
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = image.cols;
		rect.height = lf2.y;
		mask(rect).setTo(255);
	}

	if (1 < currentCol && currentCol < nCamera && 1 < currentRow && currentRow < nPhotographing) {
		mask = Mat::ones(image.size(), CV_8UC1) * 255;//ԭʼ��ģ
	}
	return mask;
}


void ExtractFunc::save(const std::string& path, Mat& image_template_gray) {
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

void ExtractFunc::load(const std::string& path) {
	cv::FileStorage store(path, cv::FileStorage::READ);
	cv::FileNode n1 = store["keypoints"];
	cv::read(n1, keypoints);
	cv::FileNode n2 = store["descriptors"];
	cv::read(n2, descriptors);
	store.release();
}