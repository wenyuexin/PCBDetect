#pragma once
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include "DetectFunc.h"


//��������
class DefectDetecter : public QObject
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���
	pcb::DetectResult *detectResult; //�����
	pcb::CvMatVector *samples; //���ڼ���һ������
	int *currentRow; //��ǰ�������кţ���Ӧ�ڼ����ģ�
	int detectState; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�

	int defectNum = 0;//ȱ�ݱ��

public:
	DefectDetecter();
	~DefectDetecter();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig* ptr) { detectConfig = ptr; } //���������ļ���������ý���
	inline void setDetectParams(pcb::DetectParams* ptr) { detectParams = ptr; } //��ʱ��������ʱ����
	inline void setSampleImages(pcb::CvMatVector* ptr) { samples = ptr; } //��ǰ���ڼ���һ������
	inline void setDetectResult(pcb::DetectResult* ptr) { detectResult = ptr; } //�����

	void doDetect();

private:
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void sig_detectState_detectCore(int state);
};

