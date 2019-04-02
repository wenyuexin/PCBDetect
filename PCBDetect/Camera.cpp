#include "Camera.h"

Camera::~Camera()
{
	for (int i = 0; i < nDevice; i++)
		camList[i].release();
}


//�����ʼ��
//�������˳�����豸˳��һ��ʱ����Ҫʹ�õڶ�������
//�������ʵ������˳���Ӧ���豸��Ž��г�ʼ��,ϵͳ�豸��Ŵ�0��ʼ
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

//����
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
