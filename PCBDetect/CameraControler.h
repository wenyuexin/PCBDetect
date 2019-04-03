#pragma once

#include "Configurator.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <map>
#include <iterator>

//���������
class CameraControler : public QThread
{
	Q_OBJECT

public:
	enum Operation {
		NoOperation,
		InitCameras,
		TakePhoto
	};

	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x400,
		CameraInitFailed = 0x401,
		InvalidCameraNum = 0x402
	};

private:
	Ui::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	int *currentRow; //��ǰ�к�
	int *nCamera; //��ǰʹ�õ��������
	int *MaxCameraNum; //��ʹ�õ��������

	std::vector<int> deviceIndex = {}; //�豸��
	std::vector<cv::VideoCapture> cameraList; //����б�
	std::map<int, bool> cameraState; //���״̬ <�豸��,״ֵ̬>
	ErrorCode errorCode = Uncheck; //�������Ĵ�����
	Operation operation = NoOperation;//����ָ��

public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCvMatSamples(Ui::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }
	inline void setCameraNum(int *num) { nCamera = num; }
	inline void setMaxCameraNum(int *num) { MaxCameraNum = num; }
	inline void setDeviceIndex(std::vector<int> &iv) { deviceIndex = iv; }
	inline void setOperation(Operation op) { operation = op; }

	ErrorCode initCameras();//��ʼ��
	QString cameraStatusMapToString(); //���״̬ת�ַ���
	ErrorCode takePhotos();//��������
	ErrorCode resetDeviceIndex(std::vector<int> iv = {}); //�趨��������豸��
	inline bool checkCameraState(int index) { return cameraState[index]; }

	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	void showMessageBox(QWidget *parent); //��������

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);

};
