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
	//这里调用相机拍照程序
	//拍摄结束后将结果存入 (*cvmatSamples)[*currentRow]中
	camera.init();
	camera.takePicture(*cvmatSamples, *currentRow, *nCamera);
	emit takePhotos_finished_camera();
}
