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
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数

	pcb::CvMatArray *cvmatSamples; //用于提取的样本图
	pcb::DetectResult *detectResult; //检测结果
	DefectDetecter *defectDetecter; //检测器

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
