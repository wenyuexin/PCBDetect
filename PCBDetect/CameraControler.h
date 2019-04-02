#pragma once

#include "Configurator.h"
#include "Camera.h"
#include <QThread>

//相机控制器
class CameraControler : public QThread
{
	Q_OBJECT

private:
	Ui::CvMatArray *cvmatSamples; //用于检测的样本图
	int *currentRow; //当前行号
	int *nCamera; //相机个数
	Camera camera; //相机

public:
	CameraControler(QThread *parent = Q_NULLPTR);
	~CameraControler();

	inline void setCvMatSamples(Ui::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setCurrentRow(int *row) { currentRow = row;  }
	inline void setNCamera(int *num) { nCamera = num; }

protected:
	void run();
	
Q_SIGNALS:
	void takePhotos_finished_camera();

};
