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
	caller = -1; //ģ��ĵ�����
	errorCode = Unchecked; //�������Ĵ�����
	operation = NoOperation;//����ָ��
}

CameraControler::~CameraControler()
{
	qDebug() << "~CameraControler";
	closeCamerasOPT(); //�ر�������ͷ�����б�
}
//�����߳�
void CameraControler::run()
{
	switch (operation) {
	case Operation::NoOperation: //�޲���
		break;
	case Operation::InitCameras: //��ʼ��
		this->initCamerasOPT();
		emit initCamerasFinished_camera(errorCode);
		break;
	case Operation::TakePhotos: //����
		this->takePhotos3();
		emit takePhotosFinished_camera(errorCode);
		break;
	}
}

////�����߳�
//void CameraControler::run()
//{
//	switch (operation) {
//	case Operation::NoOperation: //�޲���
//		break;
//	case Operation::InitCameras: //��ʼ��
//		this->initCameras2();
//		emit initCamerasFinished_camera(errorCode);
//		break;
//	case Operation::TakePhotos: //����
//		this->takePhotos2();
//		emit takePhotosFinished_camera(errorCode);
//		break;
//	}
//}


/**************** �����ʼ����ر� ******************/

//�����ʼ�� - OpenCV
//���ʧ�ޣ�����ʹ��
//�������˳�����豸˳��һ��ʱ����Ҫʹ�õڶ�������
//�������ʵ������˳���Ӧ���豸��Ž��г�ʼ��,ϵͳ�豸��Ŵ�0��ʼ
CameraControler::ErrorCode CameraControler::initCameras()
{
	if (runtimeParams->nCamera <= 0)
		return errorCode = CameraControler::InvalidCameraNum;

	//��ʼ������б�
	errorCode = CameraControler::NoError;
	if (deviceIndex.size() == 0) { //ʹ��Ĭ�ϵ��豸�ų�ʼ��
		if (runtimeParams->nCamera > adminConfig->MaxCameraNum) //�жϵ��õ���������Ƿ����
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < runtimeParams->nCamera; i++) { //������
			cameraList.push_back(cv::VideoCapture(i));
			cameraState[i] = cameraList[i].isOpened(); //�ж�����Ƿ��ܴ�
			if (!cameraState[i]) errorCode = CameraControler::InitFailed;
			else {
				cameraList[i].set(cv::CAP_PROP_FRAME_HEIGHT, adminConfig->ImageSize_H);
				cameraList[i].set(cv::CAP_PROP_FRAME_WIDTH, adminConfig->ImageSize_W);
				//cameraList[i].set(cv::CAP_PROP_BUFFERSIZE, 0);
			}
		}
	}
	else { //ʹ���趨���豸�ų�ʼ��
		if (runtimeParams->nCamera > deviceIndex.size()) //�жϵ��õ���������Ƿ����
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < deviceIndex.size(); i++) { //������
			cameraList.push_back(cv::VideoCapture(deviceIndex[i]));
			cameraState[i] = cameraList[i].isOpened(); //�ж�����Ƿ��ܴ�
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


//�����ʼ�� - ��������
bool CameraControler::initCameras2()
{

	errorCode = CameraControler::NoError;

	//������Ѿ���ʼ������ֱ��������������
	//if (isCamerasInitialized()) return true;

	//�ر��Ѿ��򿪵�������ͷ�����б�
	this->closeCameras();

	//���sdk��ʼ��
	if (CameraSdkInit(1) != CAMERA_STATUS_SUCCESS) { 
		errorCode = CameraControler::InitFailed;
		return false;
	}

	//�ж��Ƿ���ڣ�ö���豸ʧ�ܻ���ϵͳ�е��豸���������趨ֵ
	if (CameraEnumerateDevice(sCameraList, &CameraNums) != CAMERA_STATUS_SUCCESS ||
		CameraNums < adminConfig->MaxCameraNum)
	{
		errorCode = CameraControler::InitFailed; return false;
	}

	//��ȡ����б�
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

	//�����������
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
		//������û�д򿪣���ֱ������
		if (!cameraState[i]) continue;
		//�����ع�ʱ�䣬��λus
		CameraSetExposureTime(cameraList2[i], userConfig->exposureTime * 1000);
		//CameraSetExposureTime(cameraList2[i], 200 * 1000);
		//����ɫ��ģʽ - 0��ɫ 1�ڰ� -1Ĭ��
		if (userConfig->colorMode == 1) 
			CameraSetIspOutFormat(cameraList2[i], CAMERA_MEDIA_TYPE_MONO8);
		
		//��������Ĵ���ģʽ��0��ʾ�����ɼ�ģʽ��1��ʾ�������ģʽ��2��ʾӲ������ģʽ��
		CameraSetTriggerMode(cameraList2[i], 1);
		CameraSetTriggerCount(cameraList2[i], 1);
		if (CameraSetResolutionForSnap(cameraList2[i], &sImageSize) != CAMERA_STATUS_SUCCESS) {
			cameraState[i] = false; continue;
		}
		CameraPlay(cameraList2[i]);
	}

	//�������Ƿ��Ѿ��ɹ���ʼ��
	if (!isCamerasInitialized()) {
		errorCode = CameraControler::InitFailed;
		return false;
	}
	return true;
}

