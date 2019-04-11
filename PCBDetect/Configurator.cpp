#include "Configurator.h"

using pcb::DetectConfig;
using pcb::AdminConfig;
using pcb::Configurator;


/****************************************************/
/*                   AdminConfig                    */
/****************************************************/

AdminConfig::AdminConfig()
{
	MaxMotionStroke = -1; //机械结构的最大运动行程
	MaxCameraNum = -1; //可用相机总数
	PixelsNumPerUnitLength = -1; //单位长度的像素 pix/mm
	ImageOverlappingRate = -1; //分图重叠率
	ImageSize_W = -1; //分图宽度
	ImageSize_H = -1; //分图高度
	ImageAspectRatio = -1; //图像宽高比
}

//加载默认参数
void AdminConfig::loadDefaultValue()
{
	this->errorCode = Uncheck; //错误代码
	this->MaxMotionStroke = 80*5; //机械结构的最大运动行程
	this->MaxCameraNum = 5; //可用相机的总数
	this->PixelsNumPerUnitLength = 40; //单位长度内的像素个数
	this->ImageOverlappingRate = 0.05; //分图重叠率
	this->ImageSize_W = 4384; //宽高比中的宽
	this->ImageSize_H = 3288; //宽高比中的高
	this->ImageAspectRatio = 1.0 * ImageSize_W / ImageSize_H; //样本图像的宽高比
}

