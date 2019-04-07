#include "Configurator.h"

using pcb::DetectConfig;
using pcb::AdminConfig;
using pcb::Configurator;
using pcb::DetectParams;


/****************************************************/
/*                   DetectConfig                   */
/****************************************************/

//加载默认值
void DetectConfig::loadDefaultValue() 
{
	QDir dir(QDir::currentPath());
	dir.cdUp(); //转到上一级目录
	QString appDirPath = dir.absolutePath(); //上一级目录的绝对路径

	this->errorCode = Uncheck; //错误代码
	this->SampleDirPath = appDirPath + "/sample"; //样本文件存储路径
	this->TemplDirPath = appDirPath + "/template";//模板文件的存储路径
	this->OutputDirPath = appDirPath + "/output";//检测结果存储路径
	this->ImageFormat = ".bmp"; //图像后缀
	this->nCamera = 5; //相机个数
	this->nPhotographing = 4; //拍照次数
	this->nBasicUnitInRow = 4; //每一行中的基本单元数
	this->nBasicUnitInCol = 6; //每一列中的基本单元数
	this->ImageAspectRatio_W = 4; //宽高比中的宽
	this->ImageAspectRatio_H = 3; //宽高比中的高
	this->ImageAspectRatio = 4.0 / 3.0; //样本图像的宽高比
}

//参数有效性检查
DetectConfig::ErrorCode DetectConfig::checkValidity(ConfigIndex index)
{
	if (this->errorCode == ValidConfig)
		return this->errorCode;

	ErrorCode code = Uncheck;
	switch (index)
	{
	case pcb::DetectConfig::Index_All:
	case pcb::DetectConfig::Index_SampleDirPath:
		if (!QFileInfo(SampleDirPath).isDir())
			code = Invalid_SampleDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_TemplDirPath:
		if (!QFileInfo(TemplDirPath).isDir())
			code = Invalid_TemplDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_OutputDirPath:
		if (!QFileInfo(OutputDirPath).isDir())
			code = Invalid_OutputDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ImageFormat:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nCamera:
		if (nCamera < 1) code = Invalid_nCamera;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nPhotographing:
		if (nPhotographing < 1)
			code = Invalid_nPhotographing;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nBasicUnitInRow:
		if (nBasicUnitInRow < 1)
			code = Invalid_nBasicUnitInRow;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nBasicUnitInCol:
		if (nBasicUnitInCol < 1)
			code = Invalid_nBasicUnitInCol;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ImageAspectRatio_W:
		if (ImageAspectRatio_W < 1)
			code = Invalid_ImageAspectRatio_W;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ImageAspectRatio_H:
		if (ImageAspectRatio_H < 1)
			code = Invalid_ImageAspectRatio_H;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ImageAspectRatio:
		if (code != Uncheck || index != Index_All) break;
	}

	if (code == Uncheck) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) this->errorCode = code;
	return code;
}

//判断参数是否有效
bool DetectConfig::isValid() {
	if (this->errorCode == DetectConfig::Uncheck)
		checkValidity(Index_All);
	return this->errorCode == ValidConfig;
}

//将错误代码转为参数索引
DetectConfig::ConfigIndex DetectConfig::convertCodeToIndex(ErrorCode code) 
{
	switch (code)
	{
	case pcb::DetectConfig::ValidConfig:
		return Index_None;
	case pcb::DetectConfig::Invalid_SampleDirPath:
		return Index_SampleDirPath;
	case pcb::DetectConfig::Invalid_TemplDirPath:
		return Index_TemplDirPath;
	case pcb::DetectConfig::Invalid_OutputDirPath:
		return Index_OutputDirPath;
	case pcb::DetectConfig::Invalid_ImageFormat:
		return Index_ImageFormat;
	case pcb::DetectConfig::Invalid_nCamera:
		return Index_nCamera;
	case pcb::DetectConfig::Invalid_nPhotographing:
		return Index_nPhotographing;
	case pcb::DetectConfig::Invalid_nBasicUnitInRow:
		return Index_nBasicUnitInRow;
	case pcb::DetectConfig::Invalid_nBasicUnitInCol:
		return Index_nBasicUnitInCol;
	case pcb::DetectConfig::Invalid_ImageAspectRatio_W:
		return Index_ImageAspectRatio_W;
	case pcb::DetectConfig::Invalid_ImageAspectRatio_H:
		return Index_ImageAspectRatio_H;
	case pcb::DetectConfig::Invalid_ImageAspectRatio:
		return Index_ImageAspectRatio;
	}
	return Index_None;
}

