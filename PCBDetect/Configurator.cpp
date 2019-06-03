#include "Configurator.h"

using pcb::UserConfig;
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
	ImageOverlappingRate_W = -1; //分图重叠率(宽)
	ImageOverlappingRate_H = -1; //分图重叠率(高)
	ImageSize_W = -1; //分图宽度
	ImageSize_H = -1; //分图高度
	ImageAspectRatio = -1; //图像宽高比
}

AdminConfig::~AdminConfig()
{
	qDebug() << "~AdminConfig";
}

//加载默认参数
void AdminConfig::loadDefaultValue()
{
	this->errorCode = Unchecked; //错误代码
	this->MaxMotionStroke = 80 * 5; //机械结构的最大运动行程
	this->PulseNumInUnitTime = 85; //单位时间内的脉冲数
	this->MaxCameraNum = 5; //可用相机的总数
	this->PixelsNumPerUnitLength = 40; //单位长度内的像素个数
	this->ImageOverlappingRate_W = 345.0 / 4384.0; //分图重叠率(宽)
	this->ImageOverlappingRate_H = 0.050000; //分图重叠率(高)
	this->ImageSize_W = 4384; //宽高比中的宽
	this->ImageSize_H = 3288; //宽高比中的高
	this->ImageAspectRatio = 1.0 * ImageSize_W / ImageSize_H; //样本图像的宽高比
}

//参数有效性检查
AdminConfig::ErrorCode AdminConfig::checkValidity(AdminConfig::ConfigIndex index)
{
	AdminConfig::ErrorCode code = Unchecked;
	switch (index)
	{
	case pcb::AdminConfig::Index_All:
	case pcb::AdminConfig::Index_MaxMotionStroke: //机械结构的最大运动行程
		if (MaxMotionStroke <= 0 || MaxMotionStroke > 650)
			code = Invalid_MaxMotionStroke;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_PulseNumInUnitTime: //机械结构的最大运动行程
		if (PulseNumInUnitTime <= 0 || PulseNumInUnitTime > 300)
			code = Invalid_PulseNumInUnitTime;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_MaxCameraNum: //可用相机总数
		if (MaxCameraNum <= 0 || MaxCameraNum > 10)
			code = Invalid_MaxCameraNum;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength: //单位长度的像素
		if (PixelsNumPerUnitLength <= 0 || PixelsNumPerUnitLength > 1000)
			code = Invalid_PixelsNumPerUnitLength;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_W: //分图重叠率(宽)
		if (ImageOverlappingRate_W <= 0 || ImageOverlappingRate_W >= 1) 
			code = Invalid_ImageOverlappingRate_W;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_H: //分图重叠率(高)
		if (ImageOverlappingRate_H <= 0 || ImageOverlappingRate_H >= 1)
			code = Invalid_ImageOverlappingRate_H;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_W: //分图宽度
		if (ImageSize_W <= 0 || ImageSize_W > MaxResolutionOfCamera.width())
			code = Invalid_ImageSize_W;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_H: //分图高度
		if (ImageSize_H <= 0 || ImageSize_H > MaxResolutionOfCamera.height())
			code = Invalid_ImageSize_H;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageAspectRatio: //图像宽高比
		if (code != Unchecked || index != Index_All) break;
	}

	if (code == Unchecked) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) errorCode = code;
	return code;
}

