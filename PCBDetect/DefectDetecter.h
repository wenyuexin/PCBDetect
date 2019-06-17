#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "DetectFunc.h"


//��������
class DefectDetecter : public QObject
{
	Q_OBJECT

public:
	//���״̬
	enum DetectState {
		InitialState,
		Start,
		Finished,
		Error
	};

	//�������
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x900,
		Default = 0x9FF
	};

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���

	DetectFunc *detectFunc;
	pcb::CvMatArray *cvmatSamples; //���ڼ�������
	pcb::DetectResult *detectResult; //�����
	int detectState; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�
	int defectNum; //ȱ����

public:
	DefectDetecter();
	~DefectDetecter();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //ϵͳ����
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //�û�����
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //���в���
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; } //����ͼ
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; } //�����

	void detect();

private:
	void initDetectFunc();
	void makeCurrentOutputDir(std::vector<QString> &);
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void updateDetectState_detecter(int state);
	void detectFinished_detectThread(bool qualified);
};
