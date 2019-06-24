#pragma once

#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
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
		Unchecked = 0x500,
		InitFailed = 0x501,
		InvalidCameraNum = 0x502,
		TakePhotosFailed = 0x503,
		Default = 0x5FF
	};

private:
	int caller; //ģ��ĵ�����
	pcb::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	int *currentRow; //��ǰ�к�

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���

	std::vector<int> deviceIndex = {}; //�豸��
	std::vector<cv::VideoCapture> cameraList; //����б�
	std::map<int, bool> cameraState; //���״̬ <�豸��,״ֵ̬>

	std::vector<CameraHandle> cameraList2;
	tSdkCameraDevInfo sCameraList[12]; //��಻�ܳ���12̨���
	int CameraNums = 12;

	double *pfExposureTime = NULL;
	tSdkImageResolution sImageSize;

	ErrorCode errorCode; //�������Ĵ�����
	Operation operation; //����ָ��

public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCaller(int c) { caller = c; };
	inline int getCaller() { return caller; }

	inline void setCvMatSamples(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	inline void setDeviceIndex(std::vector<int> &iv) { deviceIndex = iv; }
	inline void setOperation(Operation op) { operation = op; }

	ErrorCode resetDeviceIndex(std::vector<int> iv = {}); //�趨��������豸��
	inline bool getCameraState(int index) { return cameraState[index]; }

	inline bool isReady() { return errorCode == NoError; }
	inline ErrorCode getErrorCode() { return errorCode; } //��ȡ��ǰ�Ĵ������
	bool showMessageBox(QWidget *parent); //��������

private:
	ErrorCode initCameras(); //��ʼ�� - OpenCV
	bool initCameras2(); //��ʼ�� - ��������
	QString cameraStatusMapToString(); //���״̬ת�ַ���

	bool isCamerasInitialized(); //�ж�����Ƿ��Ѿ���ʼ��
	void closeCameras();//�ر��Ѿ��򿪵����

	ErrorCode takePhotos();//����
	void takePhotos2();

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);
};
