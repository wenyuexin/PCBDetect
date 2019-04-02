#include "Camera.h"

Camera::~Camera()
{
	for (int i = 0; i < nDevice; i++)
		camList[i].release();
}


//相机初始化
//相机排列顺序与设备顺序不一致时，需要使用第二个参数
//输入相机实际排列顺序对应的设备编号进行初始化,系统设备编号从0开始
bool Camera::init(const std::vector<int> &iv)
{
	if (iv.size() == 0) {
		for (int i = 0; i < nDevice; i++) {
			camList.push_back(cv::VideoCapture(i));
			if (!camList[i].isOpened()) return false;
		}
	}
	else {
		for (int i = 0; i < iv.size(); i++) {
			camList.push_back(cv::VideoCapture(iv[i]));
			if (!camList[i].isOpened()) return false;
		}
	}
}

//拍摄
void Camera::takePicture(Ui::CvMatArray &mat_array, int currentRow, int num)
{
	if (num > camList.size())
		num = camList.size();
	
	cv::Mat frame;
	for (int i = 0; i < num; i++) {
		camList[i] >> frame;
		cv::Mat* pMat = new cv::Mat(frame);
		mat_array[currentRow][i] = pMat;
	}
}

bool Camera::resetDeviceNum(int n,std::vector<int> iv )
{
	int old_num = nDevice;
	nDevice = n;
	for (int i = 0; i < old_num; i++)
		camList[i].release();
	return init(iv);
}

int Camera::getDeviceNum()
{
	return this->nDevice;
}