//参数报错
bool DetectConfig::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	if (tempCode == ConfigFileMissing) {
		QString message = QString::fromLocal8Bit(".user.config文件丢失，已生成默认文件!    \n")
			+ QString::fromLocal8Bit("请在参数设置界面确认参数是否有效 ...   \n");
		QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
			message + "Config: User: ErrorCode: " + QString::number(tempCode),
			QString::fromLocal8Bit("确定"));
		return true;
	}

	switch (tempCode)
	{
	case pcb::DetectConfig::Invalid_SampleDirPath:
		valueName = QString::fromLocal8Bit("样本路径"); break;
	case pcb::DetectConfig::Invalid_TemplDirPath:
		valueName = QString::fromLocal8Bit("模板路径"); break;
	case pcb::DetectConfig::Invalid_OutputDirPath:
		valueName = QString::fromLocal8Bit("输出路径"); break;
	case pcb::DetectConfig::Invalid_ImageFormat:
		valueName = QString::fromLocal8Bit("图像格式"); break;
	case pcb::DetectConfig::Invalid_nCamera:
		valueName = QString::fromLocal8Bit("相机个数"); break;
	case pcb::DetectConfig::Invalid_nPhotographing:
		valueName = QString::fromLocal8Bit("拍摄次数"); break;
	case pcb::DetectConfig::Invalid_nBasicUnitInRow:
	case pcb::DetectConfig::Invalid_nBasicUnitInCol:
		valueName = QString::fromLocal8Bit("基本单元数"); break;
	case pcb::DetectConfig::Invalid_ImageAspectRatio_W:
	case pcb::DetectConfig::Invalid_ImageAspectRatio_H:
	case pcb::DetectConfig::Invalid_ImageAspectRatio:
		valueName = QString::fromLocal8Bit("图像宽高比"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
		QString::fromLocal8Bit("参数无效，请在参数设置界面重新设置") + valueName + "!        \n" +
		"Config: User: ErrorCode: " + QString::number(tempCode),
		QString::fromLocal8Bit("确定"));
	return true;
}

//计算宽高比
DetectConfig::ErrorCode DetectConfig::calcImageAspectRatio() {
	ErrorCode code = checkValidity(Index_ImageAspectRatio_W);
	if (code != ValidValue) return this->errorCode = code;
	code = checkValidity(Index_ImageAspectRatio_H);
	if (code != ValidValue) return this->errorCode = code;
	ImageAspectRatio = 1.0 * ImageAspectRatio_W / ImageAspectRatio_H;
	return ValidValue;
}

//不相等判断
DetectConfig::ConfigIndex DetectConfig::unequals(DetectConfig &other) {
	if (this->SampleDirPath != other.SampleDirPath) return Index_SampleDirPath;
	if (this->TemplDirPath != other.TemplDirPath) return Index_TemplDirPath;
	if (this->OutputDirPath != other.OutputDirPath) return Index_OutputDirPath;
	if (this->ImageFormat != other.ImageFormat) return Index_ImageFormat;
	if (this->nCamera != other.nCamera) return Index_nCamera;
	if (this->nPhotographing != other.nPhotographing) return Index_nPhotographing;
	if (this->nBasicUnitInRow != other.nBasicUnitInRow) return Index_nBasicUnitInRow;
	if (this->nBasicUnitInCol != other.nBasicUnitInCol) return Index_nBasicUnitInCol;
	if (this->ImageAspectRatio_W != other.ImageAspectRatio_W) return Index_ImageAspectRatio_W;
	if (this->ImageAspectRatio_H != other.ImageAspectRatio_H) return Index_ImageAspectRatio_H;
	return Index_None;
}

