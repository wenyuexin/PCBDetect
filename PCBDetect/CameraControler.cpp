#include "CameraControler.h"

CameraControler::CameraControler(QThread *parent)
	: QThread(parent)
{
}

CameraControler::~CameraControler()
{
}

void CameraControler::run()
{
	//�������������ճ���
	//��������󽫽������ (*cvmatSamples)[*currentRow]��
	camera.init();
	camera.takePicture(*cvmatSamples, *currentRow, *nCamera);
	emit takePhotos_finished_camera();
}
