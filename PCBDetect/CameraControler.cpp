#include "CameraControler.h"

using cv::Mat;
using pcb::CvMatVector;


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
	case Operation::TakePhotos: //拍照
	    //拍摄结束后将结果存入 (*cvmatSamples)[*currentRow] 中
		takePhotos();
		emit takePhotosFinished_camera(errorCode);
		break;
	case Operation::NoOperation:
		break;
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
	if (detectParams->nCamera <= 0)
		return errorCode = CameraControler::InvalidCameraNum;

	//初始化相机列表
	errorCode = CameraControler::NoError;
	if (deviceIndex.size() == 0) { //使用默认的设备号初始化
		if (detectParams->nCamera > adminConfig->MaxCameraNum) //判断调用的相机个数是否过多
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < detectParams->nCamera; i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(i));
			cameraState[i] = cameraList[i].isOpened(); //判断相机是否能打开
			if (!cameraState[i]) errorCode = CameraControler::InitFailed;
			else {
				cameraList[i].set(cv::CAP_PROP_FRAME_HEIGHT, adminConfig->ImageSize_H);
				cameraList[i].set(cv::CAP_PROP_FRAME_WIDTH, adminConfig->ImageSize_W);
				//cameraList[i].set(cv::CAP_PROP_BUFFERSIZE, 0);
			}
		}
	}
	else { //使用设定的设备号初始化
		if (detectParams->nCamera > deviceIndex.size()) //判断调用的相机个数是否过多
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < deviceIndex.size(); i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(deviceIndex[i]));
			cameraState[i] = cameraList[i].isOpened(); //判断相机是否能打开
			if (!cameraState[i]) errorCode = CameraControler::InitFailed;
			else {
				cameraList[i].set(cv::CAP_PROP_FRAME_HEIGHT, adminConfig->ImageSize_H);
				cameraList[i].set(cv::CAP_PROP_FRAME_WIDTH, adminConfig->ImageSize_W);
				cameraList[i].set(cv::CAP_PROP_BUFFERSIZE, 0);
			}
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

	if (true || *currentRow == 0) {
		for (int i = 0; i < detectParams->nCamera; i++) {
			Mat frame;
			cameraList[i] >> frame;
			pcb::delay(200);
		}
	}
	pcb::delay(6000);

	for (int i = 0; i < detectParams->nCamera; i++) {
		int iCamera = detectParams->nCamera - i - 1;

		Mat frame;
		cameraList[i] >> frame;
		pcb::delay(100);
		cv::Mat* pMat = new cv::Mat(frame.clone());
		pcb::delay(100);
		(*cvmatSamples)[*currentRow][iCamera] = pMat;
		pcb::delay(2000);
	}

	return errorCode;
}

//设置设备号
CameraControler::ErrorCode CameraControler::resetDeviceIndex(std::vector<int> iv)
{
	deviceIndex = iv;
	for (int i = 0; i < cameraList.size(); i++) {
		cameraList[i].release();
	}
	return initCameras();
}

//参数报错
bool CameraControler::showMessageBox(QWidget *parent)
{
	if (errorCode == CameraControler::NoError) return false;

	QString warningMessage;
	switch (errorCode)
	{
	case CameraControler::Uncheck:
		warningMessage = QString::fromLocal8Bit("相机状态未确认！"); break;
	case CameraControler::InvalidCameraNum:
		warningMessage = QString::fromLocal8Bit("当前调用的相机个数无效！"); break;
	case CameraControler::InitFailed:
		warningMessage = QString::fromLocal8Bit("相机初始化失败！     \n");
		warningMessage += "CameraState: " + cameraStatusMapToString(); break;
	default:
		warningMessage = QString::fromLocal8Bit("未知错误！"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
		warningMessage + "    \n" +
		"CameraControler: ErrorCode: " + QString::number(errorCode),
		QString::fromLocal8Bit("确定"));
	return true;
}