//判断参数是否有效
bool AdminConfig::isValid(bool doCheck) 
{
	if (doCheck && errorCode == AdminConfig::Unchecked)
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
		case pcb::AdminConfig::Invalid_PulseNumInUnitTime:
			return Index_PulseNumInUnitTime;
		case pcb::AdminConfig::Invalid_MaxCameraNum:
			return Index_MaxCameraNum;
		case pcb::AdminConfig::Invalid_PixelsNumPerUnitLength:
			return Index_PixelsNumPerUnitLength;
		case pcb::AdminConfig::Invalid_ImageOverlappingRate_W:
			return Index_ImageOverlappingRate_W;
		case pcb::AdminConfig::Invalid_ImageOverlappingRate_H:
			return Index_ImageOverlappingRate_H;
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
	if (tempCode == UserConfig::ValidConfig) return false;

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
		valueName = pcb::chinese("运行结构的最大行程"); break;
	case pcb::AdminConfig::Invalid_PulseNumInUnitTime:
		valueName = pcb::chinese("运动结构的单位脉冲数"); break;
	case pcb::AdminConfig::Invalid_MaxCameraNum:
		valueName = pcb::chinese("可用相机总数"); break;
	case pcb::AdminConfig::Invalid_PixelsNumPerUnitLength:
		valueName = pcb::chinese("每毫米像素数"); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate_W:
		valueName = pcb::chinese("分图重叠率(宽)"); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate_H:
		valueName = pcb::chinese("分图重叠率(高)"); break;
	case pcb::AdminConfig::Invalid_ImageSize_W:
	case pcb::AdminConfig::Invalid_ImageSize_H:
	case pcb::AdminConfig::Invalid_ImageAspectRatio:
		valueName = pcb::chinese("分图尺寸"); break;
	default:
		valueName = ""; break;
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
	if (this->PulseNumInUnitTime != other.PulseNumInUnitTime) return Index_PulseNumInUnitTime;
	if (this->MaxCameraNum != other.MaxCameraNum) return Index_MaxCameraNum;
	if (this->PixelsNumPerUnitLength != other.PixelsNumPerUnitLength) return Index_PixelsNumPerUnitLength;
	if (this->ImageOverlappingRate_W != other.ImageOverlappingRate_W) return Index_ImageOverlappingRate_W;
	if (this->ImageOverlappingRate_H != other.ImageOverlappingRate_H) return Index_ImageOverlappingRate_H;
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
		if (abs(ImageAspectRatio - newConfig.ImageAspectRatio) > 1E-6) {
			resetCode |= 0x000000011;
		}
	}
	//重置运动结构模块
	if (this->MaxMotionStroke != newConfig.MaxMotionStroke ||
		this->PulseNumInUnitTime != newConfig.PulseNumInUnitTime) 
	{
		resetCode |= 0x000100000;
	}

	//重置相机模块
	if (this->MaxCameraNum != newConfig.MaxCameraNum ||
		this->ImageSize_W != newConfig.ImageSize_W ||
		this->ImageSize_H != newConfig.ImageSize_H)
	{
		resetCode |= 0x000010000;
	}

	return resetCode;
}

//拷贝结构体
void AdminConfig::copyTo(AdminConfig *dst)
{
	dst->errorCode = this->errorCode;
	dst->MaxMotionStroke = this->MaxMotionStroke;
	dst->PulseNumInUnitTime = this->PulseNumInUnitTime;
	dst->MaxCameraNum = this->MaxCameraNum;
	dst->PixelsNumPerUnitLength = this->PixelsNumPerUnitLength;
	dst->ImageOverlappingRate_W = this->ImageOverlappingRate_W;
	dst->ImageSize_W = this->ImageSize_W;
	dst->ImageSize_H = this->ImageSize_H;
	dst->ImageAspectRatio = this->ImageAspectRatio;
}



/****************************************************/
/*                   UserConfig                   */
/****************************************************/

UserConfig::UserConfig()
{
	TemplDirPath = ""; //模板路径
	SampleDirPath = "";//样本路径
	OutputDirPath = "";//结果路径
	ImageFormat = ""; //图像后缀
	ActualProductSize_W = -1;//产品实际宽度,单位mm
	ActualProductSize_H = -1;//产品实际高度,单位mm
	nBasicUnitInRow = -1; //每一行中的基本单元数
	nBasicUnitInCol = -1; //每一列中的基本单元数

	matchingAccuracyLevel = 1; //匹配模式：1高精度 2低精度
	concaveRateThresh = 50; //线路缺失率的阈值
	convexRateThresh = 50; //线路凸起率的阈值
}

