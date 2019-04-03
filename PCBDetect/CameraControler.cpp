#include "CameraControler.h"


CameraControler::CameraControler(QThread *parent)
	: QThread(parent)
{
}

CameraControler::~CameraControler()
{
	//关闭相机设备
	for (int i = 0; i < cameraList.size(); i++)
		cameraList[i].release();
}

//启动线程
void CameraControler::run()
{
	switch (operation) {
	case Operation::InitCameras: //初始化
		initCameras();
		emit initCamerasFinished_camera(errorCode);
		break;
	case Operation::TakePhoto: //拍照
	    //拍摄结束后将结果存入 (*cvmatSamples)[*currentRow] 中
		takePhotos();
		emit takePhotosFinished_camera(errorCode);
		break;
	case Operation::NoOperation:
	default:
		break;
	}
}

/******************* 相机 *********************/

//相机初始化
//相机排列顺序与设备顺序不一致时，需要使用第二个参数
//输入相机实际排列顺序对应的设备编号进行初始化,系统设备编号从0开始
CameraControler::ErrorCode CameraControler::initCameras()
{
	if (*nCamera <= 0)
		return errorCode = CameraControler::InvalidCameraNum;

	errorCode = CameraControler::NoError;
	if (deviceIndex.size() == 0) { //使用默认的设备号初始化
		if (*nCamera > *MaxCameraNum) //判断调用的相机个数是否过多
			return errorCode = CameraControler::InvalidCameraNum;

		//availableCameras.put(*nCamera); 
		for (int i = 0; i < *nCamera; i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(i));
			cameraState[i] = cameraList[i].isOpened(); //判断相机是否能打开
			if (!cameraState[i]) errorCode = CameraControler::CameraInitFailed;
		}
	}
	else { //使用设定的设备号初始化
		if (*nCamera > deviceIndex.size()) //判断调用的相机个数是否过多
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < deviceIndex.size(); i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(deviceIndex[i]));
			cameraState[i] = cameraList[i].isOpened(); //判断相机是否能打开
			if (!cameraState[i]) errorCode = CameraControler::CameraInitFailed;
		}
	}
	return errorCode;
}

//将相机状态map转为字符串
QString CameraControler::cameraStatusMapToString()
{
	QString available = "";
	std::map<int, bool>::iterator iter;
	for (iter = cameraState.begin(); iter != cameraState.end(); iter++) {
		available += (iter->second) ? "1" : "0";
	}
	return available;
}

//拍摄图像
CameraControler::ErrorCode CameraControler::takePhotos()
{
	errorCode = CameraControler::NoError;

	cv::Mat frame;
	for (int i = 0; i < *nCamera; i++) {
		cameraList[i] >> frame;
		cv::Mat* pMat = new cv::Mat(frame);
		(*cvmatSamples)[*currentRow][i] = pMat;
	}
	return errorCode;
}

//设置设备号
CameraControler::ErrorCode CameraControler::resetDeviceIndex(std::vector<int> iv)
{
	//int old_num = *MaxCameraNum;
	//*MaxCameraNum = nDevice;
	//for (int i = 0; i < old_num; i++)
	//	cameraList[i].release();
	return initCameras();
}

//参数报错
void CameraControler::showMessageBox(QWidget *parent)
{
	if (errorCode == CameraControler::NoError) return;

	QString warningMessage;
	switch (errorCode)
	{
	case CameraControler::Uncheck:
		warningMessage = QString::fromLocal8Bit("相机状态未确认！"); break;
	case CameraControler::InvalidCameraNum:
		warningMessage = QString::fromLocal8Bit("当前调用的相机个数无效！"); break;
	case CameraControler::CameraInitFailed:
		warningMessage = QString::fromLocal8Bit("相机初始化失败！     \n");
		warningMessage += "CameraState: " + cameraStatusMapToString(); break;
	default:
		warningMessage = QString::fromLocal8Bit("未知错误！"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
		warningMessage + "    \n" +
		"CameraControler: ErrorCode: " + QString::number(errorCode),
		QString::fromLocal8Bit("确定"));
}
