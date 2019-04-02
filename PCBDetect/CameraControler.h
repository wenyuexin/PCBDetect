#pragma once

#include "Configurator.h"
#include "opencv2/opencv.hpp"
#include <QThread>

//���������
class CameraControler : public QThread
{
	Q_OBJECT

public:
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
	std::vector<cv::VideoCapture> cameraList;//����б�
	std::vector<bool> availableCameras; //��ʹ�õ����


public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCvMatSamples(Ui::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }
	inline void setCameraNum(int *num) { nCamera = num; }
	inline void setMaxCameraNum(int *num) { MaxCameraNum = num; }

	ErrorCode initCameras(const std::vector<int>& iv = {});//��ʼ��
	//inline bool isInitialized() { return (errorCode == CameraControler::NoError); } //�ж�ʱ���Ѿ���ʼ��

	void takePhotos();//��������
	ErrorCode resetDeviceNum(int, std::vector<int> iv = {}); //�趨��������豸��

	static void showMessageBox(QWidget *parent, CameraControler::ErrorCode code);

protected:
	void run();
	
Q_SIGNALS:
	void takePhotoFinished_camera();

};
