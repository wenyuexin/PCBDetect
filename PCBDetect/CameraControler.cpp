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
	closeCamerasOPT(); //关闭相机并释放相机列表
}
//启动线程
void CameraControler::run()
{
	switch (operation) {
	case Operation::NoOperation: //无操作
		break;
	case Operation::InitCameras: //初始化
		this->initCamerasOPT();
		emit initCamerasFinished_camera(errorCode);
		break;
	case Operation::TakePhotos: //拍照
		this->takePhotos3();
		emit takePhotosFinished_camera(errorCode);
		break;
	}
}

////启动线程
//void CameraControler::run()
//{
//	switch (operation) {
//	case Operation::NoOperation: //无操作
//		break;
//	case Operation::InitCameras: //初始化
//		this->initCameras2();
//		emit initCamerasFinished_camera(errorCode);
//		break;
//	case Operation::TakePhotos: //拍照
//		this->takePhotos2();
//		emit takePhotosFinished_camera(errorCode);
//		break;
//	}
//}


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
		CameraSetExposureTime(cameraList2[i], userConfig->exposureTime * 1000);
		//CameraSetExposureTime(cameraList2[i], 200 * 1000);
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

////断开设备
int32_t CameraControler::GENICAM_disconnect(GENICAM_Camera *pGetCamera)
{
	int32_t isDisconnectSuccess;

	isDisconnectSuccess = pGetCamera->disConnect(pGetCamera);
	if (isDisconnectSuccess != 0)
	{
		printf("disconnect fail.\n");
		return -1;
	}

	return 0;
}

//连接设备
 int32_t CameraControler::GENICAM_connect(GENICAM_Camera *pGetCamera) {
	int32_t isConnectSuccess;
	isConnectSuccess = pGetCamera->connect(pGetCamera, accessPermissionControl);
	if (isConnectSuccess != 0) {
		printf("connect cameral failed.\n");
		return -1;
	}
	return 0;
}


