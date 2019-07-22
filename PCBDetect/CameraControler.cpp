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
	closeCameras(); //�ر�������ͷ�����б�
}

//�����߳�
void CameraControler::run()
{
	switch (operation) {
	case Operation::NoOperation: //�޲���
		break;
	case Operation::InitCameras: //��ʼ��
		this->initCameras2();
		emit initCamerasFinished_camera(errorCode);
		break;
	case Operation::TakePhotos: //����
		this->takePhotos2();
		emit takePhotosFinished_camera(errorCode);
		break;
	}
}


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
		//CameraSetExposureTime(cameraList2[i], userConfig->exposureTime * 1000);
		CameraSetExposureTime(cameraList2[i], 200 * 1000);
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
	if (cameraState.size() < runtimeParams->nCamera) return false;

	for (int i = 0; i < runtimeParams->nCamera; i++) {
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