UserConfig::~UserConfig()
{
	qDebug() << "~UserConfig";
}

//加载默认值
void UserConfig::loadDefaultValue()
{
	QDir dir(QDir::currentPath());
	dir.cdUp(); //转到上一级目录
	QString appDirPath = dir.absolutePath(); //上一级目录的绝对路径

	this->errorCode = Unchecked; //错误代码
	this->TemplDirPath = appDirPath + "/template";//模板路径
	this->SampleDirPath = appDirPath + "/sample"; //样本路径
	this->OutputDirPath = appDirPath + "/output"; //结果路径
	this->ImageFormat = ".bmp"; //图像后缀
	this->clusterComPort = "COM1"; //COM口

	this->ActualProductSize_W = 500;//产品实际宽度
	this->ActualProductSize_H = 300;//产品实际高度
	this->nBasicUnitInRow = 4; //每一行中的基本单元数
	this->nBasicUnitInCol = 6; //每一列中的基本单元数

	this->matchingAccuracyLevel = 1; //匹配精度等级：1高精度 2低精度
	this->concaveRateThresh = 50; //线路缺失率的阈值
	this->convexRateThresh = 50; //线路凸起率的阈值
}

//参数有效性检查
UserConfig::ErrorCode UserConfig::checkValidity(ConfigIndex index, AdminConfig *adminConfig)
{
	ErrorCode code = Unchecked;
	switch (index)
	{
	case pcb::UserConfig::Index_All:
	case pcb::UserConfig::Index_TemplDirPath: //模板路径
		if (TemplDirPath == "" || !QFileInfo(TemplDirPath).isDir())
			code = Invalid_TemplDirPath;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_SampleDirPath: //样本路径
		if (SampleDirPath == "" || !QFileInfo(SampleDirPath).isDir())
			code = Invalid_SampleDirPath;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_OutputDirPath: //输出路径
		if (OutputDirPath == "" || !QFileInfo(OutputDirPath).isDir())
			code = Invalid_OutputDirPath;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_ImageFormat: //图像格式
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_clusterComPort: //COM串口
		if (code != Unchecked || index != Index_All) break;

	case pcb::UserConfig::Index_ActualProductSize_W: //产品实际宽度
		if (ActualProductSize_W < 1)
			code = Invalid_ActualProductSize_W;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_ActualProductSize_H: //产品实际高度
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: UserConfig: checkValidity: adminConfig is null !";
		if (ActualProductSize_H < 1 || 
			ActualProductSize_H > adminConfig->MaxMotionStroke)
			code = Invalid_ActualProductSize_H;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_nBasicUnitInRow: //每一行中的基本单元数
		if (nBasicUnitInRow < 1)
			code = Invalid_nBasicUnitInRow;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_nBasicUnitInCol: //每一列中的基本单元数
		if (nBasicUnitInCol < 1)
			code = Invalid_nBasicUnitInCol;
		if (code != Unchecked || index != Index_All) break;

	case pcb::UserConfig::Index_matchingAccuracyLevel: //匹配模式
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_concaveRateThresh: //缺失率阈值
		if (concaveRateThresh <= 0 || concaveRateThresh >= 100)
			code = Invalid_concaveRateThresh;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_convexRateThresh: //凸起率阈值
		if (concaveRateThresh <= 0 || concaveRateThresh >= 100)
			code = Invalid_convexRateThresh;
		if (code != Unchecked || index != Index_All) break;
	}

	if (code == Unchecked) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) this->errorCode = code;
	return code;
}

//判断参数是否有效
bool UserConfig::isValid(AdminConfig *adminConfig, bool doCheck) {
	if (doCheck && this->errorCode == UserConfig::Unchecked)
		checkValidity(Index_All, adminConfig);
	return this->errorCode == ValidConfig;
}

