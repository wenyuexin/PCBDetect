#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "DetectUnit.h"
#include "pcbFuncLib.h"
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
		LoadTemplMaskRoiError = 0x901,
		Default = 0x9FF
	};

private:
	ErrorCode errorCode; //�������
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	std::vector<DetectUnit *> detectUnits; //��ⵥԪ���̣߳���һ����Ԫһ�ο��Լ��һ�ŷ�ͼ
	const int MAX_DETECT_UNITS_NUM = 10;

	DetectFunc *detectFunc;
	pcb::CvMatArray *cvmatSamples; //���ڼ�������
	pcb::DetectResult *detectResult; //�����
	int detectState; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�
	int totalDefectNum; //����PCB���е�ȱ������

	double scalingFactor; //��������
	cv::Size scaledFullImageSize; //�������ź����ͼ�ĳߴ�
	cv::Size scaledSubImageSize; //�������ź�ķ�ͼ�ĳߴ�
	cv::Mat bigTempl; //��ģ��

	std::map<cv::Point3i, cv::Mat, cmp_point3i> allDetailImage;

	cv::Point maskRoi_bl; //��ģ��������꣬һ��PCB��Ӧһ������ 
	cv::Point maskRoi_tr;

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
	void generateBigTempl();

	void makeCurrentSampleDir(std::vector<QString> &subFolders = std::vector<QString>());
	void makeCurrentOutputDir(std::vector<QString> &subFolders = std::vector<QString>());

	void initDetectUnits();
	void deleteDetectUnits();

	inline cv::Mat getBigTempl() { return bigTempl; }
	inline cv::Mat getBigTempl(cv::Rect &rect) { return bigTempl(rect); }
	

	//inline cv::Point getMaskRoi_bl() { return maskRoi_bl; }
	//inline cv::Point getMaskRoi_tr() { return maskRoi_tr; }
	inline void setMaskRoi_bl(cv::Point pt) { maskRoi_bl = pt; }
	inline void setMaskRoi_tr(cv::Point pt) { maskRoi_tr = pt; }


Q_SIGNALS:
	void updateDetectState_detecter(int state);
	void detectFinished_detectThread(bool qualified);
};