//修改曝光时间
 int32_t CameraControler::modifyCamralExposureTime(GENICAM_Camera *pGetCamera)
{
	 int32_t isDoubleNodeSuccess;
	 double exposureTimeValue;
	 GENICAM_DoubleNode *pDoubleNode = NULL;
	 GENICAM_DoubleNodeInfo doubleNodeInfo = { 0 };
	 doubleNodeInfo.pCamera = pGetCamera;
	 memcpy(doubleNodeInfo.attrName, "ExposureTime", sizeof("ExposureTime"));
	 isDoubleNodeSuccess = GENICAM_createDoubleNode(&doubleNodeInfo, &pDoubleNode);
	 if (isDoubleNodeSuccess != 0) {
		 printf("GENICAM_createDoubleNode fail.\n");
		 return -1;
	 }
	 exposureTimeValue = 0.0;
	 isDoubleNodeSuccess = pDoubleNode->getValue(pDoubleNode, &exposureTimeValue);
	 if (0 != isDoubleNodeSuccess) {
		 printf("get ExposureTime fail.\n");        //注意：需要释放pDoubleNode内部对象内存
		 pDoubleNode->release(pDoubleNode);
		 return -1;
	 }
	 else {
		 printf("before change ,ExposureTime is %f\n", exposureTimeValue);
	 }
	 isDoubleNodeSuccess = pDoubleNode->setValue(pDoubleNode, (exposureTimeValue));
	 if (0 != isDoubleNodeSuccess) {
		 printf("set ExposureTime fail.\n");        //注意：需要释放pDoubleNode内部对象内存
		 pDoubleNode->release(pDoubleNode);
		 return -1;
	 }
	 isDoubleNodeSuccess = pDoubleNode->getValue(pDoubleNode, &exposureTimeValue);
	 if (0 != isDoubleNodeSuccess) {
		 printf("get ExposureTime fail.\n");        //注意：需要释放pDoubleNode内部对象内存  
		 pDoubleNode->release(pDoubleNode);
		 return -1;
	 }
	 else {
		 printf("after change ,ExposureTime is %f\n", exposureTimeValue);
		 //注意：需要释放pDoubleNode内部对象内存        
		 pDoubleNode->release(pDoubleNode);
	 }   
	 return 0;
}
//设置触发模式
 int32_t CameraControler::setSoftTriggerConf(GENICAM_AcquisitionControl *pAcquisitionCtrl)
{
	int32_t nRet;
	GENICAM_EnumNode enumNode = { 0 };

	// 设置触发源为软触发
	enumNode = pAcquisitionCtrl->triggerSource(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "Software");
	if (nRet != 0)
	{
		printf("set trigger source failed.\n");
		//注意：需要释放enumNode内部对象内存
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	enumNode.release(&enumNode);

	// 设置触发器
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerSelector(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "FrameStart");
	if (nRet != 0)
	{
		printf("set trigger selector failed.\n");
		//注意：需要释放enumNode内部对象内存
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	enumNode.release(&enumNode);

	// 设置触发模式
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerMode(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "On");
	if (nRet != 0)
	{
		printf("set trigger mode failed.\n");
		//注意：需要释放enumNode内部对象内存
		enumNode.release(&enumNode);
		return -1;
	}
	//注意：需要释放enumNode内部对象内存
	enumNode.release(&enumNode);

	return 0;
}

//设置像素宽度
int32_t CameraControler::modifyCameraWidth(GENICAM_Camera *pGetCamera)
{
	int32_t isIntNodeSuccess;
	int64_t widthValue;
	GENICAM_IntNode *pIntNode = NULL;
	GENICAM_IntNodeInfo intNodeInfo = { 0 };

	intNodeInfo.pCamera = pGetCamera;
	memcpy(intNodeInfo.attrName, "Width", sizeof("Width"));

	isIntNodeSuccess = GENICAM_createIntNode(&intNodeInfo, &pIntNode);
	if (0 != isIntNodeSuccess)
	{
		printf("GENICAM_createIntNode fail.\n");
		return -1;
	}

	widthValue = 0;
	isIntNodeSuccess = pIntNode->getValue(pIntNode, &widthValue);
	if (0 != isIntNodeSuccess != 0)
	{
		printf("get Width fail.\n");
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("before change ,Width is %d\n", widthValue);
	}

	widthValue = adminConfig->ImageSize_W;
	isIntNodeSuccess = pIntNode->setValue(pIntNode, (widthValue ));
	if (0 != isIntNodeSuccess)
	{
		printf("set Width fail.\n");
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
		return -1;
	}

	isIntNodeSuccess = pIntNode->getValue(pIntNode, &widthValue);
	if (0 != isIntNodeSuccess)
	{
		printf("get Width fail.\n");
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("after change ,Width is %d\n", widthValue);
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
	}

	return 0;
}
//设置像素高度
int32_t CameraControler::modifyCameraHeight(GENICAM_Camera *pGetCamera)
{
	int32_t isIntNodeSuccess;
	int64_t heightValue;
	CameraControler *cameraControler;
	GENICAM_IntNode *pIntNode = NULL;
	GENICAM_IntNodeInfo intNodeInfo = { 0 };

	intNodeInfo.pCamera = pGetCamera;
	memcpy(intNodeInfo.attrName, "Height", sizeof("Height"));

	isIntNodeSuccess = GENICAM_createIntNode(&intNodeInfo, &pIntNode);
	if (0 != isIntNodeSuccess)
	{
		printf("GENICAM_createIntNode fail.\n");
		return -1;
	}

	heightValue = 0;
	isIntNodeSuccess = pIntNode->getValue(pIntNode, &heightValue);
	if (0 != isIntNodeSuccess != 0)
	{
		printf("get Height fail.\n");
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("before change ,Height is %d\n", heightValue);
	}

	heightValue = adminConfig->ImageSize_H;
	isIntNodeSuccess = pIntNode->setValue(pIntNode, (heightValue ));
	/*isIntNodeSuccess = pIntNode->setValue(pIntNode, (widthValue - 8));*/
	if (0 != isIntNodeSuccess)
	{
		printf("set Height fail.\n");
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
		return -1;
	}

	isIntNodeSuccess = pIntNode->getValue(pIntNode, &heightValue);
	if (0 != isIntNodeSuccess)
	{
		printf("get Height fail.\n");
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("after change ,Height is %d\n", heightValue);
		//注意：需要释放pIntNode内部对象内存
		pIntNode->release(pIntNode);
	}

	return 0;
}

 int32_t CameraControler::modifyCameraReverseX(GENICAM_Camera *pGetCamera)
{
	int32_t isBoolNodeSuccess;
	uint32_t reverseXValue;
	GENICAM_BoolNode *pBoolNode = NULL;
	GENICAM_BoolNodeInfo boolNodeInfo = { 0 };

	boolNodeInfo.pCamera = pGetCamera;
	memcpy(boolNodeInfo.attrName, "ReverseX", sizeof("ReverseX"));

	isBoolNodeSuccess = GENICAM_createBoolNode(&boolNodeInfo, &pBoolNode);
	if (0 != isBoolNodeSuccess)
	{
		printf("GENICAM_createBoolNode fail.\n");
		return -1;
	}

	reverseXValue = 0;
	isBoolNodeSuccess = pBoolNode->getValue(pBoolNode, &reverseXValue);
	if (0 != isBoolNodeSuccess)
	{
		printf("get ReverseX fail.\n");
		//注意：需要释放pBoolNode内部对象内存
		pBoolNode->release(pBoolNode);
		return -1;
	}
	else
	{
		printf("before change ,ReverseX is %u\n", reverseXValue);
	}

	isBoolNodeSuccess = pBoolNode->setValue(pBoolNode, 1);
	if (0 != isBoolNodeSuccess)
	{
		printf("set ReverseX fail.\n");
		//注意：需要释放pBoolNode内部对象内存
		pBoolNode->release(pBoolNode);
		return -1;
	}

	isBoolNodeSuccess = pBoolNode->getValue(pBoolNode, &reverseXValue);
	if (0 != isBoolNodeSuccess)
	{
		printf("get ReverseX fail.\n");
		//注意：需要释放pBoolNode内部对象内存
		pBoolNode->release(pBoolNode);
		return -1;
	}
	else
	{
		printf("after change ,ReverseX is %u\n", reverseXValue);
		//注意：需要释放pBoolNode内部对象内存
		pBoolNode->release(pBoolNode);
	}

	return 0;
}

int32_t CameraControler::GENICAM_CreateStreamSource(GENICAM_Camera *pGetCamera, GENICAM_StreamSource **ppStreamSource)
{
	int32_t isCreateStreamSource;
	GENICAM_StreamSourceInfo stStreamSourceInfo;


	stStreamSourceInfo.channelId = 0;
	stStreamSourceInfo.pCamera = pGetCamera;

	isCreateStreamSource = GENICAM_createStreamSource(&stStreamSourceInfo, ppStreamSource);

	if (isCreateStreamSource != 0)
	{
		printf("create stream obj  fail.\r\n");
		return -1;
	}

	return 0;
}

static void onGetFrame(GENICAM_Frame* pFrame)
{
	int32_t ret = -1;
	uint64_t blockId = 0;
	/*CameraControler *cameraControler;*/

	// 标准输出换行
	printf("\r\n");

	ret = pFrame->valid(pFrame);
	if (0 == ret)
	{
		blockId = pFrame->getBlockId(pFrame);
		printf("blockId = %d.\r\n", blockId);
	/*	CameraControler::isGrabbingFlag  = 0;*/

		IMGCNV_SOpenParam openParam;
		openParam.width = pFrame->getImageWidth(pFrame);
		openParam.height = pFrame->getImageHeight(pFrame);
		openParam.paddingX = pFrame->getImagePaddingX(pFrame);
		openParam.paddingY = pFrame->getImagePaddingY(pFrame);
		openParam.dataSize = pFrame->getImageSize(pFrame);
		openParam.pixelForamt = pFrame->getImagePixelFormat(pFrame);

		if (openParam.pixelForamt == gvspPixelMono8) {
			cv::Mat image = cv::Mat(pFrame->getImageHeight(pFrame), pFrame->getImageWidth(pFrame), CV_8U, (uint8_t*)((pFrame->getImage(pFrame))));
			cv::Mat* pMat = new cv::Mat(image.clone());
			CameraControler::pImageFrame = pMat;
			cv::imwrite("D:\\0000000_project\\opt_image\\image1008-1623.jpg", image);
			
		}
		else {
			printf("openParam.pixelForamt!gvspPixelMono8");
		}
	}
	else
	{
		printf("Frame is invalid!\n");
	}
	//注意：不管帧是否有效，都需要释放pFrame内部对象内存
	ret = pFrame->release(pFrame);

	return;
}
//开始抓流
int32_t CameraControler::GENICAM_startGrabbing(GENICAM_StreamSource *pStreamSource)
{
	int32_t isStartGrabbingSuccess;
	GENICAM_EGrabStrategy eGrabStrategy;

	eGrabStrategy = grabStrartegyLatestImage;
	isStartGrabbingSuccess = pStreamSource->startGrabbing(pStreamSource, 0, eGrabStrategy);

	if (isStartGrabbingSuccess != 0)
	{
		printf("StartGrabbing  fail.\n");
		return -1;
	}

	return 0;
}
//停止抓流
 int32_t CameraControler::GENICAM_stopGrabbing(GENICAM_StreamSource *pStreamSource)
{
	int32_t isStopGrabbingSuccess;

	isStopGrabbingSuccess = pStreamSource->stopGrabbing(pStreamSource);
	if (isStopGrabbingSuccess != 0)
	{
		printf("StopGrabbing  fail.\n");
		return -1;
	}

	return 0;
}
int32_t CameraControler::executeTriggerSoftware(GENICAM_AcquisitionControl *pAcquisitionCtrl)
{
	int32_t isTriggerSoftwareSuccess;

	GENICAM_CmdNode cmdNode = pAcquisitionCtrl->triggerSoftware(pAcquisitionCtrl);
	int count = 10;
	while (count>0) {
	isTriggerSoftwareSuccess = cmdNode.execute(&cmdNode);
		if (isTriggerSoftwareSuccess == 0) {
			break;
		}
		count--;

	}

	//sleep(50);
	/*isTriggerSoftwareSuccess = cmdNode.execute(&cmdNode);*/
	if (isTriggerSoftwareSuccess != 0)
	{
		printf("Execute triggerSoftware fail.\n");

		//注意：需要释放cmdNode内部对象内存
		cmdNode.release(&cmdNode);
		return -1;
	}

	//注意：需要释放cmdNode内部对象内存
	cmdNode.release(&cmdNode);

	return 0;
}

//相机初始化-OPT
bool CameraControler::initCamerasOPT()
{
	errorCode = CameraControler::NoError;

	//关闭已经打开的相机并释放相机列表
	//关闭失败，最初相机对象为空
	this->closeCamerasOPT();
	
	//生成系统单例
    status = GENICAM_getSystemInstance(&pSystem);
	if (-1==status) 
	{
		errorCode = CameraControler::InitFailed;
		return false;
	}
	//发现相机
	status = pSystem->discovery(pSystem, &pCameraList, &cameraCnt, typeAll);
	if (-1 == status)
	{
		errorCode = CameraControler::InitFailed;
		return false;
	}
	if (cameraCnt < 1)
	{
		printf("there is no device.\r\n");
		return false;
	}
	////系统中的设备数量少于设定值
	//if (cameraCnt < adminConfig->MaxCameraNum) {
	//	errorCode = CameraControler::InitFailed; 
	//	return false;
	//}
	//设备连接
	for (int i = 0; i <1/* adminConfig->MaxCameraNum*/; i++) {
		pCamera = &pCameraList[i];
		/*pCameraList2[i] = pCamera;*/
		status = GENICAM_connect(pCamera);
		cameraState[i] = (status == 0);
		if (status != 0)
		{
			errorCode = CameraControler::InitFailed;
			return false;
		}

		//创建属性节点 AcquisitionControl
		acquisitionControlInfo.pCamera = pCamera;
		status = GENICAM_createAcquisitionControl(&acquisitionControlInfo, &pAcquisitionCtrl);
		if (status != 0)
		{
			errorCode = CameraControler::InitFailed;
			return false;
		}
        //设置相机参数

		//如果相机没有打开，则直接跳过
	/*	if (!cameraState[i]) continue;*/
		//设置曝光时间，单位us
		modifyCamralExposureTime(pCamera);
		// 修改相机像素宽度,和高度，通用int型属性访问实例
		modifyCameraWidth(pCamera);
		modifyCameraHeight(pCamera);

		//设置触发模式
		status = setSoftTriggerConf(pAcquisitionCtrl);
		if (status != 0)
		{
			errorCode = CameraControler::InitFailed;
			//注意：需要释放pAcquisitionCtrl内部对象内存
			pAcquisitionCtrl->release(pAcquisitionCtrl);
			return false;
		}

		//创建流对象
		status = GENICAM_CreateStreamSource(pCamera, &pStreamSource);
		if ((status != 0) || (NULL == pStreamSource))
		{
			printf("create stream obj  fail.\r\n");
			//注意：需要释放pAcquisitionCtrl内部对象内存
			pAcquisitionCtrl->release(pAcquisitionCtrl);
			return false;
		}
		//注册回调函数（获取图像）
		status = pStreamSource->attachGrabbing(pStreamSource, onGetFrame);
		if (status != 0)
		{
			printf("attch Grabbing fail!\n");
			//注意：需要释放pAcquisitionCtrl内部对象内存
			pAcquisitionCtrl->release(pAcquisitionCtrl);

			//注意：需要释放pStreamSource内部对象内存
			pStreamSource->release(pStreamSource);
			return false;
		}
		status = GENICAM_startGrabbing(pStreamSource);

		if (status != 0)
		{
			printf("StartGrabbing  fail.\n");
			/*isGrabbingFlag = 0;*/
			//注意：需要释放pAcquisitionCtrl内部对象内存
			pAcquisitionCtrl->release(pAcquisitionCtrl);

			//注意：需要释放pStreamSource内部对象内存
			pStreamSource->release(pStreamSource);
			return false;
		}
		else
		{
			/*isGrabbingFlag = 1;*/
		}
		////执行一次软触发
		//status = executeTriggerSoftware(pAcquisitionCtrl);
		//if (status != 0)
		//{
		//	printf("TriggerSoftware fail.\n");
		//	//注意：需要释放pAcquisitionCtrl内部对象内存
		//	pAcquisitionCtrl->release(pAcquisitionCtrl);

		//	//注意：需要释放pStreamSource内部对象内存
		//	pStreamSource->release(pStreamSource);
		//	return false;
		//}

	}
		////修改相机ReverseX，通用bool型属性访问实例
		//modifyCameraReverseX(pCamera);
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
	if (cameraState.size() <1 /*runtimeParams->nCamera*/) return false;

	for (int i = 0; i < 1/*runtimeParams->nCamera*/; i++) {
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

//关闭已经打开的相机设备
void CameraControler::closeCamerasOPT()
{

	//关闭相机设备 - OPT pCameraList[i]
	for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
		status = GENICAM_disconnect(&pCameraList[i]);
		if (status != 0)
		{
			printf("disconnect camera failed.\n");
			getchar();
			return;
		}
	}
	pCameraList = NULL;; //清空列表

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

//拍摄图像 - OPT
void CameraControler::takePhotos3()
{
	clock_t t1 = clock();
	//int colorMode = userConfig->colorMode; //色彩模式
	//int dataType = (colorMode == 1) ? CV_8UC1 : CV_8UC3; //Mat的数据类型

	for (int i = 0; i < runtimeParams->nCamera; i++) {
		//执行一次软触发
	/*	isGrabbingFlag = 1;*/
		status = executeTriggerSoftware(pAcquisitionCtrl);
		if (status != 0)
		{
			printf("TriggerSoftware fail.\n");
			//注意：需要释放pAcquisitionCtrl内部对象内存
			pAcquisitionCtrl->release(pAcquisitionCtrl);

			//注意：需要释放pStreamSource内部对象内存
			pStreamSource->release(pStreamSource);
			return;
		}
		Sleep(2000);

		//具有bug,触发没进入回调函数，就会陷入死循环
		//while (isGrabbingFlag)
		//{
		//	Sleep(50);
		//}

		////注意：需要释放pAcquisitionCtrl内部对象内存
		//pAcquisitionCtrl->release(pAcquisitionCtrl);

		(*cvmatSamples)[*currentRow][i] = pImageFrame;

		////注销回调函数
	/*	status = pStreamSource->detachGrabbing(pStreamSource, onGetFrame);
		if (status != 0)*/
		//{
		//	printf("detachGrabbing  fail.\n");
		//}

		//// stop grabbing from camera
		////停止抓流
		//status = GENICAM_stopGrabbing(pStreamSource);
		//if (status != 0)
		//{
		//	printf("Stop Grabbing  fail.\n");
		//}

		////注意：需要释放pStreamSource内部对象内存
		//pStreamSource->release(pStreamSource);



		//将图像转换为Mat类
		//if (colorMode == 1) {
		//	//相机格式是Mono8时
		//	cv::Mat image = cv::Mat(pFrame->getImageHeight(pFrame),
		//		pFrame->getImageWidth(pFrame),
		//		CV_8U,
		//		(uint8_t*)((pFrame->getImage(pFrame)));
		//}
		//else {
		//	//相机图像格式为彩色格式
		//	cv::Mat image = cv::Mat(pFrame->getImageHeight(pFrame),
		//		pFrame->getImageWidth(pFrame),
		//		CV_8UC3,
		//		(uint8_t*)pBGRbuffer);
		//}



		// disconnect camera
		//断开设备
		
	/*	return;*/
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

//int CameraControler::isGrabbingFlag = 0;
cv::Mat* CameraControler::pImageFrame;