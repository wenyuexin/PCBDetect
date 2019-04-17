#include "CameraControler.h"

using cv::Mat;
using pcb::CvMatVector;


CameraControler::CameraControler(QThread *parent)
	: QThread(parent)
{
}

CameraControler::~CameraControler()
{
	//�ر�����豸
	for (int i = 0; i < cameraList.size(); i++)
		cameraList[i].release();
}

//�����߳�
void CameraControler::run()
{
	switch (operation) {
	case Operation::InitCameras: //��ʼ��
		initCameras();
		emit initCamerasFinished_camera(errorCode);
		break;
	case Operation::TakePhotos: //����
	    //��������󽫽������ (*cvmatSamples)[*currentRow] ��
		takePhotos();
		emit takePhotosFinished_camera(errorCode);
		break;
	case Operation::NoOperation:
		break;
	default:
		break;
	}
}

/******************* ��� *********************/

//�����ʼ��
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
				cameraList[i].set(cv::CAP_PROP_BUFFERSIZE, 0);
			}
		}
	}
	return errorCode;
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
	default:
		warningMessage = QString::fromLocal8Bit("δ֪����"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
		warningMessage + "    \n" +
		"CameraControler: ErrorCode: " + QString::number(errorCode),
		QString::fromLocal8Bit("ȷ��"));
	return true;
}
