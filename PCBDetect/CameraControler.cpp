#include "CameraControler.h"

using cv::Mat;
using cv::Size;
using pcb::CvMatVector;

#ifdef _WIN64
#pragma comment(lib, ".\\MVCAMSDK_X64.lib")
#else
#pragma comment(lib, ".\\MVCAMSDK.lib")
#endif

CameraControler::CameraControler(QThread *parent)
	: QThread(parent)
{
	caller = -1; //模块的调用者
	errorCode = Unchecked; //控制器的错误码
	operation = NoOperation;//操作指令
}

CameraControler::~CameraControler()
{
	qDebug() << "~CameraControler";
	closeCameras(); //关闭相机并释放相机列表
}

//启动线程
void CameraControler::run()
{
	switch (operation) {
	case Operation::NoOperation: //无操作
		break;
	case Operation::InitCameras: //初始化
		this->initCameras2();
		emit initCamerasFinished_camera(errorCode);
		break;
	case Operation::TakePhotos: //拍照
		this->takePhotos2();
		emit takePhotosFinished_camera(errorCode);
		break;
	}
}


/**************** 相机初始化与关闭 ******************/

//相机初始化 - OpenCV
//年久失修，谨慎使用
//相机排列顺序与设备顺序不一致时，需要使用第二个参数
//输入相机实际排列顺序对应的设备编号进行初始化,系统设备编号从0开始
CameraControler::ErrorCode CameraControler::initCameras()
{
	if (runtimeParams->nCamera <= 0)
		return errorCode = CameraControler::InvalidCameraNum;

	//初始化相机列表
	errorCode = CameraControler::NoError;
	if (deviceIndex.size() == 0) { //使用默认的设备号初始化
		if (runtimeParams->nCamera > adminConfig->MaxCameraNum) //判断调用的相机个数是否过多
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < runtimeParams->nCamera; i++) { //添加相机
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
		if (runtimeParams->nCamera > deviceIndex.size()) //判断调用的相机个数是否过多
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < deviceIndex.size(); i++) { //添加相机
			cameraList.push_back(cv::VideoCapture(deviceIndex[i]));
			cameraState[i] = cameraList[i].isOpened(); //判断相机是否能打开
			if (!cameraState[i]) errorCode = CameraControler::InitFailed;
			else {
				cameraList[i].set(cv::CAP_PROP_FRAME_HEIGHT, adminConfig->ImageSize_H);
				cameraList[i].set(cv::CAP_PROP_FRAME_WIDTH, adminConfig->ImageSize_W);
				//cameraList[i].set(cv::CAP_PROP_BUFFERSIZE, 0);
			}
		}
	}
	return errorCode;
}

