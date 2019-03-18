#include "Configurator.h"

using Ui::DetectConfig;

Configurator::Configurator(QFile *file)
{
	this->configFile = file;
}

Configurator::~Configurator()
{
}

/********************** 生成默认的参数配置文件 *************************/

//生成默认的参数配置文件
void Configurator::init(QString filePath)
{
	QFileInfo config(filePath);
	if (!config.isFile()) { //没有配置文件  则创建文件 ; 生成配置文件
		QFile file(filePath);
		file.open(QIODevice::WriteOnly);
		QTextStream textStrteam(&file);
		QVariantMap pathConfig;

		QDir dir(QDir::currentPath());
		dir.cdUp(); //转到上一级目录
		QString appDirPath = dir.absolutePath(); //上一级目录的绝对路径
		pathConfig.insert("OutputDirPath", appDirPath + "/output");
		pathConfig.insert("SampleDirPath", appDirPath + "/sample");
		pathConfig.insert("TemplDirPath", appDirPath + "/template");
		pathConfig.insert("ImageFormat", ".bmp");

		pathConfig.insert("nCamera", "5");
		pathConfig.insert("nPhotographing", "8");
		pathConfig.insert("nBasicUnitInRow", "4");
		pathConfig.insert("nBasicUnitInCol", "6");
		pathConfig.insert("ImageAspectRatio_W", "4"); //样本图像的宽高比
		pathConfig.insert("ImageAspectRatio_H", "3"); //样本图像的宽高比
		pathConfig.insert("imageAspectRatio", QString::number(4.0 / 3.0, 'g', 7));

		QJsonDocument jsonDocument = QJsonDocument::fromVariant(pathConfig);
		textStrteam << jsonDocument.toJson();
		file.close();
	}
}


/************* 将某个参数的写入config文件中 ************/

//将参数写入配置文件中 - QString
bool Configurator::jsonSetValue(const QString &key, QString &value)
{
	QTextStream textStrteam(configFile);
	configFile->seek(0);
	QString val = configFile->readAll();
	QJsonParseError json_error;
	QJsonDocument confDcoument = QJsonDocument::fromJson(val.toUtf8(), &json_error);

	if (json_error.error == QJsonParseError::NoError) {
		if (!confDcoument.isNull() || !confDcoument.isEmpty()) {
			if (confDcoument.isObject()) {
				QJsonObject obj = confDcoument.object();
				obj[key] = value;
				QJsonDocument document = QJsonDocument::fromVariant(obj.toVariantMap());
				configFile->resize(0);
				textStrteam << document.toJson();
				return true;
			}
		}
		else { //文件为空
			qDebug() << "文件空";
		}
	}
	return false;
}

//将参数写入配置文件中 - int
bool Configurator::jsonSetValue(const QString &key, int &value)
{
	return jsonSetValue(key, QString::number(value));
}

//将参数写入配置文件中 - double
bool Configurator::jsonSetValue(const QString &key, double &value)
{
	return jsonSetValue(key, QString::number(value, 'g', 7));
}


/************* 从config文件中读取某个参数 ************/

//从配置文件中读取参数 - QString
bool Configurator::jsonReadValue(const QString &key, QString &value)
{
	configFile->seek(0);
	QString val = configFile->readAll();
	QJsonParseError json_error;
	QJsonDocument confDcoument = QJsonDocument::fromJson(val.toUtf8(), &json_error);
	if (json_error.error == QJsonParseError::NoError) {
		if (!confDcoument.isNull() || !confDcoument.isEmpty()) {
			if (confDcoument.isObject()) {
				QJsonObject obj = confDcoument.object();
				value = obj[key].toString();
				return true;
			}
		}
		else {
			qDebug() << "文件空";
			value = "";
		}
	}
	return false;
}

//从配置文件中读取参数 - double
bool Configurator::jsonReadValue(const QString &key, double &value)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr)) {
		value = valueStr.toDouble(); return true;
	}
	return false;
}

//从配置文件中读取参数 - int
bool Configurator::jsonReadValue(const QString &key, int &value)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr)) {
		value = valueStr.toInt(); return true;
	}
	return false;
}


/******************* 暂时没用 ********************/

//获取当前磁盘剩余空间
quint64 Configurator::getDiskFreeSpace(QString driver) 
{
	LPCWSTR lpcwstrDriver = (LPCWSTR)driver.utf16();
	ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;

	if (!GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes)) {
		qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
		return 0;
	}
	return (quint64)liTotalFreeBytes.QuadPart / 1024 / 1024 / 1024;
}

//暂时没用
bool Configurator::checkDir(QString dirpath)
{
	QDir dir(dirpath);
	dir.setSorting(QDir::Name | QDir::Time | QDir::Reversed);
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList folder_list = dir.entryInfoList();
	if (folder_list.size() < 1) {
		return false;
	}
	else {
		QString name = folder_list.at(0).absoluteFilePath();
		QDir dir(name);
		dir.setSorting(QDir::Name | QDir::Time | QDir::Reversed);
		dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
		QFileInfoList file_list = dir.entryInfoList();
		if (file_list.size() < 1) return false;

		QString name2 = file_list.at(0).absoluteFilePath();
		QFileInfo config(name2 + "/" + "outputImage/");
		if (!config.isDir()) return false; //没有配置文件 则创建文件
	}
	return true;
}


/********************* 其他 **********************/

//非阻塞延迟
void Ui::delay(unsigned long msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}