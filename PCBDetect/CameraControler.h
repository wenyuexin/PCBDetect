#pragma once

#include "Configurator.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <map>
#include <iterator>


namespace pcb {
	class CameraControler;
}

//相机控制器
class CameraControler : public QThread
{
	Q_OBJECT

public:
	//相机的相关操作
	enum Operation {
		NoOperation,
		InitCameras,
		TakePhoto
	};

	//相机的错误代码
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x400,
		InitFailed = 0x401,
		InvalidCameraNum = 0x402
	};

private:
	pcb::CvMatArray *cvmatSamples; //用于检测的样本图
	int *currentRow; //当前行号
	int *nCamera; //当前使用的相机个数
	int *MaxCameraNum; //可使用的相机总数

	std::vector<int> deviceIndex = {}; //设备号
	std::vector<cv::VideoCapture> cameraList; //相机列表
	std::map<int, bool> cameraState; //相机状态 <设备号,状态值>
	ErrorCode errorCode = Uncheck; //控制器的错误码
	Operation operation = NoOperation;//操作指令

public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCvMatSamples(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }
	inline void setCameraNum(int *num) { nCamera = num; }
	inline void setMaxCameraNum(int *num) { MaxCameraNum = num; }
	inline void setDeviceIndex(std::vector<int> &iv) { deviceIndex = iv; }
	inline void setOperation(Operation op) { operation = op; }

	ErrorCode initCameras();//初始化
	QString cameraStatusMapToString(); //相机状态转字符串
	ErrorCode takePhotos();//进行拍摄
	ErrorCode resetDeviceIndex(std::vector<int> iv = {}); //设定接入的总设备数
	inline bool checkCameraState(int index) { return cameraState[index]; }

	inline ErrorCode getErrorCode() { return errorCode; } //获取当前的错误代码
	bool showMessageBox(QWidget *parent); //弹窗警告

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);

};