//相机初始化 - 迈德威视
bool CameraControler::initCameras2()
{
	errorCode = CameraControler::NoError;

	//若相机已经初始化，则直接跳过后续步骤
	//if (isCamerasInitialized()) return true;

	//关闭已经打开的相机并释放相机列表
	this->closeCameras();

	//相机sdk初始化
	if (CameraSdkInit(1) != CAMERA_STATUS_SUCCESS) { 
		errorCode = CameraControler::InitFailed;
		return false;
	}

	//判断是否存在：枚举设备失败或者系统中的设备数量少于设定值
	if (CameraEnumerateDevice(sCameraList, &CameraNums) != CAMERA_STATUS_SUCCESS ||
		CameraNums < adminConfig->MaxCameraNum)
	{
		errorCode = CameraControler::InitFailed; return false;
	}

	//获取相机列表
	CameraSdkStatus status;
	if (deviceIndex.empty() || deviceIndex.size() < adminConfig->MaxCameraNum) {
		for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
			cameraList2.push_back(-1);
			status = CameraInit(&sCameraList[i], -1, -1, &cameraList2[i]);
			cameraState[i] = (status == CAMERA_STATUS_SUCCESS);
		}
	}
	else {
		for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
			cameraList2.push_back(-1);
			status = CameraInit(&sCameraList[deviceIndex[i]], -1, -1, &cameraList2[i]);
			cameraState[i] = (status == CAMERA_STATUS_SUCCESS);
		}
	}

	//设置相机参数
	sImageSize.iIndex = 255;
	sImageSize.uBinSumMode = 0;
	sImageSize.uBinAverageMode = 0;
	sImageSize.uSkipMode = 0;
	sImageSize.uResampleMask = 0;
	sImageSize.iHOffsetFOV = 0;
	sImageSize.iVOffsetFOV = 0;
	sImageSize.iWidthFOV = adminConfig->ImageSize_W;
	sImageSize.iHeightFOV = adminConfig->ImageSize_H;
	sImageSize.iWidth = adminConfig->ImageSize_W;
	sImageSize.iHeight = adminConfig->ImageSize_H;
	sImageSize.iWidthZoomHd = 0;
	sImageSize.iHeightZoomHd = 0;
	sImageSize.iWidthZoomSw = 0;
	sImageSize.iHeightZoomSw = 0;

	for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
		//如果相机没有打开，则直接跳过
		if (!cameraState[i]) continue;
		//设置曝光时间，单位us
		//CameraSetExposureTime(cameraList2[i], userConfig->exposureTime * 1000);
		CameraSetExposureTime(cameraList2[i], 200 * 1000);
		//设置色彩模式 - 0彩色 1黑白 -1默认
		if (userConfig->colorMode == 1) 
			CameraSetIspOutFormat(cameraList2[i], CAMERA_MEDIA_TYPE_MONO8);
		
		//设置相机的触发模式。0表示连续采集模式；1表示软件触发模式；2表示硬件触发模式。
		CameraSetTriggerMode(cameraList2[i], 1);
		CameraSetTriggerCount(cameraList2[i], 1);
		if (CameraSetResolutionForSnap(cameraList2[i], &sImageSize) != CAMERA_STATUS_SUCCESS) {
			cameraState[i] = false; continue;
		}
		CameraPlay(cameraList2[i]);
	}

	//检查相机是否已经成功初始化
	if (!isCamerasInitialized()) {
		errorCode = CameraControler::InitFailed;
		return false;
	}
	return true;
}

//将相机状态map转为字符串
QString CameraControler::cameraStatusMapToString()
{
	QString available = "";
	if (cameraState.size() == 0) return available;

	int nC = min(runtimeParams->nCamera, cameraState.size());
	for (int i = 0; i < nC; i++) {
		int iCamera = adminConfig->MaxCameraNum - i - 1;
		available += (cameraState[iCamera]) ? "1" : "0";
	}
	return available + " (" + QString::number(runtimeParams->nCamera) + ")";
}

//判断相机是否已经初始化
bool CameraControler::isCamerasInitialized()
{
	if (cameraState.size() < runtimeParams->nCamera) return false;

	for (int i = 0; i < runtimeParams->nCamera; i++) {
		int iCamera = adminConfig->MaxCameraNum - i - 1;
		if (!cameraState[iCamera]) return false;
	}
	return true;
}

//关闭已经打开的相机设备
void CameraControler::closeCameras()
{
	//关闭相机设备 - OpenCV
	for (int i = 0; i < cameraList.size(); i++)
		cameraList[i].release();
	cameraList.clear(); //清空列表

	//关闭相机设备 - 迈德威视
	for (int i = 0; i < cameraList2.size(); i++)
		CameraUnInit(cameraList2[i]);
	cameraList2.clear(); //清空列表

	//清空相机状态
	cameraState.clear();
}


/******************* 相机拍照 ********************/

//拍摄图像 - OpenCV
//年久失修，谨慎使用
CameraControler::ErrorCode CameraControler::takePhotos()
{
	errorCode = CameraControler::NoError;

	if (true || *currentRow == 0) {
		for (int i = 0; i < runtimeParams->nCamera; i++) {
			Mat frame;
			cameraList[i] >> frame;
			pcb::delay(200);
		}
	}
	pcb::delay(8000);

	for (int i = 0; i < runtimeParams->nCamera; i++) {
		int iCamera = runtimeParams->nCamera - i - 1;
		Mat frame;
		cameraList[i] >> frame;
		pcb::delay(5000);
		cv::Mat* pMat = new cv::Mat(frame.clone());
		pcb::delay(200);
		(*cvmatSamples)[*currentRow][iCamera] = pMat;
		pcb::delay(200);

		//cv::imwrite((QString::number(*currentRow)+"_"+ QString::number(i) + ".jpg").toStdString(), *pMat);
	}
	pcb::delay(5000);
	return errorCode;
}

