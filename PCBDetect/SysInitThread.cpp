#include "SysInitThread.h"


SysInitThread::SysInitThread()
{
	bootStatus = 0; //启动状态
}

SysInitThread::~SysInitThread()
{
}


/***************** 启动线程 *****************/

void SysInitThread::run()
{
	switch (bootStatus)
	{
	case 0: 
	case 1: //读取参数配置文件，对config进行初始化
		if (!initDetectConfig()) { bootStatus = 1;  return; }
	case 2: //其他初始化操作
		Ui::delay(1000);
	default:
		break;
	}
	
	//初始化结束
	emit sysInitFinished_initThread();
}


/****************** 初始化 ********************/

//对config进行初始化
bool SysInitThread::initDetectConfig()
{
	emit sysInitStatus_initThread(QString::fromLocal8Bit("正在获取历史参数配置 ..."));
	Ui::delay(1200);

	QString configFilePath = QDir::currentPath() + "/.config";
	QFile configFile(configFilePath);
	if (!configFile.open(QIODevice::ReadOnly)) { //若配置文件不存在，则生成默认的配置文件
		Configurator::init(configFilePath); 
		emit configError_initThread(Ui::ConfigFileMissing); return false;
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
		configurator.jsonReadValue("imageAspectRatio", config->imageAspectRatio);

		configFile.close();

		//参数有效性判断
		if (!(QFileInfo(config->OutputDirPath).isDir())) { 
			emit configError_initThread(Ui::Invalid_SampleDirPath); return false; 
		}
		if (!(QFileInfo(config->TemplDirPath).isDir())) { 
			emit configError_initThread(Ui::Invalid_TemplDirPath); return false;
		}
		if (!(QFileInfo(config->SampleDirPath).isDir())) { 
			emit configError_initThread(Ui::Invalid_OutputDirPath); return false; 
		}
	}

	emit sysInitStatus_initThread(QString::fromLocal8Bit("历史参数配置获取结束  "));
	return true;
}
