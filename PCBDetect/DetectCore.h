#pragma once
#include "Configurator.h"
#include "opencv2/opencv.hpp"
#include "DetectFunc.h"


//��������
class DetectCore : public QObject
{
	Q_OBJECT

private:
	pcb::DetectConfig *config; //���ڼ��Ĳ�������
	pcb::DetectParams *params; //���ڼ��Ĳ�������
	pcb::DetectResult *result; //�����
	pcb::CvMatVector *samples; //���ڼ���һ������
	int *currentRow; //��ǰ�������кţ���Ӧ�ڼ����ģ�
	int detectState; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�

	int defectNum = 0;//ȱ�ݱ��

public:
	DetectCore();
	~DetectCore();

	inline void setDetectConfig(pcb::DetectConfig* ptr = Q_NULLPTR) { config = ptr; } //���������ļ���������ý���
	inline void setDetectParams(pcb::DetectParams* ptr = Q_NULLPTR) { params = ptr; } //��ʱ��������ʱ����
	inline void setSampleImages(pcb::CvMatVector* ptr = Q_NULLPTR) { samples = ptr; } //��ǰ���ڼ���һ������
	inline void setDetectResult(pcb::DetectResult* ptr = Q_NULLPTR) { result = ptr; } //�����

	void doDetect();

private:
	//cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);
	//cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);

Q_SIGNALS:
	void sig_detectState_detectCore(int state);
};

