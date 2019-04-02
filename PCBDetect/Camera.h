#pragma once

#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include <vector>


class Camera {

private:
	int nDevice; //������豸����
	std::vector<cv::VideoCapture> camList;

public:
	Camera(int n=5) : nDevice(n) {}
	~Camera();

public:
	bool init(const std::vector<int>& iv={});//��ʼ��
	void takePicture(Ui::CvMatArray &, int currentRow ,int num);//��������
	bool resetDeviceNum(int, std::vector<int> iv = {}); //�趨��������豸��
	int  getDeviceNum(); //���ؽ�������豸��

};