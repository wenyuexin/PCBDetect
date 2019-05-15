#include "DetectThread.h"

using pcb::CvMatVector;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::DetectResult;
using pcb::QPixmapVector;
using pcb::QImageVector;
using pcb::QImageArray;


DetectThread::DetectThread()
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	cvmatSamples = Q_NULLPTR; //������ȡ������ͼ
	defectDetecter = Q_NULLPTR; //��ȡ��
}

DetectThread::~DetectThread()
{
	qDebug() << "~DetectThread";
}

//��ʼ��ȱ�ݼ����
void DetectThread::initDefectDetecter()
{
	defectDetecter->setAdminConfig(adminConfig);
	defectDetecter->setUserConfig(userConfig);
	defectDetecter->setRuntimeParams(runtimeParams);
	defectDetecter->setSampleImages(cvmatSamples);
	defectDetecter->setDetectResult(detectResult);
	defectDetecter->init();
}


//������ȡ�߳�
void DetectThread::run()
{
	defectDetecter->detect(); //���
}