////�Ͽ��豸
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

//�����豸
 int32_t CameraControler::GENICAM_connect(GENICAM_Camera *pGetCamera) {
	int32_t isConnectSuccess;
	isConnectSuccess = pGetCamera->connect(pGetCamera, accessPermissionControl);
	if (isConnectSuccess != 0) {
		printf("connect cameral failed.\n");
		return -1;
	}
	return 0;
}


//�޸��ع�ʱ��
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
		 printf("get ExposureTime fail.\n");        //ע�⣺��Ҫ�ͷ�pDoubleNode�ڲ������ڴ�
		 pDoubleNode->release(pDoubleNode);
		 return -1;
	 }
	 else {
		 printf("before change ,ExposureTime is %f\n", exposureTimeValue);
	 }
	 isDoubleNodeSuccess = pDoubleNode->setValue(pDoubleNode, (exposureTimeValue));
	 if (0 != isDoubleNodeSuccess) {
		 printf("set ExposureTime fail.\n");        //ע�⣺��Ҫ�ͷ�pDoubleNode�ڲ������ڴ�
		 pDoubleNode->release(pDoubleNode);
		 return -1;
	 }
	 isDoubleNodeSuccess = pDoubleNode->getValue(pDoubleNode, &exposureTimeValue);
	 if (0 != isDoubleNodeSuccess) {
		 printf("get ExposureTime fail.\n");        //ע�⣺��Ҫ�ͷ�pDoubleNode�ڲ������ڴ�  
		 pDoubleNode->release(pDoubleNode);
		 return -1;
	 }
	 else {
		 printf("after change ,ExposureTime is %f\n", exposureTimeValue);
		 //ע�⣺��Ҫ�ͷ�pDoubleNode�ڲ������ڴ�        
		 pDoubleNode->release(pDoubleNode);
	 }   
	 return 0;
}
//���ô���ģʽ
 int32_t CameraControler::setSoftTriggerConf(GENICAM_AcquisitionControl *pAcquisitionCtrl)
{
	int32_t nRet;
	GENICAM_EnumNode enumNode = { 0 };

	// ���ô���ԴΪ����
	enumNode = pAcquisitionCtrl->triggerSource(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "Software");
	if (nRet != 0)
	{
		printf("set trigger source failed.\n");
		//ע�⣺��Ҫ�ͷ�enumNode�ڲ������ڴ�
		enumNode.release(&enumNode);
		return -1;
	}
	//ע�⣺��Ҫ�ͷ�enumNode�ڲ������ڴ�
	enumNode.release(&enumNode);

	// ���ô�����
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerSelector(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "FrameStart");
	if (nRet != 0)
	{
		printf("set trigger selector failed.\n");
		//ע�⣺��Ҫ�ͷ�enumNode�ڲ������ڴ�
		enumNode.release(&enumNode);
		return -1;
	}
	//ע�⣺��Ҫ�ͷ�enumNode�ڲ������ڴ�
	enumNode.release(&enumNode);

	// ���ô���ģʽ
	memset(&enumNode, 0, sizeof(enumNode));
	enumNode = pAcquisitionCtrl->triggerMode(pAcquisitionCtrl);
	nRet = enumNode.setValueBySymbol(&enumNode, "On");
	if (nRet != 0)
	{
		printf("set trigger mode failed.\n");
		//ע�⣺��Ҫ�ͷ�enumNode�ڲ������ڴ�
		enumNode.release(&enumNode);
		return -1;
	}
	//ע�⣺��Ҫ�ͷ�enumNode�ڲ������ڴ�
	enumNode.release(&enumNode);

	return 0;
}

