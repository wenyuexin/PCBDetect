#pragma once
#include <QThread>
#include <QDebug>
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "DetectFunc.h"
#include <vector>
#include <map>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

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

	int segThresh;//�ָ���ֵ
	bool UsingDefaultSegThresh;//�Զ��ָ��ʾ

	double scalingFactor; //�������ӣ�ֻ��
	cv::Size *scaledFullImageSize; //�������ź����ͼ�ĳߴ磬ֻ��
	cv::Size *scaledSubImageSize; //�������ź�ķ�ͼ�ĳߴ磬ֻ��

	int defectNum; //��ͼ��ȱ����
	cv::Mat markedSubImage; //��Ǻ�ķ�ͼ��ʹ���������
	std::map <cv::Point3i, cv::Mat, cmp_point3i > detailImage;//ȱ��ϸ��ͼ��ʹ���������

public:
	DetectUnit(QObject *parent = Q_NULLPTR);
	~DetectUnit();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; } //����ϵͳ����
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; } //�����û�����
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; } //�������в���
	inline void setDetectFunc(DetectFunc *ptr) { detectFunc = ptr; } //���ü�⺯����

	inline void setSubImage(cv::Mat &src) { samp = src; } //���÷�ͼ
	inline void setCurrentCol(int col) { curCol = col; } //���÷�ͼ���к�
	inline void setMaskRoi(cv::Point *bl, cv::Point *tr) { maskRoi_bl = bl; maskRoi_tr = tr; } //������ģ��������
	inline void setSegThresh(int value) { segThresh = value; }//������ֵ
	inline void setThreshFlag(bool flag) { UsingDefaultSegThresh = flag; }//������ֵ

	inline void setScalingFactor(double f) { scalingFactor = f; } //������������
	inline void setScaledFullImageSize(cv::Size *sz) { scaledFullImageSize = sz; } //���þ������ź����ͼͼ��ߴ�
	inline void setScaledSubImageSize(cv::Size *sz) { scaledSubImageSize = sz; } //���þ������ź�ķ�ͼͼ��ߴ�

	inline int getDefectNum() { defectNum = detailImage.size(); return defectNum; } //��ȡ��ͼ��ȱ����
	inline std::map<cv::Point3i, cv::Mat, cmp_point3i> getDetailImage(){ return detailImage; }//��ȡȱ����Ϣ��ϸ��ͼ
	inline void clearDetailImage() { detailImage.clear(); }
	inline cv::Mat getMarkedSubImage() { return markedSubImage; }

	inline int getcurRow() { return curRow; }
	inline int getcurCol() { return curCol; }

protected:
	void run();

public:
	cv::Mat rectBlack;


	//bool alignImages_surf_load(std::vector<cv::KeyPoint> &keypoints_1, cv::Mat& descriptors_1, cv::Mat &image_sample_gray, cv::Mat &imgReg, cv::Mat &H, cv::Mat &imMatches);
	void save(const std::string& path, cv::Mat& image_template_gray);
	void load(const std::string& path);

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
};
