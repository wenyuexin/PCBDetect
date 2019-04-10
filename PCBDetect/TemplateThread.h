#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImageConverter.h"
#include "TemplateExtractor.h"
#include "TemplFunc.h"


class TemplateThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���

	pcb::CvMatArray *cvmatSamples; //������ȡ������ͼ
	TemplateExtractor *templExtractor; //��ȡ��

public:
	TemplateThread(QObject *parent = Q_NULLPTR);
	~TemplateThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setTemplateExtractor(TemplateExtractor *ptr) { templExtractor = ptr; }
	void initTemplateExtractor();

protected:
	void run();
};