//�������ؿ��
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
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
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
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
		pIntNode->release(pIntNode);
		return -1;
	}

	isIntNodeSuccess = pIntNode->getValue(pIntNode, &widthValue);
	if (0 != isIntNodeSuccess)
	{
		printf("get Width fail.\n");
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("after change ,Width is %d\n", widthValue);
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
		pIntNode->release(pIntNode);
	}

	return 0;
}
//�������ظ߶�
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
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
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
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
		pIntNode->release(pIntNode);
		return -1;
	}

	isIntNodeSuccess = pIntNode->getValue(pIntNode, &heightValue);
	if (0 != isIntNodeSuccess)
	{
		printf("get Height fail.\n");
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
		pIntNode->release(pIntNode);
		return -1;
	}
	else
	{
		printf("after change ,Height is %d\n", heightValue);
		//ע�⣺��Ҫ�ͷ�pIntNode�ڲ������ڴ�
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
		//ע�⣺��Ҫ�ͷ�pBoolNode�ڲ������ڴ�
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
		//ע�⣺��Ҫ�ͷ�pBoolNode�ڲ������ڴ�
		pBoolNode->release(pBoolNode);
		return -1;
	}

	isBoolNodeSuccess = pBoolNode->getValue(pBoolNode, &reverseXValue);
	if (0 != isBoolNodeSuccess)
	{
		printf("get ReverseX fail.\n");
		//ע�⣺��Ҫ�ͷ�pBoolNode�ڲ������ڴ�
		pBoolNode->release(pBoolNode);
		return -1;
	}
	else
	{
		printf("after change ,ReverseX is %u\n", reverseXValue);
		//ע�⣺��Ҫ�ͷ�pBoolNode�ڲ������ڴ�
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

	// ��׼�������
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
	//ע�⣺����֡�Ƿ���Ч������Ҫ�ͷ�pFrame�ڲ������ڴ�
	ret = pFrame->release(pFrame);

	return;
}
//��ʼץ��
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
//ֹͣץ��
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

		//ע�⣺��Ҫ�ͷ�cmdNode�ڲ������ڴ�
		cmdNode.release(&cmdNode);
		return -1;
	}

	//ע�⣺��Ҫ�ͷ�cmdNode�ڲ������ڴ�
	cmdNode.release(&cmdNode);

	return 0;
}