//参数有效性检查
AdminConfig::ErrorCode AdminConfig::checkValidity(AdminConfig::ConfigIndex index)
{
	if (errorCode == ValidConfig) 
		return this->errorCode;

	AdminConfig::ErrorCode code = Uncheck;
	switch (index)
	{
	case pcb::AdminConfig::Index_All:
	case pcb::AdminConfig::Index_MaxMotionStroke: //机械结构的最大运动行程
		if (MaxMotionStroke <= 0) 
			code = Invalid_MaxMotionStroke;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_MaxCameraNum: //可用相机总数
		if (MaxCameraNum <= 0) 
			code = Invalid_MaxCameraNum;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength: //单位长度的像素
		if (PixelsNumPerUnitLength <= 0) 
			code = Invalid_PixelsNumPerUnitLength;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageOverlappingRate: //分图重叠率
		if (ImageOverlappingRate <= 0 || ImageOverlappingRate >= 1) 
			code = Invalid_ImageOverlappingRate;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_W: //分图宽度
		if (ImageSize_W <= 0)
			code = Invalid_ImageSize_W;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_H: //分图高度
		if (ImageSize_H <= 0 || ImageSize_H <= PixelsNumPerUnitLength)
			code = Invalid_ImageSize_H;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageAspectRatio: //图像宽高比
		if (code != Uncheck || index != Index_All) break;
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
		case pcb::AdminConfig::Invalid_MaxMotionStroke:
			return Index_MaxMotionStroke;
		case pcb::AdminConfig::Invalid_MaxCameraNum:
			return Index_MaxCameraNum;
		case pcb::AdminConfig::Invalid_PixelsNumPerUnitLength:
			return Index_PixelsNumPerUnitLength;
		case pcb::AdminConfig::Invalid_ImageOverlappingRate:
			return Index_ImageOverlappingRate;
		case pcb::AdminConfig::Invalid_ImageSize_W:
			return Index_ImageSize_W;
		case pcb::AdminConfig::Invalid_ImageSize_H:
			return Index_ImageSize_H;
		case pcb::AdminConfig::Invalid_ImageAspectRatio:
			return Index_ImageAspectRatio;
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
		QString message = pcb::chinese(".admin.config文件丢失，已生成默认文件!  \n")
			+ pcb::chinese("请联系管理员确认参数是否有效 ...  \n");
		QMessageBox::warning(parent, pcb::chinese("警告"),
			message + "Config: Admin: ErrorCode: " + QString::number(tempCode),
			pcb::chinese("确定"));
		return true;
	}

	switch (tempCode)
	{
	case pcb::AdminConfig::Invalid_MaxMotionStroke:
		valueName = pcb::chinese("\"机械结构最大行程\""); break;
	case pcb::AdminConfig::Invalid_MaxCameraNum:
		valueName = pcb::chinese("\"可用相机总数\""); break;
	case pcb::AdminConfig::Invalid_PixelsNumPerUnitLength:
		valueName = pcb::chinese("\"每毫米像素数\""); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate:
		valueName = pcb::chinese("\"分图重叠率\""); break;
	case pcb::AdminConfig::Invalid_ImageSize_W:
	case pcb::AdminConfig::Invalid_ImageSize_H:
	case pcb::AdminConfig::Invalid_ImageAspectRatio:
		valueName = pcb::chinese("\"分图尺寸\""); break;
	default:
		valueName = "\"\""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("系统参数无效，请联系管理员重新设置") + valueName + "!        \n" +
		"Config: Admin: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}

//计算宽高比
AdminConfig::ErrorCode AdminConfig::calcImageAspectRatio() 
{
	ErrorCode code = checkValidity(Index_ImageSize_W);
	if (code != ValidValue) return code;
	code = checkValidity(Index_ImageSize_H);
	if (code != ValidValue) return code;
	ImageAspectRatio = 1.0 * ImageSize_W / ImageSize_H;
	return ValidValue;
}

//不相等判断
AdminConfig::ConfigIndex AdminConfig::unequals(AdminConfig &other)
{
	if (this->MaxMotionStroke != other.MaxMotionStroke) return Index_MaxMotionStroke;
	if (this->MaxCameraNum != other.MaxCameraNum) return Index_MaxCameraNum;
	if (this->PixelsNumPerUnitLength != other.PixelsNumPerUnitLength) return Index_PixelsNumPerUnitLength;
	if (this->ImageOverlappingRate != other.ImageOverlappingRate) return Index_ImageOverlappingRate;
	if (this->ImageSize_W != other.ImageSize_W) return Index_ImageSize_W;
	if (this->ImageSize_H != other.ImageSize_H) return Index_ImageSize_H;
	return Index_None;
}

//功能：获取系统重置代码
int AdminConfig::getSystemResetCode(AdminConfig &newConfig)
{
	int resetCode = 0b000000000;//重置代码

	//重置模板提取、检测界面
	if (ImageSize_W != newConfig.ImageSize_W || ImageSize_H != newConfig.ImageSize_H) {
		if (abs(ImageAspectRatio - newConfig.ImageAspectRatio) > 1E-6) 
			resetCode |= 0x000000110;
	}
	//重置运动结构模块
	if (this->MaxMotionStroke != newConfig.MaxMotionStroke) resetCode |= 0x000100000;
	//重置相机模块
	if (this->MaxCameraNum != newConfig.MaxCameraNum) resetCode |= 0x000010000;

	return resetCode;
}

//拷贝结构体
void AdminConfig::copyTo(AdminConfig *dst)
{
	dst->errorCode = this->errorCode;
	dst->MaxMotionStroke = this->MaxMotionStroke;
	dst->MaxCameraNum = this->MaxCameraNum;
	dst->PixelsNumPerUnitLength = this->PixelsNumPerUnitLength;
	dst->ImageOverlappingRate = this->ImageOverlappingRate;
	dst->ImageSize_W = this->ImageSize_W;
	dst->ImageSize_H = this->ImageSize_H;
	dst->ImageAspectRatio = this->ImageAspectRatio;
}



/****************************************************/
/*                   DetectConfig                   */
/****************************************************/

DetectConfig::DetectConfig()
{
	SampleDirPath = "";//样本文件存储路径
	TemplDirPath = ""; //模板文件的存储路径
	OutputDirPath = "";//检测结果存储路径
	ImageFormat = ""; //图像后缀
	ActualProductSize_W = -1;//产品实际宽度,单位mm
	ActualProductSize_H = -1;//产品实际高度,单位mm
	nBasicUnitInRow = -1; //每一行中的基本单元数
	nBasicUnitInCol = -1; //每一列中的基本单元数
}

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
	this->ActualProductSize_W = 500;//产品实际宽度
	this->ActualProductSize_H = 600;//产品实际高度
	this->nBasicUnitInRow = 4; //每一行中的基本单元数
	this->nBasicUnitInCol = 6; //每一列中的基本单元数
}

//参数有效性检查
DetectConfig::ErrorCode DetectConfig::checkValidity(ConfigIndex index, AdminConfig *adminConfig)
{
	if (this->errorCode == ValidConfig)
		return this->errorCode;

	ErrorCode code = Uncheck;
	switch (index)
	{
	case pcb::DetectConfig::Index_All:
	case pcb::DetectConfig::Index_SampleDirPath: //样本路径
		if (SampleDirPath == "" || !QFileInfo(SampleDirPath).isDir())
			code = Invalid_SampleDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_TemplDirPath: //模板路径
		if (TemplDirPath == "" || !QFileInfo(TemplDirPath).isDir())
			code = Invalid_TemplDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_OutputDirPath: //输出路径
		if (OutputDirPath == "" || !QFileInfo(OutputDirPath).isDir())
			code = Invalid_OutputDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ImageFormat: //图像格式
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ActualProductSize_W: //产品实际宽度
		if (ActualProductSize_W < 1)
			code = Invalid_ActualProductSize_W;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ActualProductSize_H: //产品实际高度
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectConfig: checkValidity: adminConfig is null !";
		if (ActualProductSize_H < 1 || 
			ActualProductSize_H > adminConfig->MaxMotionStroke)
			code = Invalid_ActualProductSize_H;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nBasicUnitInRow: //每一行中的基本单元数
		if (nBasicUnitInRow < 1)
			code = Invalid_nBasicUnitInRow;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nBasicUnitInCol: //每一列中的基本单元数
		if (nBasicUnitInCol < 1)
			code = Invalid_nBasicUnitInCol;
		if (code != Uncheck || index != Index_All) break;
	}

	if (code == Uncheck) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) this->errorCode = code;
	return code;
}

