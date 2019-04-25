#include "CameraControler.h"

using cv::Mat;
using pcb::CvMatVector;

#ifdef _WIN64
#pragma comment(lib, ".\\MVCAMSDK_X64.lib")
#else
#pragma comment(lib, ".\\MVCAMSDK.lib")
#endif

CameraControler::CameraControler(QThread *parent)
	: QThread(parent)
{
	errorCode = Uncheck; //�������Ĵ�����
	operation = NoOperation;//����ָ��
}

CameraControler::~CameraControler()
{
	qDebug() << "~CameraControler";

	//�ر�����豸
	for (int i = 0; i < cameraList.size(); i++)
		cameraList[i].release();

	for (int i = 0; i < cameraList2.size(); i++)
		CameraUnInit(cameraList2[i]);
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

/******************* �����ʼ�������� *********************/

//�����ʼ�� - OpenCV
//�������˳�����豸˳��һ��ʱ����Ҫʹ�õڶ�������
//�������ʵ������˳���Ӧ���豸��Ž��г�ʼ��,ϵͳ�豸��Ŵ�0��ʼ
CameraControler::ErrorCode CameraControler::initCameras()
{
	if (detectParams->nCamera <= 0)
		return errorCode = CameraControler::InvalidCameraNum;

	//��ʼ������б�
	errorCode = CameraControler::NoError;
	if (deviceIndex.size() == 0) { //ʹ��Ĭ�ϵ��豸�ų�ʼ��
		if (detectParams->nCamera > adminConfig->MaxCameraNum) //�жϵ��õ���������Ƿ����
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < detectParams->nCamera; i++) { //������
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
		if (detectParams->nCamera > deviceIndex.size()) //�жϵ��õ���������Ƿ����
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
	if (CameraSdkInit(1) != CAMERA_STATUS_SUCCESS) { //���sdk��ʼ��ʧ��
		errorCode = CameraControler::InitFailed;
		return false;
	}

	errorCode = CameraControler::NoError;
	CameraSdkStatus status;

	if (CameraEnumerateDevice(sCameraList, &CameraNums) != CAMERA_STATUS_SUCCESS ||
		CameraNums < adminConfig->MaxCameraNum)
	{
		//ö���豸ʧ�ܻ���ϵͳ�е��豸���������趨ֵ
		return false;
	}

	if (deviceIndex.empty() || deviceIndex.size() < adminConfig->MaxCameraNum) {
		for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
			cameraList2.push_back(-1);
			status = CameraInit(&sCameraList[i], -1, -1, &cameraList2[i]);
			cameraState[i] = (status != CAMERA_STATUS_SUCCESS);
			if (status != CAMERA_STATUS_SUCCESS) { //������һ̨�����ʼ��ʧ��
				errorCode = CameraControler::InitFailed;
				//return false;
			}
		}
	}
	else {
		for (int i = 0; i < adminConfig->MaxCameraNum; i++) {
			cameraList.push_back(-1);
			status = CameraInit(&sCameraList[deviceIndex[i]], -1, -1, &cameraList2[i]);
			cameraState[i] = (status != CAMERA_STATUS_SUCCESS);
			if (status != CAMERA_STATUS_SUCCESS) {//������һ̨�����ʼ��ʧ��
				errorCode = CameraControler::InitFailed;
				//return false;
			}
		}
	}

	double *pfExposureTime = NULL;
	tSdkImageResolution sImageSize;
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
		//��ʼ�ɼ�ͼ��
		//��������Ĵ���ģʽ��0��ʾ�����ɼ�ģʽ��1��ʾ�������ģʽ��2��ʾӲ������ģʽ��
		CameraSetTriggerMode(cameraList2[i], 1);
		CameraSetTriggerCount(cameraList2[i], 5);
		if (CameraSetResolutionForSnap(cameraList2[i], &sImageSize) != CAMERA_STATUS_SUCCESS)
			return false;
		CameraPlay(cameraList2[i]);
	}
	return true;
}

//�����״̬mapתΪ�ַ���
QString CameraControler::cameraStatusMapToString()
{
	QString available = "";
	std::map<int, bool>::iterator iter;
	for (iter = cameraState.begin(); iter != cameraState.end(); iter++) {
		available += (iter->second) ? "1" : "0";
	}
	return available;
}

//����ͼ��
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
	pcb::delay(8000);

	for (int i = 0; i < detectParams->nCamera; i++) {
		int iCamera = detectParams->nCamera - i - 1;
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

void CameraControler::takePhotos2()
{
	for (int i = 0; i < detectParams->nCamera; i++) {
		BYTE *pRawBuffer;
		BYTE *pRgbBuffer;
		tSdkFrameHead FrameInfo;

		//CString sFileName;
		tSdkImageResolution sImageSize;

		CameraSdkStatus status;
		//CString msg;
		//memset(&sImageSize, 0, sizeof(tSdkImageResolution));
		//sImageSize.iIndex = 0xff;
		//CameraGetInformation(camList[i], &FrameInfo)
		//CameraSoftTrigger(camList[i]);//ִ��һ��������ִ�к󣬻ᴥ����CameraSetTriggerCountָ����֡����
		//CameraGetImageBuffer(camList[i], &FrameInfo, &pRawBuffer, 10000);//ץһ��ͼ
		CameraClearBuffer(cameraList2[i]);
		CameraSoftTrigger(cameraList2[i]);
		double t0 = clock();
		CameraGetImageBuffer(cameraList2[i], &FrameInfo, &pRawBuffer, 10000);//ץһ��ͼ

		//CameraSnapToBuffer(camList[i], &FrameInfo, &pRawBuffer, 10000);//ץһ��ͼ

		//����һ��buffer����������õ�ԭʼ����ת��ΪRGB���ݣ���ͬʱ���ͼ����Ч��
		pRgbBuffer = (unsigned char *)CameraAlignMalloc(FrameInfo.iWidth*FrameInfo.iHeight * 3, 16);

		CameraImageProcess(cameraList2[i], pRawBuffer, pRgbBuffer, &FrameInfo);//����ͼ�񣬲��õ�RGB��ʽ������

		while (CameraReleaseImageBuffer(cameraList2[i], pRawBuffer) != CAMERA_STATUS_SUCCESS);//�ͷ���CameraSnapToBuffer��CameraGetImageBuffer��õ�ͼ�񻺳���
		cv::Mat fram( //��pRgbBufferת��ΪMat��
			cv::Size(adminConfig->ImageSize_W, adminConfig->ImageSize_H), //width��height
			//FrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
			CV_8UC3,
			pRgbBuffer
		);
		cv::flip(fram, fram, -1); //ֱ�ӻ�ȡ��ͼ��ʱ���ģ�������ת180��
		cv::flip(fram, fram, 1);
		cv::Mat* pMat = new cv::Mat(fram.clone());
		int iCamera = detectParams->nCamera - i - 1;
		(*cvmatSamples)[*currentRow][iCamera] = pMat;
		CameraAlignFree(pRgbBuffer);
	}

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
	case CameraControler::Uncheck:
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
