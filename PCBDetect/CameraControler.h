#pragma once

#include "Configurator.h"
#include "opencv2/opencv.hpp"
#include <QThread>

//相机控制器
class CameraControler : public QThread
{
	Q_OBJECT

public:
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x400,
		CameraInitFailed = 0x401,
		InvalidCameraNum = 0x402
	};

private:
	Ui::CvMatArray *cvmatSamples; //用于检测的样本图
	int *currentRow; //当前行号
	int *nCamera; //当前使用的相机个数
	int *MaxCameraNum; //可使用的相机总数
	std::vector<cv::VideoCapture> cameraList;//相机列表
	std::vector<bool> availableCameras; //可使用的相机


public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCvMatSamples(Ui::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }
	inline void setCameraNum(int *num) { nCamera = num; }
	inline void setMaxCameraNum(int *num) { MaxCameraNum = num; }

	ErrorCode initCameras(const std::vector<int>& iv = {});//初始化
	//inline bool isInitialized() { return (errorCode == CameraControler::NoError); } //判断时候已经初始化

	void takePhotos();//进行拍摄
	ErrorCode resetDeviceNum(int, std::vector<int> iv = {}); //设定接入的总设备数

	static void showMessageBox(QWidget *parent, CameraControler::ErrorCode code);

protected:
	void run();
	
Q_SIGNALS:
	void takePhotoFinished_camera();

};