//判断参数是否有效
bool DetectConfig::isValid(AdminConfig *adminConfig) {
	if (this->errorCode == DetectConfig::Uncheck)
		checkValidity(Index_All, adminConfig);
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
	case pcb::DetectConfig::Invalid_ActualProductSize_W:
		return Index_ActualProductSize_W;
	case pcb::DetectConfig::Invalid_ActualProductSize_H:
		return Index_ActualProductSize_H;
	case pcb::DetectConfig::Invalid_nBasicUnitInRow:
		return Index_nBasicUnitInRow;
	case pcb::DetectConfig::Invalid_nBasicUnitInCol:
		return Index_nBasicUnitInCol;
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
		QString message = pcb::chinese(".user.config文件丢失，已生成默认文件!    \n")
			+ pcb::chinese("请在参数设置界面确认参数是否有效 ...   \n");
		QMessageBox::warning(parent, pcb::chinese("警告"),
			message + "Config: User: ErrorCode: " + QString::number(tempCode),
			pcb::chinese("确定"));
		return true;
	}

	switch (tempCode)
	{
	case pcb::DetectConfig::Invalid_SampleDirPath:
		valueName = pcb::chinese("\"样本路径\""); break;
	case pcb::DetectConfig::Invalid_TemplDirPath:
		valueName = pcb::chinese("\"模板路径\""); break;
	case pcb::DetectConfig::Invalid_OutputDirPath:
		valueName = pcb::chinese("\"输出路径\""); break;
	case pcb::DetectConfig::Invalid_ImageFormat:
		valueName = pcb::chinese("\"图像格式\""); break;
	case pcb::DetectConfig::Invalid_ActualProductSize_W:
	case pcb::DetectConfig::Invalid_ActualProductSize_H:
		valueName = pcb::chinese("\"产品实际尺寸\""); break;
	case pcb::DetectConfig::Invalid_nBasicUnitInRow:
	case pcb::DetectConfig::Invalid_nBasicUnitInCol:
		valueName = pcb::chinese("\"基本单元数\""); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("用户参数无效，请在参数设置界面重新设置") + valueName + "!        \n" +
		"Config: User: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}

//不相等判断
DetectConfig::ConfigIndex DetectConfig::unequals(DetectConfig &other) {
	if (this->SampleDirPath != other.SampleDirPath) return Index_SampleDirPath;
	if (this->TemplDirPath != other.TemplDirPath) return Index_TemplDirPath;
	if (this->OutputDirPath != other.OutputDirPath) return Index_OutputDirPath;
	if (this->ImageFormat != other.ImageFormat) return Index_ImageFormat;
	if (this->ActualProductSize_W != other.ActualProductSize_W) return Index_ActualProductSize_W;
	if (this->ActualProductSize_H != other.ActualProductSize_H) return Index_ActualProductSize_H;
	if (this->nBasicUnitInRow != other.nBasicUnitInRow) return Index_nBasicUnitInRow;
	if (this->nBasicUnitInCol != other.nBasicUnitInCol) return Index_nBasicUnitInCol;
	return Index_None;
}

//功能：获取系统重置代码
int DetectConfig::getSystemResetCode(DetectConfig &newConfig)
{
	int resetCode = 0b000000000;
	if (ActualProductSize_W != newConfig.ActualProductSize_W ||
		ActualProductSize_H != newConfig.ActualProductSize_H)
	{
		//resetCode |= 0b1100;
	}
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
	dst->ActualProductSize_W = this->ActualProductSize_W; //产品实际宽度
	dst->ActualProductSize_H = this->ActualProductSize_H; //产品实际高度
	dst->nBasicUnitInRow = this->nBasicUnitInRow; //每一行中的基本单元数
	dst->nBasicUnitInCol = this->nBasicUnitInCol; //每一列中的基本单元数
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
bool Configurator::jsonSetValue(const QString &key, QString &value, bool encode)
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
				if (encode) { //加密
					QString encodeKey = encrypt(key); 
					QString encodeValue = encrypt(value);
					obj[encodeKey] = encodeValue;
				}
				else { //不加密
					obj[key] = value;
				}
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
bool Configurator::jsonSetValue(const QString &key, int &value, bool encode)
{
	return jsonSetValue(key, QString::number(value), encode);
}

//将参数写入配置文件中 - double
bool Configurator::jsonSetValue(const QString &key, double &value, bool encode)
{
	return jsonSetValue(key, QString::number(value, 'g', 7), encode);
}


/************* 从config文件中读取某个参数 ************/

//从配置文件中读取参数 - QString
bool Configurator::jsonReadValue(const QString &key, QString &value, bool decode)
{
	configFile->seek(0);
	QString val = configFile->readAll();
	QJsonParseError json_error;
	QJsonDocument confDcoument = QJsonDocument::fromJson(val.toUtf8(), &json_error);
	if (json_error.error == QJsonParseError::NoError) {
		if (!confDcoument.isNull() || !confDcoument.isEmpty()) {
			if (confDcoument.isObject()) {
				QJsonObject obj = confDcoument.object();
				if (decode) { //解密
					QString encodeKey = encrypt(key); 
					QString encodeValue = obj[encodeKey].toString();
					value = decrypt(encodeValue); 
				}
				else { //不加密
					value = obj[key].toString();
				}
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
bool Configurator::jsonReadValue(const QString &key, double &value, bool decode)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr, decode)) {
		value = valueStr.toDouble(); return true;
	}
	return false;
}

//从配置文件中读取参数 - int
bool Configurator::jsonReadValue(const QString &key, int &value, bool decode)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr, decode)) {
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
		//if (fileDateTime.isNull()) fileDateTime = fileInfo.created();

		keys[0] = fileDateTime.toString("dd").toInt() % 9;
		keys[1] = fileDateTime.toString("MM").toInt() % 9;
		keys[2] = fileDateTime.toString("yyyy").toInt() % 9;
		keys[3] = (keys[0] + keys[1] + keys[2] + 2019) % 9;
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
		config->markConfigFileMissing();//标记文件丢失
		saveConfigFile(fileName, config);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonReadValue("SampleDirPath", config->SampleDirPath, false);
		configurator.jsonReadValue("TemplDirPath", config->TemplDirPath, false);
		configurator.jsonReadValue("OutputDirPath", config->OutputDirPath, false);
		configurator.jsonReadValue("ImageFormat", config->ImageFormat, false);

		configurator.jsonReadValue("ActualProductSize_W", config->ActualProductSize_W, false);
		configurator.jsonReadValue("ActualProductSize_H", config->ActualProductSize_H, false);
		configurator.jsonReadValue("nBasicUnitInRow", config->nBasicUnitInRow, false);
		configurator.jsonReadValue("nBasicUnitInCol", config->nBasicUnitInCol, false);
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
		configurator.jsonSetValue("SampleDirPath", config->SampleDirPath, false);//样本文件夹
		configurator.jsonSetValue("TemplDirPath", config->TemplDirPath, false);//模板文件夹
		configurator.jsonSetValue("OutputDirPath", config->OutputDirPath, false);//输出文件夹
		configurator.jsonSetValue("ImageFormat", config->ImageFormat, false);//图像格式

		configurator.jsonSetValue("ActualProductSize_W", QString::number(config->ActualProductSize_W), false);
		configurator.jsonSetValue("ActualProductSize_H", QString::number(config->ActualProductSize_H), false);
		configurator.jsonSetValue("nBasicUnitInRow", QString::number(config->nBasicUnitInRow), false);
		configurator.jsonSetValue("nBasicUnitInCol", QString::number(config->nBasicUnitInCol), false);
		configFile.close();
	}
	return success;
}

