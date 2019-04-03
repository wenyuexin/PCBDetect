#include "CameraControler.h"


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
	case Operation::TakePhoto: //����
	    //��������󽫽������ (*cvmatSamples)[*currentRow] ��
		takePhotos();
		emit takePhotosFinished_camera(errorCode);
		break;
	case Operation::NoOperation:
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
	if (*nCamera <= 0)
		return errorCode = CameraControler::InvalidCameraNum;

	errorCode = CameraControler::NoError;
	if (deviceIndex.size() == 0) { //ʹ��Ĭ�ϵ��豸�ų�ʼ��
		if (*nCamera > *MaxCameraNum) //�жϵ��õ���������Ƿ����
			return errorCode = CameraControler::InvalidCameraNum;

		//availableCameras.put(*nCamera); 
		for (int i = 0; i < *nCamera; i++) { //������
			cameraList.push_back(cv::VideoCapture(i));
			cameraState[i] = cameraList[i].isOpened(); //�ж�����Ƿ��ܴ�
			if (!cameraState[i]) errorCode = CameraControler::CameraInitFailed;
		}
	}
	else { //ʹ���趨���豸�ų�ʼ��
		if (*nCamera > deviceIndex.size()) //�жϵ��õ���������Ƿ����
			return errorCode = CameraControler::InvalidCameraNum;

		for (int i = 0; i < deviceIndex.size(); i++) { //������
			cameraList.push_back(cv::VideoCapture(deviceIndex[i]));
			cameraState[i] = cameraList[i].isOpened(); //�ж�����Ƿ��ܴ�
			if (!cameraState[i]) errorCode = CameraControler::CameraInitFailed;
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

	cv::Mat frame;
	for (int i = 0; i < *nCamera; i++) {
		cameraList[i] >> frame;
		cv::Mat* pMat = new cv::Mat(frame);
		(*cvmatSamples)[*currentRow][i] = pMat;
	}
	return errorCode;
}

//�����豸��
CameraControler::ErrorCode CameraControler::resetDeviceIndex(std::vector<int> iv)
{
	//int old_num = *MaxCameraNum;
	//*MaxCameraNum = nDevice;
	//for (int i = 0; i < old_num; i++)
	//	cameraList[i].release();
	return initCameras();
}

//��������
void CameraControler::showMessageBox(QWidget *parent)
{
	if (errorCode == CameraControler::NoError) return;

	QString warningMessage;
	switch (errorCode)
	{
	case CameraControler::Uncheck:
		warningMessage = QString::fromLocal8Bit("���״̬δȷ�ϣ�"); break;
	case CameraControler::InvalidCameraNum:
		warningMessage = QString::fromLocal8Bit("��ǰ���õ����������Ч��"); break;
	case CameraControler::CameraInitFailed:
		warningMessage = QString::fromLocal8Bit("�����ʼ��ʧ�ܣ�     \n");
		warningMessage += "CameraState: " + cameraStatusMapToString(); break;
	default:
		warningMessage = QString::fromLocal8Bit("δ֪����"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
		warningMessage + "    \n" +
		"CameraControler: ErrorCode: " + QString::number(errorCode),
		QString::fromLocal8Bit("ȷ��"));
}
