#include "DetectThread.h"

using pcb::CvMatVector;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::DetectResult;
using pcb::QPixmapVector;
using pcb::QImageVector;
using pcb::QImageArray;


DetectThread::DetectThread()
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	detectConfig = Q_NULLPTR; //�û�����
	detectParams = Q_NULLPTR; //���в���
	cvmatSamples = Q_NULLPTR; //������ȡ������ͼ
	defectDetecter = Q_NULLPTR; //��ȡ��
}

DetectThread::~DetectThread()
{
}

//��ʼ��ȱ�ݼ����
void DetectThread::initDefectDetecter()
{
	defectDetecter->setAdminConfig(adminConfig);
	defectDetecter->setDetectConfig(detectConfig);
	defectDetecter->setDetectParams(detectParams);
	defectDetecter->setSampleImages(cvmatSamples);
	defectDetecter->setDetectResult(detectResult);
	defectDetecter->initDetectFunc();
}


//������ȡ�߳�
void DetectThread::run()
{
	defectDetecter->detect(); //���
}