//功能：获取系统重置代码
//输出：返回一个重置代码，代码不同的二进制位，代表不同的重置操作
//      0b1234 第1位置位，则表示重置模板提取模块
//             第2位置位，则表示重置检测模块
//             第3位置位，则表示重置运动结构
//             第4位置位，则表示重置相机
int DetectConfig::getSystemResetCode(DetectConfig &newConfig) {
	int resetCode = 0b0000;
	if (nCamera != newConfig.nCamera) resetCode |= 0x1100;
	if (nPhotographing != newConfig.nPhotographing) resetCode |= 0x1100;
	if (ImageAspectRatio_W != newConfig.ImageAspectRatio_W || ImageAspectRatio_H != newConfig.ImageAspectRatio_H) {
		if (abs(ImageAspectRatio - newConfig.ImageAspectRatio) < 1E-5) resetCode |= 0x1100;
	}
	if (nCamera > newConfig.nCamera) resetCode |= 0x0001;
	return resetCode;
}

//拷贝结构体
void DetectConfig::copyTo(DetectConfig *dst) 
{
	dst->errorCode = this->errorCode; //参数有效性
	dst->SampleDirPath = this->SampleDirPath; //样本文件存储路径
	dst->TemplDirPath = this->TemplDirPath;//模板文件的存储路径
	dst->OutputDirPath = this->OutputDirPath;//检测结果存储路径
	dst->ImageFormat = this->ImageFormat; //图像后缀
	dst->nCamera = this->nCamera; //相机个数
	dst->nPhotographing = this->nPhotographing; //拍照次数
	dst->nBasicUnitInRow = this->nBasicUnitInRow; //每一行中的基本单元数
	dst->nBasicUnitInCol = this->nBasicUnitInCol; //每一列中的基本单元数
	dst->ImageAspectRatio_W = this->ImageAspectRatio_W; //宽高比中的宽
	dst->ImageAspectRatio_H = this->ImageAspectRatio_H; //宽高比中的高
	dst->ImageAspectRatio = this->ImageAspectRatio; //样本图像的宽高比
}



/****************************************************/
/*                   AdminConfig                    */
/****************************************************/

//加载默认参数
void AdminConfig::loadDefaultValue()
{
	this->errorCode = Uncheck; //错误代码
	this->MaxMotionStroke = 80*5; //机械结构的最大运动行程
	this->MaxCameraNum = 5; //可用相机的总数
	this->ImageResolutionRatio = 40; //图像分辨率
	this->ImageOverlappingRate = 0.05; //分图重叠率
}

//参数有效性检查
AdminConfig::ErrorCode AdminConfig::checkValidity(AdminConfig::ConfigIndex index)
{
	if (errorCode == ValidConfig) return errorCode;

	AdminConfig::ErrorCode code = Uncheck;
	switch (index)
	{
	case Index_All:
	case Index_MaxMotionStroke:
		if (MaxMotionStroke <= 0) {
			errorCode = Invalid_MaxMotionStroke;
		}
		if (code != Uncheck || index != Index_All) break;
	case Index_MaxCameraNum:
		if (MaxCameraNum <= 0) {
			errorCode = Invalid_MaxCameraNum;
		}
		if (code != Uncheck || index != Index_All) break;
	case Index_ImageResolutionRatio:
		if (ImageResolutionRatio < 0) {
			errorCode = Invalid_ImageResolutionRatio;
		}
		if (code != Uncheck || index != Index_All) break;
	case Index_ImageOverlappingRate:
		if (ImageOverlappingRate <= 0 || ImageOverlappingRate >= 1) {
			errorCode = Invalid_ImageOverlappingRate;
		}
	}

	if (code == Uncheck) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) errorCode = code;
	return code;
}

//判断参数是否有效
bool AdminConfig::isValid() 
{
	if (errorCode == AdminConfig::Uncheck)
		checkValidity(Index_All);
	return errorCode == ValidConfig;
}