//将错误代码转为参数索引
UserConfig::ConfigIndex UserConfig::convertCodeToIndex(ErrorCode code)
{
	switch (code)
	{
	case pcb::UserConfig::ValidConfig:
		return Index_None;
	case pcb::UserConfig::Invalid_TemplDirPath:
		return Index_TemplDirPath;
	case pcb::UserConfig::Invalid_SampleDirPath:
		return Index_SampleDirPath;
	case pcb::UserConfig::Invalid_OutputDirPath:
		return Index_OutputDirPath;
	case pcb::UserConfig::Invalid_ImageFormat:
		return Index_ImageFormat;
	case pcb::UserConfig::Invalid_ActualProductSize_W:
		return Index_ActualProductSize_W;
	case pcb::UserConfig::Invalid_ActualProductSize_H:
		return Index_ActualProductSize_H;
	case pcb::UserConfig::Invalid_nBasicUnitInRow:
		return Index_nBasicUnitInRow;
	case pcb::UserConfig::Invalid_nBasicUnitInCol:
		return Index_nBasicUnitInCol;
	case pcb::UserConfig::Invalid_clusterComPort:
		return Index_clusterComPort;
	case pcb::UserConfig::Invalid_matchingAccuracyLevel:
		return Index_matchingAccuracyLevel;
	case pcb::UserConfig::Invalid_concaveRateThresh:
		return Index_concaveRateThresh;
	case pcb::UserConfig::Invalid_convexRateThresh:
		return Index_convexRateThresh;
	}
	return Index_None;
}

