#pragma once

#include <QThread>
#include "Configurator.h"
#include "ImageConverter.h"
#include "DetectCore.h"


class DetectThread :
	public QThread
{
	Q_OBJECT

private:
	DetectCore *detectCore; //检测核心

	int *currentRow; //当前正在检测的样本行的行号
	Ui::DetectParams *params;
	Ui::QImageArray *samples; //正在检测的一行样本
	std::vector<ImageConverter *> threads; //格式转换线程
	int nThreads = 10; //默认相机个数不超过10

public:
	DetectThread();
	~DetectThread();

	inline void setDetectCore(DetectCore* ptr = Q_NULLPTR) { detectCore = ptr; }
	inline void setSampleImages(Ui::QImageArray* ptr = Q_NULLPTR) { samples = ptr; }
	inline void setDetectParams(Ui::DetectParams* ptr = Q_NULLPTR) { params = ptr; }

private:
	void initImageConvertThreads();
	void deleteImageConvertThreads();
	void convertQImageToCvMat(Ui::QImageVector &src, Ui::CvMatVector &dst);

protected:
	void run();
};
