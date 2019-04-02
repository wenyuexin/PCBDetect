#pragma once

#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include <vector>


class Camera {

private:
	int nDevice; //接入的设备总数
	std::vector<cv::VideoCapture> camList;

public:
	Camera(int n=5) : nDevice(n) {}
	~Camera();

public:
	bool init(const std::vector<int>& iv={});//初始化
	void takePicture(Ui::CvMatArray &, int currentRow ,int num);//进行拍摄
	bool resetDeviceNum(int, std::vector<int> iv = {}); //设定接入的总设备数
	int  getDeviceNum(); //返回接入的总设备数

};