//错误代码转参数索引
AdminConfig::ConfigIndex AdminConfig::convertCodeToIndex(ErrorCode code)
{
	switch (code)
	{
		case pcb::AdminConfig::ValidConfig:
			return Index_None;
		case  pcb::AdminConfig::Invalid_MaxMotionStroke,
			return Index_MaxMotionStroke;
		case pcb::AdminConfig::Invalid_MaxCameraNum,
			return Index_MaxCameraNum;
		case pcb::AdminConfig::Invalid_ImageResolutionRatio:
			return Index_ImageResolutionRatio;
		case pcb::AdminConfig::Invalid_ImageOverlappingRate:
			return Index_ImageOverlappingRate;
	}
	return Index_None;
}

//弹窗报错
bool AdminConfig::showMessageBox(QWidget *parent, AdminConfig::ErrorCode code)
{
	AdminConfig::ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	if (tempCode == AdminConfig::ConfigFileMissing) {
		QString message = QString::fromLocal8Bit(".admin.config文件丢失，已生成默认文件!  \n")
			+ QString::fromLocal8Bit("请联系管理员确认参数是否有效 ...  \n");
		QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
			message + "Config: Admin: ErrorCode: " + QString::number(tempCode),
			QString::fromLocal8Bit("确定"));
		return true;
	}

	switch (code)
	{
	case pcb::AdminConfig::Invalid_MaxMotionStroke:
		valueName = QString::fromLocal8Bit("机械结构最大行程"); break;
	case pcb::AdminConfig::Invalid_MaxCameraNum:
		valueName = QString::fromLocal8Bit("可用相机总数"); break;
	case pcb::AdminConfig::Invalid_ImageResolutionRatio:
		valueName = QString::fromLocal8Bit("图像分辨率"); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate:
		valueName = QString::fromLocal8Bit("分图重叠率"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("警告"),
		QString::fromLocal8Bit("参数无效，请联系管理员重新设置") + valueName + "!        \n" +
		"Config: Admin: ErrorCode: " + QString::number(tempCode),
		QString::fromLocal8Bit("确定"));
	return true;
}

//不相等判断
AdminConfig::ConfigIndex AdminConfig::unequals(AdminConfig &other)
{
	if (this->MaxMotionStroke != other.MaxMotionStroke) return Index_MaxMotionStroke;
	if (this->MaxCameraNum != other.MaxCameraNum) return Index_MaxCameraNum;
	if (this->ImageResolutionRatio != other.ImageResolutionRatio) return Index_ImageResolutionRatio;
	if (this->ImageOverlappingRate != other.ImageOverlappingRate) return Index_ImageOverlappingRate;
	return Index_None;
}

//功能：获取系统重置代码
//输出：返回一个重置代码，代码不同的二进制位，代表不同的重置操作
//      0b1234 第1位置位，则表示重置模板提取模块
//             第2位置位，则表示重置检测模块
//             第3位置位，则表示重置运动结构
//             第4位置位，则表示重置相机
int AdminConfig::getSystemResetCode(AdminConfig &newConfig)
{
	int resetCode = 0b0000;

	//重置运动结构模块
	if (this->MaxMotionStroke != newConfig.MaxMotionStroke) resetCode |= 0x0010;
	if (this->ImageResolutionRatio != newConfig.ImageResolutionRatio) resetCode |= 0x0010;
	if (this->ImageOverlappingRate != newConfig.ImageOverlappingRate) resetCode |= 0x0010;
	//重置相机模块
	if (this->MaxCameraNum != newConfig.MaxCameraNum) resetCode |= 0x0001;
	return resetCode;
}

//拷贝结构体
void AdminConfig::copyTo(AdminConfig *dst)
{
	dst->errorCode = this->errorCode;
	dst->MaxMotionStroke = this->MaxMotionStroke;
	dst->MaxCameraNum = this->MaxCameraNum;
	dst->ImageResolutionRatio = this->ImageResolutionRatio;
	dst->ImageOverlappingRate = this->ImageOverlappingRate;
}



/****************************************************/
/*                    Configurator                  */
/****************************************************/

