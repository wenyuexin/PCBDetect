#pragma once
#include <QObject>
#include "opencv2/opencv.hpp"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ExtractFunc.h"
#include <QStringList>


//��׼ģ����ȡ��
class TemplateExtractor : public QObject
{
	Q_OBJECT

public:
	//��ȡ״̬
	enum ExtractState {
		InitialState,
		Start,
		Finished,
		Error
	};

	//�������
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x800,
		Default = 0x8FF
	};
	
private:
	pcb::AdminConfig *adminConfig;
	pcb::UserConfig *userConfig;
	pcb::RuntimeParams *runtimeParams;
	pcb::CvMatArray *cvmatSamples;
	ExtractFunc *templFunc;
	ExtractState extractState;

	double scalingFactor; //��������
	cv::Size originalFullImageSize; //��ͼͼ���ԭʼ�ߴ�
	cv::Size scaledFullImageSize; //�������ź����ͼ�ĳߴ�
	cv::Size originalSubImageSize; //��ͼͼ���ԭʼ�ߴ�
	cv::Size scaledSubImageSize; //�������ź�ķ�ͼ�ĳߴ�

public:
	TemplateExtractor(QObject *parent = Q_NULLPTR);
	~TemplateExtractor();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }

	void extract();
	
private:
	void initExtractFunc();
	void makeCurrentTemplDir(std::vector<QString>&);

Q_SIGNALS:
	void extractState_extractor(int);

};
