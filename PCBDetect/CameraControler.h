#pragma once
//#include"Windows.h"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <map>
#include <iterator>
#include "AMC98C.h"
#include "Windows.h"
#include <CameraApi.h>
#include "GenICam/CAPI/SDK.h"
#include "ImageConvert.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>



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

	int32_t status;//״̬

	//OPT
	std::vector<GENICAM_Camera*> pCameraList2;
	/*GENICAM_Frame *pFrame = NULL;*/
	GENICAM_System *pSystem = NULL;//�����豸�������������Զ���
	GENICAM_Camera *pCamera = NULL;//�������ӿ���
	GENICAM_Camera *pCameraList = NULL;
	GENICAM_StreamSource *pStreamSource = NULL;
	GENICAM_AcquisitionControl *pAcquisitionCtrl = NULL;//�����Բ�����
	GENICAM_AcquisitionControlInfo acquisitionControlInfo = { 0 };

	bool m_bRun; //��ʾ�̱߳�־

	uint32_t cameraCnt = 12;//�����Ŀ
	HANDLE threadHandle;
	unsigned threadID;//�߳���
	int cameraIndex = -1;


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
	/*static int isGrabbingFlag ;*/
	static cv::Mat* pImageFrame;

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
	//OPT
	//�Ͽ����ӣ����
	int32_t GENICAM_disconnect(GENICAM_Camera *pGetCamera);
	//�������
	int32_t GENICAM_connect(GENICAM_Camera *pGetCamera);
	//�޸��ع�ʱ��
	int32_t modifyCamralExposureTime(GENICAM_Camera *pGetCamera);
	//�޸�ͼ����
	int32_t modifyCameraWidth(GENICAM_Camera *pGetCamera);
	//�޸�ͼ��߶�
	int32_t modifyCameraHeight(GENICAM_Camera *pGetCamera);
	//�޸����X����
	int32_t modifyCameraReverseX(GENICAM_Camera *pGetCamera);
	//���ô�����ʽ
	int32_t setSoftTriggerConf(GENICAM_AcquisitionControl *pAcquisitionCtrl);
	//����������
	int32_t GENICAM_CreateStreamSource(GENICAM_Camera *pGetCamera, GENICAM_StreamSource **ppStreamSource);
	//��ʼץ��
	int32_t GENICAM_startGrabbing(GENICAM_StreamSource *pStreamSource);
	//ֹͣץ��
	int32_t GENICAM_stopGrabbing(GENICAM_StreamSource *pStreamSource);
	//ִ�д���
	int32_t executeTriggerSoftware(GENICAM_AcquisitionControl *pAcquisitionCtrl);

	ErrorCode initCameras(); //��ʼ�� - OpenCV
	bool initCameras2(); //��ʼ�� - ��������
	bool initCamerasOPT();//��ʼ��-OPT
	QString cameraStatusMapToString(); //���״̬ת�ַ���

	bool isCamerasInitialized(); //�ж�����Ƿ��Ѿ���ʼ��
	void closeCameras();//�ر��Ѿ��򿪵����
	void closeCamerasOPT();//�ر��Ѿ��򿪵����-OPT

	ErrorCode takePhotos();//����
	void takePhotos2();
	void takePhotos3();//����ͼ�� - OPT

protected:
	void run();
	
Q_SIGNALS:
	void initCamerasFinished_camera(int);
	void takePhotosFinished_camera(int);
};
void onGetFrame(GENICAM_Frame* pFrame);
