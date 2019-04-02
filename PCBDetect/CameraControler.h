#pragma once

#include "Configurator.h"
#include "Camera.h"
#include <QThread>

//���������
class CameraControler : public QThread
{
	Q_OBJECT

private:
	Ui::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	int *currentRow; //��ǰ�к�
	int *nCamera; //�������
	Camera camera; //���

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