//参数报错
bool UserConfig::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == UserConfig::ValidConfig) return false;

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
	case pcb::UserConfig::Invalid_SampleDirPath:
		valueName = pcb::chinese("模板路径"); break;
	case pcb::UserConfig::Invalid_OutputDirPath:
		valueName = pcb::chinese("样本路径"); break;
	case pcb::UserConfig::Invalid_TemplDirPath:
		valueName = pcb::chinese("输出路径"); break;
	case pcb::UserConfig::Invalid_ImageFormat:
		valueName = pcb::chinese("图像格式"); break;
	case pcb::UserConfig::Invalid_ActualProductSize_W:
	case pcb::UserConfig::Invalid_ActualProductSize_H:
		valueName = pcb::chinese("产品实际尺寸"); break;
	case pcb::UserConfig::Invalid_nBasicUnitInRow:
	case pcb::UserConfig::Invalid_nBasicUnitInCol:
		valueName = pcb::chinese("基本单元数"); break;
	case pcb::UserConfig::Invalid_clusterComPort:
		valueName = pcb::chinese("运动控制串口"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("用户参数无效，请在参数设置界面重新设置") + valueName + "!  \n" +
		pcb::chinese("错误来源：") + valueName + "\n"
		"Config: User: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}

//不相等判断
UserConfig::ConfigIndex UserConfig::unequals(UserConfig &other) {
	if (this->TemplDirPath != other.TemplDirPath) return Index_TemplDirPath;
	if (this->SampleDirPath != other.SampleDirPath) return Index_SampleDirPath;
	if (this->OutputDirPath != other.OutputDirPath) return Index_OutputDirPath;
	if (this->ImageFormat != other.ImageFormat) return Index_ImageFormat;
	if (this->clusterComPort != other.clusterComPort) return Index_clusterComPort;

	if (this->ActualProductSize_W != other.ActualProductSize_W) return Index_ActualProductSize_W;
	if (this->ActualProductSize_H != other.ActualProductSize_H) return Index_ActualProductSize_H;
	if (this->nBasicUnitInRow != other.nBasicUnitInRow) return Index_nBasicUnitInRow;
	if (this->nBasicUnitInCol != other.nBasicUnitInCol) return Index_nBasicUnitInCol;

	if (this->matchingAccuracyLevel != other.matchingAccuracyLevel) return Index_matchingAccuracyLevel;
	if (this->concaveRateThresh != other.concaveRateThresh) return Index_concaveRateThresh;
	if (this->convexRateThresh != other.convexRateThresh) return Index_convexRateThresh;
	return Index_None;
}

//功能：获取系统重置代码
int UserConfig::getSystemResetCode(UserConfig &newConfig)
{
	int resetCode = 0b000000000;
	if (clusterComPort != newConfig.clusterComPort) {
		resetCode |= 0b000100000;
	}
	return resetCode;
}

//拷贝结构体
void UserConfig::copyTo(UserConfig *dst)
{
	dst->errorCode = this->errorCode; //参数有效性
	dst->TemplDirPath = this->TemplDirPath;//模板文件的存储路径
	dst->SampleDirPath = this->SampleDirPath;//样本文件存储路径
	dst->OutputDirPath = this->OutputDirPath;//检测结果存储路径
	dst->ImageFormat = this->ImageFormat; //图像后缀
	dst->clusterComPort = this->clusterComPort; //COM口

	dst->ActualProductSize_W = this->ActualProductSize_W; //产品实际宽度
	dst->ActualProductSize_H = this->ActualProductSize_H; //产品实际高度
	dst->nBasicUnitInRow = this->nBasicUnitInRow; //每一行中的基本单元数
	dst->nBasicUnitInCol = this->nBasicUnitInCol; //每一列中的基本单元数

	dst->matchingAccuracyLevel = this->matchingAccuracyLevel; //匹配模式：0高精度 1低精度
	dst->concaveRateThresh = this->concaveRateThresh; //线路缺失率的阈值
	dst->convexRateThresh = this->convexRateThresh; //线路凸起率的阈值
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
	qDebug() << "~Configurator";
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
	return jsonSetValue(key, QString::number(value, 'f', 6), encode);
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

//更新秘钥
void Configurator::updateKeys()
{
	QFileInfo fileInfo = QFileInfo(*configFile);

	//如果最近的修改时间为空，则使用文件的创建时间
	if (fileDateTime.isNull() || fileDateTime != fileInfo.lastModified()) {
		fileDateTime = fileInfo.lastModified();
		//if (fileDateTime.isNull()) fileDateTime = fileInfo.created();

		ushort dd = fileDateTime.toString("dd").toUShort() % 17;
		ushort mm = fileDateTime.toString("MM").toUShort() % 7;
		ushort yy = fileDateTime.toString("yy").toUShort() % 5;

		keys[0] = 179 - 11 * dd; //176
		keys[1] = 311 - 13 * dd - 17 * mm; //310
		keys[2] = 563 - 19 * dd - 23 * mm - 29 * yy; //558
		keys[3] = 883 - 31 * dd - 37 * mm - 41 * yy; //882
	}
}


//加密
QString Configurator::encrypt(QString origin) const
{
	QString encodeStr;
	int len = origin.size();
	for (int i = 0; i < len; i++) {
		ushort value = origin.at(i).unicode() ^ keys[i % 4];
		QString ch = QString::number(value, 32);
		if (i != 0) ch = "_" + ch;
		encodeStr.append(ch);
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
	QStringList chList = origin.split("_");
	QString encodeStr;
	for (size_t i = 0; i < chList.size(); i++) {
		if (chList[i] == "") continue;
		ushort value = chList[i].toUShort(Q_NULLPTR, 32);
		encodeStr.append(QChar(value ^ keys[i % 4]));
	}
	return encodeStr;
}

QString Configurator::decrypt(const char* origin) const
{
	return decrypt(QString(origin));
}


/****************** 配置类的读写 ********************/

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
		configurator.jsonReadValue("PulseNumInUnitTime", config->PulseNumInUnitTime, true);
		configurator.jsonReadValue("MaxCameraNum", config->MaxCameraNum, true);
		configurator.jsonReadValue("PixelsNumPerUnitLength", config->PixelsNumPerUnitLength, true);
		configurator.jsonReadValue("ImageOverlappingRate_W", config->ImageOverlappingRate_W, true);
		configurator.jsonReadValue("ImageOverlappingRate_H", config->ImageOverlappingRate_H, true);
		configurator.jsonReadValue("ImageSize_W", config->ImageSize_W, true);
		configurator.jsonReadValue("ImageSize_H", config->ImageSize_H, true);
		configFile.close();
	}
	return success;
}

//将AdminConfig中的参数保存到配置文件中
bool Configurator::saveConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		UserConfig defaultConfig;
		defaultConfig.loadDefaultValue();//加载默认值
		saveConfigFile(fileName, &defaultConfig);//保存默认config
		success = false;
	}
	else { //文件存在，并且可以正常读写
		Configurator configurator(&configFile);
		configurator.jsonSetValue("MaxMotionStroke", config->MaxMotionStroke, true);
		configurator.jsonSetValue("PulseNumInUnitTime", config->PulseNumInUnitTime, true);
		configurator.jsonSetValue("MaxCameraNum", config->MaxCameraNum, true);
		configurator.jsonSetValue("PixelsNumPerUnitLength", config->PixelsNumPerUnitLength, true);
		configurator.jsonSetValue("ImageOverlappingRate_W", config->ImageOverlappingRate_W, true);
		configurator.jsonSetValue("ImageOverlappingRate_H", config->ImageOverlappingRate_H, true);
		configurator.jsonSetValue("ImageSize_W", config->ImageSize_W, true);
		configurator.jsonSetValue("ImageSize_H", config->ImageSize_H, true);
		configFile.close();
	}
	return success;
}


