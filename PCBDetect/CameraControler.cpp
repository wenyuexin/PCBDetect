#include "CameraControler.h"


CameraControler::CameraControler(QThread *parent)
	: QThread(parent)
{
}

CameraControler::~CameraControler()
{
	for (int i = 0; i < cameraList.size(); i++)
		cameraList[i].release();
}

void CameraControler::run()
{
	//此处调用拍照函数
	//拍摄结束后将结果存入 (*cvmatSamples)[*currentRow] 中
	takePhotos();
	emit takePhotoFinished_camera();
}

/******************* 相机 *********************/

//相机初始化
//相机排列顺序与设备顺序不一致时，需要使用第二个参数
//输入相机实际排列顺序对应的设备编号进行初始化,系统设备编号从0开始
CameraControler::ErrorCode CameraControler::initCameras(const std::vector<int> &iv)
{
	*nCamera = 1;

	if (*nCamera <= 0)
		return CameraControler::InvalidCameraNum;

	CameraControler::ErrorCode errorCode = CameraControler::NoError;
	if (iv.size() == 0) { //使用默认的设备号初始化
		if (*nCamera > *MaxCameraNum) //判断调用的相机个数是否过多
			return CameraControler::InvalidCameraNum;

		availableCameras.resize(*nCamera); 
		for (int i = 0; i < *nCamera; i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(i));
			availableCameras[i] = cameraList[i].isOpened();
			if (!availableCameras[i]) 
				errorCode = CameraControler::CameraInitFailed;
		}
	}
	else { //使用设定的设备号初始化
		if (*nCamera > iv.size()) //判断调用的相机个数是否过多
			return CameraControler::InvalidCameraNum;

		for (int i = 0; i < iv.size(); i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(iv[i]));
			availableCameras[i] = cameraList[i].isOpened();
			if (!availableCameras[i])
				errorCode = CameraControler::CameraInitFailed;
		}
	}
	return errorCode;
}

//拍摄图像
void CameraControler::takePhotos()
{
	ErrorCode errorCode = CameraControler::NoError;

	cv::Mat frame;
	for (int i = 0; i < *nCamera; i++) {
		cameraList[i] >> frame;
		cv::Mat* pMat = new cv::Mat(frame);
		(*cvmatSamples)[*currentRow][i] = pMat;
	}
}

//设置设备号
CameraControler::ErrorCode CameraControler::resetDeviceNum(int nDevice, std::vector<int> iv)
{
	//int old_num = *MaxCameraNum;
	//*MaxCameraNum = nDevice;
	//for (int i = 0; i < old_num; i++)
	//	cameraList[i].release();
	return initCameras(iv);
}


//参数报错
void CameraControler::showMessageBox(QWidget *parent, CameraControler::ErrorCode code) {
	QString message;
	switch (code)
	{
	case CameraControler::Uncheck:
		message = QString::fromLocal8Bit("相机状态未确认"); break;
	case CameraControler::InvalidCameraNum:
		message = QString::fromLocal8Bit("当前调用的相机个数无效"); break;
	case CameraControler::CameraInitFailed:
		message = QString::fromLocal8Bit("相机初始化失败"); break;
	default:
		message = QString::fromLocal8Bit("未知错误"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
		message + "!    \n" +
		"CameraControler: ErrorCode: " + QString::number(code),
		QString::fromLocal8Bit("确定"));
}