Configurator::Configurator(QFile *file)
{
	this->configFile = file;
	updateKeys();
}

Configurator::~Configurator()
{
}

/************** 生成默认的参数配置文件 **************/

//生成默认的参数配置文件
void Configurator::createConfigFile(QString filePath)
{
	QFileInfo config(filePath);
	if (!config.isFile()) { //没有配置文件  则创建文件 ; 生成配置文件
		QFile file(filePath);
		file.open(QIODevice::WriteOnly);
		QTextStream textStrteam(&file);
		QVariantMap pathConfig;
		//pathConfig.insert("###", "###");
		QJsonDocument jsonDocument = QJsonDocument::fromVariant(pathConfig);
		textStrteam << jsonDocument.toJson();
		file.close();
	}
}


/********** 将某个参数的写入config文件中 ************/

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
				QString encodeKey = encrypt(key); //加密
				QString encodeValue = encrypt(value);
				obj[encodeKey] = encodeValue;
				QJsonDocument document = QJsonDocument::fromVariant(obj.toVariantMap());
				configFile->resize(0);
				textStrteam << document.toJson();
				updateKeys();//更新秘钥
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
				QString encodeKey = encrypt(key); //加密
				QString encodeValue = obj[encodeKey].toString();
				value = decrypt(encodeValue); //解密
				QByteArray();
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


/******************* 加密与解密 ********************/

//修改秘钥
void Configurator::updateKeys()
{
	QFileInfo fileInfo = QFileInfo(*configFile);

	//如果最近的修改时间为空，则使用文件的创建时间
	if (fileDateTime.isNull() || fileDateTime != fileInfo.lastModified()) {
		fileDateTime = fileInfo.lastModified();
		if (fileDateTime.isNull()) fileDateTime = fileInfo.created();
		keys[0] = fileDateTime.toString("dd").toInt() % 10;
		keys[1] = fileDateTime.toString("MM").toInt() % 10;
		keys[2] = fileDateTime.toString("yyyy").toInt() % 10;
		keys[3] = (keys[1] + keys[2] + 2019) % 10;
	}
}

//加密
QString Configurator::encrypt(QString origin) const
{
	QString encodeStr;
	int len = origin.size();
	for (int i = 0; i < len; i++) {
		encodeStr.append(origin.at(i).unicode() ^ keys[i % 4]);
	}
	return encodeStr;
}

QString Configurator::encrypt(const char* origin) const
{
	return encrypt(QString(origin));
}

//解密
QString Configurator::decrypt(QString origin) const
{
	QString encodeStr;
	int len = origin.size();
	for (int i = 0; i < len; i++) {
		encodeStr.append(origin.at(i).unicode() ^ keys[i % 4]);
	}
	return encodeStr;
}

QString Configurator::decrypt(const char* origin) const
{
	return decrypt(QString(origin));
}


/****************** 配置类的读写 ********************/

//将配置文件中的参数加载到DetectConfig中
bool Configurator::loadConfigFile(const QString &fileName, DetectConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) { //判断配置文件读写权限
		createConfigFile(configFilePath);//创建配置文件
		config->loadDefaultValue();//加载默认值
		saveConfigFile(fileName, config);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonReadValue("OutputDirPath", config->OutputDirPath);
		configurator.jsonReadValue("SampleDirPath", config->SampleDirPath);
		configurator.jsonReadValue("TemplDirPath", config->TemplDirPath);
		configurator.jsonReadValue("ImageFormat", config->ImageFormat);

		configurator.jsonReadValue("nCamera", config->nCamera);
		configurator.jsonReadValue("nPhotographing", config->nPhotographing);
		configurator.jsonReadValue("nBasicUnitInRow", config->nBasicUnitInRow);
		configurator.jsonReadValue("nBasicUnitInCol", config->nBasicUnitInCol);
		configurator.jsonReadValue("ImageAspectRatio_W", config->ImageAspectRatio_W);
		configurator.jsonReadValue("ImageAspectRatio_H", config->ImageAspectRatio_H);
		configFile.close();
	}
	return success;
}

