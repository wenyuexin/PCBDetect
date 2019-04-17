#pragma once

#include <QThread>
#include "Configurator.h"
#include "RuntimeLib.h"
#include "ImageConverter.h"
#include "DefectDetecter.h"


class DetectThread : public QThread
{
	Q_OBJECT

private:
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���

	pcb::CvMatArray *cvmatSamples; //������ȡ������ͼ
	pcb::DetectResult *detectResult; //�����
	DefectDetecter *defectDetecter; //�����

public:
	DetectThread();
	~DetectThread();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }

	inline void setSampleImages(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; }
	inline void setDefectDetecter(DefectDetecter* ptr) { defectDetecter = ptr; }
	void initDefectDetecter();

protected:
	void run();
};