//将配置文件中的参数加载到UserConfig中
bool Configurator::loadConfigFile(const QString &fileName, UserConfig *config)
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
		configurator.jsonReadValue("clusterComPort", config->clusterComPort, false);

		configurator.jsonReadValue("ActualProductSize_W", config->ActualProductSize_W, false);
		configurator.jsonReadValue("ActualProductSize_H", config->ActualProductSize_H, false);
		configurator.jsonReadValue("nBasicUnitInRow", config->nBasicUnitInRow, false);
		configurator.jsonReadValue("nBasicUnitInCol", config->nBasicUnitInCol, false);

		configurator.jsonReadValue("matchingAccuracyLevel", config->matchingAccuracyLevel, false);
		configurator.jsonReadValue("concaveRateThresh", config->concaveRateThresh, false);
		configurator.jsonReadValue("convexRateThresh", config->convexRateThresh, false);
		configFile.close();
	}
	return success;
}

//将 UserConfig 中的参数保存到配置文件中
bool Configurator::saveConfigFile(const QString &fileName, UserConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		UserConfig defaultConfig;
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
		configurator.jsonSetValue("clusterComPort", config->clusterComPort, false);

		configurator.jsonSetValue("ActualProductSize_W", config->ActualProductSize_W, false);
		configurator.jsonSetValue("ActualProductSize_H", config->ActualProductSize_H, false);
		configurator.jsonSetValue("nBasicUnitInRow", config->nBasicUnitInRow, false);
		configurator.jsonSetValue("nBasicUnitInCol", config->nBasicUnitInCol, false);

		configurator.jsonSetValue("matchingAccuracyLevel", config->matchingAccuracyLevel, false);
		configurator.jsonSetValue("concaveRateThresh", config->concaveRateThresh, false);
		configurator.jsonSetValue("convexRateThresh", config->convexRateThresh, false);
		configFile.close();
	}
	return success;
}


/******************* 暂时没用 ********************/

//获取当前磁盘剩余空间
//quint64 Configurator::getDiskFreeSpace(QString driver)
//{
//	LPCWSTR lpcwstrDriver = (LPCWSTR)driver.utf16();
//	ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
//
//	if (!GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes)) {
//		qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
//		return 0;
//	}
//	return (quint64)liTotalFreeBytes.QuadPart / 1024 / 1024 / 1024;
//}

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