//将 DetectConfig 中的参数保存到配置文件中
bool Configurator::saveConfigFile(const QString &fileName, DetectConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		DetectConfig defaultConfig;
		defaultConfig.loadDefaultValue();//加载默认值
		saveConfigFile(fileName, &defaultConfig);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonSetValue("SampleDirPath", config->SampleDirPath);//样本文件夹
		configurator.jsonSetValue("TemplDirPath", config->TemplDirPath);//模板文件夹
		configurator.jsonSetValue("OutputDirPath", config->OutputDirPath);//输出文件夹
		configurator.jsonSetValue("ImageFormat", config->ImageFormat);//图像格式
		configurator.jsonSetValue("nCamera", QString::number(config->nCamera)); //相机个数
		configurator.jsonSetValue("nPhotographing", QString::number(config->nPhotographing)); //拍照次数
		configurator.jsonSetValue("nBasicUnitInRow", QString::number(config->nBasicUnitInRow)); //每一行中的基本单元数
		configurator.jsonSetValue("nBasicUnitInCol", QString::number(config->nBasicUnitInCol)); //每一列中的基本单元数
		configurator.jsonSetValue("ImageAspectRatio_W", QString::number(config->ImageAspectRatio_W)); //样本图像的宽高比
		configurator.jsonSetValue("ImageAspectRatio_H", QString::number(config->ImageAspectRatio_H)); //样本图像的宽高比
		configurator.jsonSetValue("ImageAspectRatio", QString::number(config->ImageAspectRatio, 'g', 7)); //样本图像的宽高比
		configFile.close();
	}
	return success;
}

//将配置文件中的参数加载到 AdminConfig 中
bool Configurator::saveConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) { //判断配置文件读写权限
		createConfigFile(configFilePath);//创建配置文件
		config->loadDefaultValue();//加载默认值
		saveConfigFile(fileName, config);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonReadValue("MaxMotionStroke", config->MaxMotionStroke);
		configurator.jsonReadValue("MaxCameraNum", config->MaxCameraNum);
		configurator.jsonReadValue("ImageResolutionRatio", config->ImageResolutionRatio);
		configurator.jsonReadValue("ImageOverlappingRate", config->ImageOverlappingRate);
		configFile.close();
	}
	return success;
}

//将 AdminConfig 中的参数保存到配置文件中
bool Configurator::loadConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		DetectConfig defaultConfig;
		defaultConfig.loadDefaultValue();//加载默认值
		saveConfigFile(fileName, &defaultConfig);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonSetValue("MaxMotionStroke", config->MaxMotionStroke);
		configurator.jsonSetValue("MaxCameraNum", config->MaxCameraNum);
		configurator.jsonSetValue("ImageResolutionRatio", config->ImageResolutionRatio);
		configurator.jsonSetValue("ImageOverlappingRate", config->ImageOverlappingRate);
		configFile.close();
	}
	return success;
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



/****************************************************/
/*                   DetectParams                   */
/****************************************************/

//重置产品序号
void DetectParams::resetSerialNum()
{
	QString sampleModelNum = ""; //型号
	QString sampleBatchNum = ""; //批次号
	QString sampleNum = ""; //样本编号
}

//加载默认的运行参数
void DetectParams::loadDefaultValue()
{
	resetSerialNum();
	imageSize = QSize(-1, -1);
	int currentRow_detect = -1; //检测行号
	int currentRow_extract = -1; //提取行号
}



/****************************************************/
/*                   namespace pcb                  */
/****************************************************/

//非阻塞延迟
void pcb::delay(unsigned long msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//交互式文件夹路径选择
QString pcb::selectDirPath(QString windowTitle)
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(windowTitle); //设置文件保存对话框的标题
	fileDialog->setFileMode(QFileDialog::Directory); //设置文件对话框弹出的时候显示文件夹
	fileDialog->setViewMode(QFileDialog::Detail); //文件以详细的形式显示，显示文件名，大小，创建日期等信息

	QString path = "";
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) //选择路径
		path = fileDialog->selectedFiles()[0];
	delete fileDialog;
	return path;
}
