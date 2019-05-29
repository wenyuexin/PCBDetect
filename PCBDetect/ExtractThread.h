#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImageConverter.h"
#include "TemplateExtractor.h"
#include "ExtractFunc.h"


class ExtractThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���

	pcb::CvMatArray *cvmatSamples; //������ȡ������ͼ
	TemplateExtractor *templExtractor; //��ȡ��

public:
	ExtractThread(QObject *parent = Q_NULLPTR);
	~ExtractThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }

	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setTemplateExtractor(TemplateExtractor *ptr) { templExtractor = ptr; }
	void init();

protected:
	void run();
};
