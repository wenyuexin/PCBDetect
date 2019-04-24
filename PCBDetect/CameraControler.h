#pragma once

#include "Configurator.h"
#include "RuntimeLib.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <map>
#include <iterator>
#include "AMC98C.h"
#include <CameraApi.h>


//���������
class CameraControler : public QThread
{
	Q_OBJECT

public:
	//�������ز���
	enum Operation {
		NoOperation,
		InitCameras,
		TakePhotos
	};

	//����Ĵ������
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x500,
		InitFailed = 0x501,
		InvalidCameraNum = 0x502,
		TakePhotosFailed = 0x503,
		Default = 0x5FF
	};

private:
	pcb::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	int *currentRow; //��ǰ�к�

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���

	std::vector<int> deviceIndex = {}; //�豸��
	std::vector<cv::VideoCapture> cameraList; //����б�
	std::map<int, bool> cameraState; //���״̬ <�豸��,״ֵ̬>

	std::vector<CameraHandle> cameraList2;
	tSdkCameraDevInfo sCameraList[12]; //��಻�ܳ���12̨���
	int CameraNums = 12;

	ErrorCode errorCode; //�������Ĵ�����
	Operation operation; //����ָ��

public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCvMatSamples(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	inline void setDeviceIndex(std::vector<int> &iv) { deviceIndex = iv; }
	inline void setOperation(Operation op) { operation = op; }

	ErrorCode resetDeviceIndex(std::vector<int> iv = {}); //�趨��������豸��
	inline bool checkCameraState(int index) { return cameraState[index]; }

	inline bool isReady() { return errorCode == NoError; }
	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	bool showMessageBox(QWidget *parent); //��������

private:
	ErrorCode initCameras();//��ʼ��
	bool initCameras2();
	QString cameraStatusMapToString(); //���״̬ת�ַ���
	ErrorCode takePhotos();//��������
	void takePhotos2();

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);
};