//�����ʼ��-OPT
bool CameraControler::initCamerasOPT()
{
	errorCode = CameraControler::NoError;

	//�ر��Ѿ��򿪵�������ͷ�����б�
	//�ر�ʧ�ܣ�����������Ϊ��
	this->closeCamerasOPT();
	
	//����ϵͳ����
    status = GENICAM_getSystemInstance(&pSystem);
	if (-1==status) 
	{
		errorCode = CameraControler::InitFailed;
		return false;
	}
	//�������
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
	////ϵͳ�е��豸���������趨ֵ
	//if (cameraCnt < adminConfig->MaxCameraNum) {
	//	errorCode = CameraControler::InitFailed; 
	//	return false;
	//}
	//�豸����
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

		//�������Խڵ� AcquisitionControl
		acquisitionControlInfo.pCamera = pCamera;
		status = GENICAM_createAcquisitionControl(&acquisitionControlInfo, &pAcquisitionCtrl);
		if (status != 0)
		{
			errorCode = CameraControler::InitFailed;
			return false;
		}
        //�����������

		//������û�д򿪣���ֱ������
	/*	if (!cameraState[i]) continue;*/
		//�����ع�ʱ�䣬��λus
		modifyCamralExposureTime(pCamera);
		// �޸�������ؿ��,�͸߶ȣ�ͨ��int�����Է���ʵ��
		modifyCameraWidth(pCamera);
		modifyCameraHeight(pCamera);

		//���ô���ģʽ
		status = setSoftTriggerConf(pAcquisitionCtrl);
		if (status != 0)
		{
			errorCode = CameraControler::InitFailed;
			//ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
			pAcquisitionCtrl->release(pAcquisitionCtrl);
			return false;
		}

		//����������
		status = GENICAM_CreateStreamSource(pCamera, &pStreamSource);
		if ((status != 0) || (NULL == pStreamSource))
		{
			printf("create stream obj  fail.\r\n");
			//ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
			pAcquisitionCtrl->release(pAcquisitionCtrl);
			return false;
		}
		//ע��ص���������ȡͼ��
		status = pStreamSource->attachGrabbing(pStreamSource, onGetFrame);
		if (status != 0)
		{
			printf("attch Grabbing fail!\n");
			//ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
			pAcquisitionCtrl->release(pAcquisitionCtrl);

			//ע�⣺��Ҫ�ͷ�pStreamSource�ڲ������ڴ�
			pStreamSource->release(pStreamSource);
			return false;
		}
		status = GENICAM_startGrabbing(pStreamSource);

		if (status != 0)
		{
			printf("StartGrabbing  fail.\n");
			/*isGrabbingFlag = 0;*/
			//ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
			pAcquisitionCtrl->release(pAcquisitionCtrl);

			//ע�⣺��Ҫ�ͷ�pStreamSource�ڲ������ڴ�
			pStreamSource->release(pStreamSource);
			return false;
		}
		else
		{
			/*isGrabbingFlag = 1;*/
		}
		////ִ��һ������
		//status = executeTriggerSoftware(pAcquisitionCtrl);
		//if (status != 0)
		//{
		//	printf("TriggerSoftware fail.\n");
		//	//ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
		//	pAcquisitionCtrl->release(pAcquisitionCtrl);

		//	//ע�⣺��Ҫ�ͷ�pStreamSource�ڲ������ڴ�
		//	pStreamSource->release(pStreamSource);
		//	return false;
		//}

	}
		////�޸����ReverseX��ͨ��bool�����Է���ʵ��
		//modifyCameraReverseX(pCamera);
	return true;

}



//�����״̬mapתΪ�ַ���
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

//�ж�����Ƿ��Ѿ���ʼ��
bool CameraControler::isCamerasInitialized()
{
	if (cameraState.size() <1 /*runtimeParams->nCamera*/) return false;

	for (int i = 0; i < 1/*runtimeParams->nCamera*/; i++) {
		int iCamera = adminConfig->MaxCameraNum - i - 1;
		if (!cameraState[iCamera]) return false;
	}
	return true;
}

//�ر��Ѿ��򿪵�����豸
void CameraControler::closeCameras()
{
	//�ر�����豸 - OpenCV
	for (int i = 0; i < cameraList.size(); i++)
		cameraList[i].release();
	cameraList.clear(); //����б�

	//�ر�����豸 - ��������
	for (int i = 0; i < cameraList2.size(); i++)
		CameraUnInit(cameraList2[i]);
	cameraList2.clear(); //����б�

	//������״̬
	cameraState.clear();
}

