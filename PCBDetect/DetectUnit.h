#pragma once
#include <QThread>
#include <QDebug>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "DetectFunc.h"


//���ڼ�ⵥ����ͼ���߳�
class DetectUnit : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	DetectFunc *detectFunc; //��⺯����

	cv::Mat samp; //���ڼ��ķ�ͼ
	int curRow; //��ͼ���ڵ��У�ֻ��
	int curCol; //��ͼ���ڵ��У�ֻ��
	cv::Point *maskRoi_bl; //��ģ�������½����ֻ꣬��
	cv::Point *maskRoi_tr; //��ģ�������Ͻ����ֻ꣬��

	int defectNum; //��ͼ��ȱ����
	cv::Mat markedSubImage; //��Ǻ�ķ�ͼ

public:
	DetectUnit(QObject *parent = Q_NULLPTR);
	~DetectUnit();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //����ϵͳ����
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //�����û�����
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //�������в���

	inline void setDetectFunc(DetectFunc *ptr) { detectFunc = ptr; } //���ü�⺯����
	inline void setSubImage(cv::Mat &src) { samp = src; } //���÷�ͼ

	inline void setMaskRoi(cv::Point *bl, cv::Point *tr) { maskRoi_bl = bl; maskRoi_bl = tr; }

	inline int getDefectNum() { return defectNum; } //��ȡ��ͼ��ȱ����
	inline cv::Mat getMarkedSubImage() { return markedSubImage; }


protected:
	void run();
};
