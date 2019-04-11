#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "DetectFunc.h"


//��������
class DefectDetecter : public QObject
{
	Q_OBJECT

public:
	enum DetectState {
		InitialState,
		Start,
		Finished,
		Default
	};

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���

	DetectFunc *detectFunc;
	pcb::CvMatArray *cvmatSamples; //���ڼ�������
	pcb::DetectResult *detectResult; //�����
	int detectState; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�
	int defectNum; //ȱ����

public:
	DefectDetecter();
	~DefectDetecter();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //ϵͳ����
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; } //�û�����
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; } //���в���
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; } //����ͼ
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; } //�����

	void initDetectFunc();
	void detect();

private:
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void updateDetectState_detecter(int state);
};