//�ر��Ѿ��򿪵�����豸
void CameraControler::closeCamerasOPT()
{

	//�ر�����豸 - OPT pCameraList[i]
	for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
		status = GENICAM_disconnect(&pCameraList[i]);
		if (status != 0)
		{
			printf("disconnect camera failed.\n");
			getchar();
			return;
		}
	}
	pCameraList = NULL;; //����б�

	//������״̬
	cameraState.clear();
}


/******************* ������� ********************/

//����ͼ�� - OpenCV
//���ʧ�ޣ�����ʹ��
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

//����ͼ�� - ��������
void CameraControler::takePhotos2()
{
	clock_t t1 = clock();
	Size frameSize(adminConfig->ImageSize_W, adminConfig->ImageSize_H);
	int colorMode = userConfig->colorMode; //ɫ��ģʽ
	int dataType = (colorMode == 1) ? CV_8UC1 : CV_8UC3; //Mat����������

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
		//CameraSoftTrigger(camList[i]);//ִ��һ��������ִ�к󣬻ᴥ����CameraSetTriggerCountָ����֡����
		//CameraGetImageBuffer(camList[i], &FrameInfo, &pRawBuffer, 10000);//ץһ��ͼ
		
		double t0 = clock();
		int counter = 5;
		while (counter > 0) {
		

			CameraClearBuffer(cameraList2[iCamera]);
			CameraSoftTrigger(cameraList2[iCamera]);
			int flag = CameraGetImageBuffer(cameraList2[iCamera], &FrameInfo, &pRawBuffer, 10000);
			if (CAMERA_STATUS_SUCCESS == flag) break; //ץһ��ͼ
			counter--;
		}
		//CameraSnapToBuffer(camList[i], &FrameInfo, &pRawBuffer, 10000);//ץһ��ͼ

		//����һ��buffer����������õ�ԭʼ����ת��ΪRGB���ݣ���ͬʱ���ͼ����Ч��
		counter = 10;
		int bufferSize = FrameInfo.iWidth * FrameInfo.iHeight * (colorMode==1 ? 1 : 3);
		while (counter > 0) {
			pRgbBuffer = (unsigned char *) CameraAlignMalloc(bufferSize, 16);
			if (pRgbBuffer != NULL) break;
			counter--;
		}

		//����ͼ�񣬲��õ�RGB��ʽ������
		CameraImageProcess(cameraList2[iCamera], pRawBuffer, pRgbBuffer, &FrameInfo);
		//�ͷ���CameraSnapToBuffer��CameraGetImageBuffer��õ�ͼ�񻺳���
		while (CameraReleaseImageBuffer(cameraList2[iCamera], pRawBuffer) != CAMERA_STATUS_SUCCESS);
		//��pRgbBufferת��ΪMat��
		cv::Mat fram(frameSize, dataType, pRgbBuffer);

		cv::flip(fram, fram, -1); //ֱ�ӻ�ȡ��ͼ��ʱ���ģ�������ת180��
		cv::flip(fram, fram, 1);
		cv::Mat* pMat = new cv::Mat(fram.clone());
		(*cvmatSamples)[*currentRow][i] = pMat;
		CameraAlignFree(pRgbBuffer);
	} 

	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("�����ͼ��") << (t2 - t1) << "ms" 
		<< "( currentRow_show =" << *currentRow << ")" << endl;
	return;
}

