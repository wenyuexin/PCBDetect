#pragma once
#include <QThread>
#include <QDebug>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "FuncLib.h"
#include "DetectFunc.h"
#include <vector>
#include <map>

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

	double scalingFactor; //�������ӣ�ֻ��
	cv::Size *scaledFullImageSize; //�������ź����ͼ�ĳߴ磬ֻ��
	cv::Size *scaledSubImageSize; //�������ź�ķ�ͼ�ĳߴ磬ֻ��

	int defectNum; //��ͼ��ȱ����
	cv::Mat markedSubImage; //��Ǻ�ķ�ͼ��ʹ���������
	std::map<cv::Point3i, cv::Mat> detailImage;//ȱ��ϸ��ͼ��ʹ���������

public:
	DetectUnit(QObject *parent = Q_NULLPTR);
	~DetectUnit();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //����ϵͳ����
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //�����û�����
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //�������в���

	inline void setDetectFunc(DetectFunc *ptr) { detectFunc = ptr; } //���ü�⺯����
	inline void setSubImage(cv::Mat &src) { samp = src; } //���÷�ͼ

	inline void setCurrentRow(int row) { curRow = row; }
	inline void setCurrentCol(int col) { curCol = col; }
	inline void setMaskRoi(cv::Point *bl, cv::Point *tr) { maskRoi_bl = bl; maskRoi_bl = tr; }

	inline void setScalingFactor(double f) { scalingFactor = f; } //������������
	inline void setScaledFullImageSize(cv::Size *sz) { scaledFullImageSize = sz; } //���þ������ź����ͼͼ��ߴ�
	inline void setScaledSubImageSize(cv::Size *sz) { scaledSubImageSize = sz; } //���þ������ź�ķ�ͼͼ��ߴ�

	inline int getDefectNum() { defectNum = detailImage.size(); return defectNum; } //��ȡ��ͼ��ȱ����
	inline std::map<cv::Point3i, cv::Mat> getDetailImage(){ return detailImage; }//��ȡȱ����Ϣ��ϸ��ͼ
	inline cv::Mat getMarkedSubImage() { return markedSubImage; }


protected:
	void run();
};
