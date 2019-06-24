#pragma once

#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <map>
#include <iterator>
#include "AMC98C.h"
#include <CameraApi.h>


//相机控制器
class CameraControler : public QThread
{
	Q_OBJECT

public:
	//相机的相关操作
	enum Operation {
		NoOperation,
		InitCameras,
		TakePhotos
	};

	//相机的错误代码
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x500,
		InitFailed = 0x501,
		InvalidCameraNum = 0x502,
		TakePhotosFailed = 0x503,
		Default = 0x5FF
	};

private:
	int caller; //模块的调用者
	pcb::CvMatArray *cvmatSamples; //用于检测的样本图
	int *currentRow; //当前行号

	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数

	std::vector<int> deviceIndex = {}; //设备号
	std::vector<cv::VideoCapture> cameraList; //相机列表
	std::map<int, bool> cameraState; //相机状态 <设备号,状态值>

	std::vector<CameraHandle> cameraList2;
	tSdkCameraDevInfo sCameraList[12]; //最多不能超过12台相机
	int CameraNums = 12;

	double *pfExposureTime = NULL;
	tSdkImageResolution sImageSize;

	ErrorCode errorCode; //控制器的错误码
	Operation operation; //操作指令

public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCaller(int c) { caller = c; };
	inline int getCaller() { return caller; }

	inline void setCvMatSamples(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	inline void setDeviceIndex(std::vector<int> &iv) { deviceIndex = iv; }
	inline void setOperation(Operation op) { operation = op; }

	ErrorCode resetDeviceIndex(std::vector<int> iv = {}); //设定接入的总设备数
	inline bool getCameraState(int index) { return cameraState[index]; }

	inline bool isReady() { return errorCode == NoError; }
	inline ErrorCode getErrorCode() { return errorCode; } //获取当前的错误代码
	bool showMessageBox(QWidget *parent); //弹窗警告

private:
	ErrorCode initCameras(); //初始化 - OpenCV
	bool initCameras2(); //初始化 - 迈德威视
	QString cameraStatusMapToString(); //相机状态转字符串

	bool isCamerasInitialized(); //判断相机是否已经初始化
	void closeCameras();//关闭已经打开的相机

	ErrorCode takePhotos();//拍摄
	void takePhotos2();

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);
};
