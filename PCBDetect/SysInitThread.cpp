#include "SysInitThread.h"


SysInitThread::SysInitThread()
{
}

SysInitThread::~SysInitThread()
{
}


void SysInitThread::run()
{
	//读取参数配置文件，对config进行初始化
	emit initializeStatus_initThread(QString::fromLocal8Bit("正在获取历史参数配置 ..."));
	Sleep(1500);

	int ConfigCode = initDetectConfig();
	if (ConfigCode) { emit configError_initThread(ConfigCode); return; }

	emit initializeStatus_initThread(QString::fromLocal8Bit("历史参数配置获取结束  "));

	//其他初始化操作
	Sleep(1200);
	
	//初始化结束
	emit initializeFinished_initThread();
}


/****************** 初始化 ********************/

//设置参数结构体的指针
void SysInitThread::setDetectConfig(DetectConfig *ptr) { config = ptr; }


//对config进行初始化
int SysInitThread::initDetectConfig()
{
	QString configFilePath = QDir::currentPath() + "/.config";
	QFile configFile(configFilePath);
	if (!configFile.open(QIODevice::ReadOnly)) { //若配置文件不存在，则生成默认的配置文件
		Configurator::init(configFilePath); 
	}
	else { //从配置文件中读取参数
		Configurator configurator(&configFile);
		configurator.jsonReadValue("OutputDirPath", config->OutputDirPath);
		configurator.jsonReadValue("SampleDirPath", config->SampleDirPath);
		configurator.jsonReadValue("TemplDirPath", config->TemplDirPath);
		configurator.jsonReadValue("ImageFormat", config->ImageFormat);

		configurator.jsonReadValue("nCamera", config->nCamera);
		configurator.jsonReadValue("nPhotographing", config->nPhotographing);
		configurator.jsonReadValue("nBasicUnitInRow", config->nBasicUnitInRow);
		configurator.jsonReadValue("nBasicUnitInCol", config->nBasicUnitInCol);

		// ...
		configFile.close();

		//参数有效性判断
		if (!(QFileInfo(config->OutputDirPath).isDir())) return -1;
		if (!(QFileInfo(config->TemplDirPath).isDir())) return -2;
		if (!(QFileInfo(config->SampleDirPath).isDir())) return -3;
	}

	config->imageSize = QSize(4384, 3288); //样本图像的尺寸
	return 0;
}
