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
	//�˴��������պ���
	//��������󽫽������ (*cvmatSamples)[*currentRow] ��
	takePhotos();
	emit takePhotoFinished_camera();
}

/******************* ��� *********************/

//�����ʼ��
//�������˳�����豸˳��һ��ʱ����Ҫʹ�õڶ�������
//�������ʵ������˳���Ӧ���豸��Ž��г�ʼ��,ϵͳ�豸��Ŵ�0��ʼ
CameraControler::ErrorCode CameraControler::initCameras(const std::vector<int> &iv)
{
	*nCamera = 1;

	if (*nCamera <= 0)
		return CameraControler::InvalidCameraNum;

	CameraControler::ErrorCode errorCode = CameraControler::NoError;
	if (iv.size() == 0) { //ʹ��Ĭ�ϵ��豸�ų�ʼ��
		if (*nCamera > *MaxCameraNum) //�жϵ��õ���������Ƿ����
			return CameraControler::InvalidCameraNum;

		availableCameras.resize(*nCamera); 
		for (int i = 0; i < *nCamera; i++) { //������
			cameraList.push_back(cv::VideoCapture(i));
			availableCameras[i] = cameraList[i].isOpened();
			if (!availableCameras[i]) 
				errorCode = CameraControler::CameraInitFailed;
		}
	}
	else { //ʹ���趨���豸�ų�ʼ��
		if (*nCamera > iv.size()) //�жϵ��õ���������Ƿ����
			return CameraControler::InvalidCameraNum;

		for (int i = 0; i < iv.size(); i++) { //������
			cameraList.push_back(cv::VideoCapture(iv[i]));
			availableCameras[i] = cameraList[i].isOpened();
			if (!availableCameras[i])
				errorCode = CameraControler::CameraInitFailed;
		}
	}
	return errorCode;
}

//����ͼ��
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

//�����豸��
CameraControler::ErrorCode CameraControler::resetDeviceNum(int nDevice, std::vector<int> iv)
{
	//int old_num = *MaxCameraNum;
	//*MaxCameraNum = nDevice;
	//for (int i = 0; i < old_num; i++)
	//	cameraList[i].release();
	return initCameras(iv);
}


//��������
void CameraControler::showMessageBox(QWidget *parent, CameraControler::ErrorCode code) {
	QString message;
	switch (code)
	{
	case CameraControler::Uncheck:
		message = QString::fromLocal8Bit("���״̬δȷ��"); break;
	case CameraControler::InvalidCameraNum:
		message = QString::fromLocal8Bit("��ǰ���õ����������Ч"); break;
	case CameraControler::CameraInitFailed:
		message = QString::fromLocal8Bit("�����ʼ��ʧ��"); break;
	default:
		message = QString::fromLocal8Bit("δ֪����"); break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
		message + "!    \n" +
		"CameraControler: ErrorCode: " + QString::number(code),
		QString::fromLocal8Bit("ȷ��"));
}