//拍摄图像 - 迈德威视
void CameraControler::takePhotos2()
{
	clock_t t1 = clock();
	Size frameSize(adminConfig->ImageSize_W, adminConfig->ImageSize_H);
	int colorMode = userConfig->colorMode; //色彩模式
	int dataType = (colorMode == 1) ? CV_8UC1 : CV_8UC3; //Mat的数据类型

	for (int i = 0; i < runtimeParams->nCamera; i++) {
		int iCamera = adminConfig->MaxCameraNum - i - 1;

		BYTE *pRawBuffer;
		BYTE *pRgbBuffer;
		tSdkFrameHead FrameInfo;
		tSdkImageResolution sImageSize;
		CameraSdkStatus status;
		//CString msg;
		//memset(&sImageSize, 0, sizeof(tSdkImageResolution));
		//sImageSize.iIndex = 0xff;
		//CameraGetInformation(camList[i], &FrameInfo)
		//CameraSoftTrigger(camList[i]);//执行一次软触发。执行后，会触发由CameraSetTriggerCount指定的帧数。
		//CameraGetImageBuffer(camList[i], &FrameInfo, &pRawBuffer, 10000);//抓一张图
		
		double t0 = clock();
		int counter = 5;
		while (counter > 0) {
			CameraClearBuffer(cameraList2[iCamera]);
			CameraSoftTrigger(cameraList2[iCamera]);
			int flag = CameraGetImageBuffer(cameraList2[iCamera], &FrameInfo, &pRawBuffer, 10000);
			if (CAMERA_STATUS_SUCCESS == flag) break; //抓一张图
			counter--;
		}
		//CameraSnapToBuffer(camList[i], &FrameInfo, &pRawBuffer, 10000);//抓一整图

		//申请一个buffer，用来将获得的原始数据转换为RGB数据，并同时获得图像处理效果
		counter = 10;
		int bufferSize = FrameInfo.iWidth * FrameInfo.iHeight * (colorMode==1 ? 1 : 3);
		while (counter > 0) {
			pRgbBuffer = (unsigned char *) CameraAlignMalloc(bufferSize, 16);
			if (pRgbBuffer != NULL) break;
			counter--;
		}

		//处理图像，并得到RGB格式的数据
		CameraImageProcess(cameraList2[iCamera], pRawBuffer, pRgbBuffer, &FrameInfo);
		//释放由CameraSnapToBuffer、CameraGetImageBuffer获得的图像缓冲区
		while (CameraReleaseImageBuffer(cameraList2[iCamera], pRawBuffer) != CAMERA_STATUS_SUCCESS);
		//将pRgbBuffer转换为Mat类
		cv::Mat fram(frameSize, dataType, pRgbBuffer);

		cv::flip(fram, fram, -1); //直接获取的图像时反的，这里旋转180度
		cv::flip(fram, fram, 1);
		cv::Mat* pMat = new cv::Mat(fram.clone());
		(*cvmatSamples)[*currentRow][i] = pMat;
		CameraAlignFree(pRgbBuffer);
	} 

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("相机拍图：") << (t2 - t1) << "ms" 
		<< "( currentRow_show =" << *currentRow << ")" << endl;
	return;
}


/********************* 其他 ********************/

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
	case CameraControler::Unchecked:
		warningMessage = QString::fromLocal8Bit("相机状态未确认！"); break;
	case CameraControler::InvalidCameraNum:
		warningMessage = QString::fromLocal8Bit("当前调用的相机个数无效！"); break;
	case CameraControler::InitFailed:
		warningMessage = QString::fromLocal8Bit("相机初始化失败！     \n");
		warningMessage += "CameraState: " + cameraStatusMapToString(); break;
	case CameraControler::TakePhotosFailed:
		warningMessage = QString::fromLocal8Bit("拍照失败！"); break;
	default:
		warningMessage = QString::fromLocal8Bit("未知错误！"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
		warningMessage + "    \n" +
		"CameraControler: ErrorCode: " + QString::number(errorCode),
		QString::fromLocal8Bit("确定"));
	return true;
}