//将配置文件中的参数加载到 AdminConfig 中
bool Configurator::loadConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) { //判断配置文件读写权限
		createConfigFile(configFilePath);//创建配置文件
		config->loadDefaultValue();//加载默认值
		config->markConfigFileMissing();//标记文件丢失
		saveConfigFile(fileName, config);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonReadValue("MaxMotionStroke", config->MaxMotionStroke, true);
		configurator.jsonReadValue("MaxCameraNum", config->MaxCameraNum, true);
		configurator.jsonReadValue("PixelsNumPerUnitLength", config->PixelsNumPerUnitLength, true);
		configurator.jsonReadValue("ImageOverlappingRate", config->ImageOverlappingRate, true);
		configurator.jsonReadValue("ImageSize_W", config->ImageSize_W, true);
		configurator.jsonReadValue("ImageSize_H", config->ImageSize_H, true);
		configFile.close();
	}
	return success;
}

//将 AdminConfig 中的参数保存到配置文件中
bool Configurator::saveConfigFile(const QString &fileName, AdminConfig *config)
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
		configurator.jsonSetValue("MaxMotionStroke", config->MaxMotionStroke, true);
		configurator.jsonSetValue("MaxCameraNum", config->MaxCameraNum, true);
		configurator.jsonSetValue("PixelsNumPerUnitLength", config->PixelsNumPerUnitLength, true);
		configurator.jsonSetValue("ImageOverlappingRate", config->ImageOverlappingRate, true);
		configurator.jsonSetValue("ImageSize_W", config->ImageSize_W, true);
		configurator.jsonSetValue("ImageSize_H", config->ImageSize_H, true);
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
