#pragma once
//#include"Windows.h"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <map>
#include <iterator>
#include "AMC98C.h"
#include "Windows.h"
#include <CameraApi.h>
#include "GenICam/CAPI/SDK.h"
#include "ImageConvert.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>



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

	int32_t status;//状态

	//OPT
	std::vector<GENICAM_Camera*> pCameraList2;
	/*GENICAM_Frame *pFrame = NULL;*/
	GENICAM_System *pSystem = NULL;//发现设备、创建常用属性对象
	GENICAM_Camera *pCamera = NULL;//相机对象接口类
	GENICAM_Camera *pCameraList = NULL;
	GENICAM_StreamSource *pStreamSource = NULL;
	GENICAM_AcquisitionControl *pAcquisitionCtrl = NULL;//下属性操作类
	GENICAM_AcquisitionControlInfo acquisitionControlInfo = { 0 };

	bool m_bRun; //显示线程标志

	uint32_t cameraCnt = 12;//相机数目
	HANDLE threadHandle;
	unsigned threadID;//线程数
	int cameraIndex = -1;


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
	/*static int isGrabbingFlag ;*/
	static cv::Mat* pImageFrame;

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
	//OPT
	//断开连接，相机
	int32_t GENICAM_disconnect(GENICAM_Camera *pGetCamera);
	//连接相机
	int32_t GENICAM_connect(GENICAM_Camera *pGetCamera);
	//修改曝光时间
	int32_t modifyCamralExposureTime(GENICAM_Camera *pGetCamera);
	//修改图像宽度
	int32_t modifyCameraWidth(GENICAM_Camera *pGetCamera);
	//修改图像高度
	int32_t modifyCameraHeight(GENICAM_Camera *pGetCamera);
	//修改相机X方向
	int32_t modifyCameraReverseX(GENICAM_Camera *pGetCamera);
	//设置触发方式
	int32_t setSoftTriggerConf(GENICAM_AcquisitionControl *pAcquisitionCtrl);
	//创建流对象
	int32_t GENICAM_CreateStreamSource(GENICAM_Camera *pGetCamera, GENICAM_StreamSource **ppStreamSource);
	//开始抓流
	int32_t GENICAM_startGrabbing(GENICAM_StreamSource *pStreamSource);
	//停止抓流
	int32_t GENICAM_stopGrabbing(GENICAM_StreamSource *pStreamSource);
	//执行触发
	int32_t executeTriggerSoftware(GENICAM_AcquisitionControl *pAcquisitionCtrl);

	ErrorCode initCameras(); //初始化 - OpenCV
	bool initCameras2(); //初始化 - 迈德威视
	bool initCamerasOPT();//初始化-OPT
	QString cameraStatusMapToString(); //相机状态转字符串

	bool isCamerasInitialized(); //判断相机是否已经初始化
	void closeCameras();//关闭已经打开的相机
	void closeCamerasOPT();//关闭已经打开的相机-OPT

	ErrorCode takePhotos();//拍摄
	void takePhotos2();
	void takePhotos3();//拍摄图像 - OPT

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);
};
void onGetFrame(GENICAM_Frame* pFrame);