//����ͼ�� - OPT
void CameraControler::takePhotos3()
{
	clock_t t1 = clock();
	//int colorMode = userConfig->colorMode; //ɫ��ģʽ
	//int dataType = (colorMode == 1) ? CV_8UC1 : CV_8UC3; //Mat����������

	for (int i = 0; i < runtimeParams->nCamera; i++) {
		//ִ��һ������
	/*	isGrabbingFlag = 1;*/
		status = executeTriggerSoftware(pAcquisitionCtrl);
		if (status != 0)
		{
			printf("TriggerSoftware fail.\n");
			//ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
			pAcquisitionCtrl->release(pAcquisitionCtrl);

			//ע�⣺��Ҫ�ͷ�pStreamSource�ڲ������ڴ�
			pStreamSource->release(pStreamSource);
			return;
		}
		Sleep(2000);

		//����bug,����û����ص��������ͻ�������ѭ��
		//while (isGrabbingFlag)
		//{
		//	Sleep(50);
		//}

		////ע�⣺��Ҫ�ͷ�pAcquisitionCtrl�ڲ������ڴ�
		//pAcquisitionCtrl->release(pAcquisitionCtrl);

		(*cvmatSamples)[*currentRow][i] = pImageFrame;

		////ע���ص�����
	/*	status = pStreamSource->detachGrabbing(pStreamSource, onGetFrame);
		if (status != 0)*/
		//{
		//	printf("detachGrabbing  fail.\n");
		//}

		//// stop grabbing from camera
		////ֹͣץ��
		//status = GENICAM_stopGrabbing(pStreamSource);
		//if (status != 0)
		//{
		//	printf("Stop Grabbing  fail.\n");
		//}

		////ע�⣺��Ҫ�ͷ�pStreamSource�ڲ������ڴ�
		//pStreamSource->release(pStreamSource);



		//��ͼ��ת��ΪMat��
		//if (colorMode == 1) {
		//	//�����ʽ��Mono8ʱ
		//	cv::Mat image = cv::Mat(pFrame->getImageHeight(pFrame),
		//		pFrame->getImageWidth(pFrame),
		//		CV_8U,
		//		(uint8_t*)((pFrame->getImage(pFrame)));
		//}
		//else {
		//	//���ͼ���ʽΪ��ɫ��ʽ
		//	cv::Mat image = cv::Mat(pFrame->getImageHeight(pFrame),
		//		pFrame->getImageWidth(pFrame),
		//		CV_8UC3,
		//		(uint8_t*)pBGRbuffer);
		//}



		// disconnect camera
		//�Ͽ��豸
		
	/*	return;*/
	}
	clock_t t2 = clock();
	qDebug() << "====================" << pcb::chinese("�����ͼ��") << (t2 - t1) << "ms"
		<< "( currentRow_show =" << *currentRow << ")" << endl;
	return;
}



/********************* ���� ********************/

//�����豸��
CameraControler::ErrorCode CameraControler::resetDeviceIndex(std::vector<int> iv)
{
	deviceIndex = iv;
	for (int i = 0; i < cameraList.size(); i++) {
		cameraList[i].release();
	}     
	return initCameras();
}

//��������
bool CameraControler::showMessageBox(QWidget *parent)
{
	if (errorCode == CameraControler::NoError) return false;

	QString warningMessage;
	switch (errorCode)
	{
	case CameraControler::Unchecked:
		warningMessage = QString::fromLocal8Bit("���״̬δȷ�ϣ�"); break;
	case CameraControler::InvalidCameraNum:
		warningMessage = QString::fromLocal8Bit("��ǰ���õ����������Ч��"); break;
	case CameraControler::InitFailed:
		warningMessage = QString::fromLocal8Bit("�����ʼ��ʧ�ܣ�     \n");
		warningMessage += "CameraState: " + cameraStatusMapToString(); break;
	case CameraControler::TakePhotosFailed:
		warningMessage = QString::fromLocal8Bit("����ʧ�ܣ�"); break;
	default:
		warningMessage = QString::fromLocal8Bit("δ֪����"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
		warningMessage + "    \n" +
		"CameraControler: ErrorCode: " + QString::number(errorCode),
		QString::fromLocal8Bit("ȷ��"));
	return true;
}

//int CameraControler::isGrabbingFlag = 0;
cv::Mat* CameraControler::pImageFrame;