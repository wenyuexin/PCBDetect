#include "Configurator.h"

using pcb::DetectConfig;
using pcb::AdminConfig;
using pcb::Configurator;


/****************************************************/
/*                   AdminConfig                    */
/****************************************************/

AdminConfig::AdminConfig()
{
	MaxMotionStroke = -1; //��е�ṹ������˶��г�
	MaxCameraNum = -1; //�����������
	PixelsNumPerUnitLength = -1; //��λ���ȵ����� pix/mm
	ImageOverlappingRate = -1; //��ͼ�ص���
	ImageSize_W = -1; //��ͼ���
	ImageSize_H = -1; //��ͼ�߶�
	ImageAspectRatio = -1; //ͼ���߱�
}

//����Ĭ�ϲ���
void AdminConfig::loadDefaultValue()
{
	this->errorCode = Uncheck; //�������
	this->MaxMotionStroke = 80*5; //��е�ṹ������˶��г�
	this->MaxCameraNum = 5; //�������������
	this->PixelsNumPerUnitLength = 40; //��λ�����ڵ����ظ���
	this->ImageOverlappingRate = 0.05; //��ͼ�ص���
	this->ImageSize_W = 4384; //��߱��еĿ�
	this->ImageSize_H = 3288; //��߱��еĸ�
	this->ImageAspectRatio = 1.0 * ImageSize_W / ImageSize_H; //����ͼ��Ŀ�߱�
}

//������Ч�Լ��
AdminConfig::ErrorCode AdminConfig::checkValidity(AdminConfig::ConfigIndex index)
{
	if (errorCode == ValidConfig) 
		return this->errorCode;

	AdminConfig::ErrorCode code = Uncheck;
	switch (index)
	{
	case pcb::AdminConfig::Index_All:
	case pcb::AdminConfig::Index_MaxMotionStroke: //��е�ṹ������˶��г�
		if (MaxMotionStroke <= 0) 
			code = Invalid_MaxMotionStroke;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_MaxCameraNum: //�����������
		if (MaxCameraNum <= 0) 
			code = Invalid_MaxCameraNum;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength: //��λ���ȵ�����
		if (PixelsNumPerUnitLength <= 0) 
			code = Invalid_PixelsNumPerUnitLength;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageOverlappingRate: //��ͼ�ص���
		if (ImageOverlappingRate <= 0 || ImageOverlappingRate >= 1) 
			code = Invalid_ImageOverlappingRate;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_W: //��ͼ���
		if (ImageSize_W <= 0)
			code = Invalid_ImageSize_W;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_H: //��ͼ�߶�
		if (ImageSize_H <= 0)
			code = Invalid_ImageSize_H;
		if (code != Uncheck || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageAspectRatio: //ͼ���߱�
		if (code != Uncheck || index != Index_All) break;
	}

	if (code == Uncheck) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) errorCode = code;
	return code;
}

//�жϲ����Ƿ���Ч
bool AdminConfig::isValid() 
{
	if (errorCode == AdminConfig::Uncheck)
		checkValidity(Index_All);
	return errorCode == ValidConfig;
}

//�������ת��������
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

//��������
bool AdminConfig::showMessageBox(QWidget *parent, AdminConfig::ErrorCode code)
{
	AdminConfig::ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	if (tempCode == AdminConfig::ConfigFileMissing) {
		QString message = pcb::chinese(".admin.config�ļ���ʧ��������Ĭ���ļ�!  \n")
			+ pcb::chinese("����ϵ����Աȷ�ϲ����Ƿ���Ч ...  \n");
		QMessageBox::warning(parent, pcb::chinese("����"),
			message + "Config: Admin: ErrorCode: " + QString::number(tempCode),
			pcb::chinese("ȷ��"));
		return true;
	}

	switch (tempCode)
	{
	case pcb::AdminConfig::Invalid_MaxMotionStroke:
		valueName = pcb::chinese("\"��е�ṹ����г�\""); break;
	case pcb::AdminConfig::Invalid_MaxCameraNum:
		valueName = pcb::chinese("\"�����������\""); break;
	case pcb::AdminConfig::Invalid_PixelsNumPerUnitLength:
		valueName = pcb::chinese("\"ÿ����������\""); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate:
		valueName = pcb::chinese("\"��ͼ�ص���\""); break;
	case pcb::AdminConfig::Invalid_ImageSize_W:
	case pcb::AdminConfig::Invalid_ImageSize_H:
	case pcb::AdminConfig::Invalid_ImageAspectRatio:
		valueName = pcb::chinese("\"��ͼ�ߴ�\""); break;
	default:
		valueName = "\"\""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("ϵͳ������Ч������ϵ����Ա��������") + valueName + "!        \n" +
		"Config: Admin: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}

//�����߱�
AdminConfig::ErrorCode AdminConfig::calcImageAspectRatio() {
	ErrorCode code = checkValidity(Index_ImageSize_W);
	if (code != ValidValue) return code;
	code = checkValidity(Index_ImageSize_H);
	if (code != ValidValue) return code;
	ImageAspectRatio = 1.0 * ImageSize_W / ImageSize_H;
	return ValidValue;
}

//������ж�
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

//���ܣ���ȡϵͳ���ô���
//���������һ�����ô��룬���벻ͬ�Ķ�����λ������ͬ�����ò���
//      0b1234 ��1λ��λ�����ʾ����ģ����ȡģ��
//             ��2λ��λ�����ʾ���ü��ģ��
//             ��3λ��λ�����ʾ�����˶��ṹ
//             ��4λ��λ�����ʾ�������
int AdminConfig::getSystemResetCode(AdminConfig &newConfig)
{
	int resetCode = 0b0000;//���ô���

	//����ģ����ȡ��������
	if (ImageSize_W != newConfig.ImageSize_W || ImageSize_H != newConfig.ImageSize_H)
		if (abs(ImageAspectRatio - newConfig.ImageAspectRatio) > 1E-6) resetCode |= 0x1100;
	//�����˶��ṹģ��
	if (this->MaxMotionStroke != newConfig.MaxMotionStroke) resetCode |= 0x0010;
	//�������ģ��
	if (this->MaxCameraNum != newConfig.MaxCameraNum) resetCode |= 0x0001;

	return resetCode;
}

//�����ṹ��
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
	SampleDirPath = "";//�����ļ��洢·��
	TemplDirPath = ""; //ģ���ļ��Ĵ洢·��
	OutputDirPath = "";//������洢·��
	ImageFormat = ""; //ͼ���׺
	ActualProductSize_W = -1;//��Ʒʵ�ʿ��,��λmm
	ActualProductSize_H = -1;//��Ʒʵ�ʸ߶�,��λmm
	nBasicUnitInRow = -1; //ÿһ���еĻ�����Ԫ��
	nBasicUnitInCol = -1; //ÿһ���еĻ�����Ԫ��
}

//����Ĭ��ֵ
void DetectConfig::loadDefaultValue()
{
	QDir dir(QDir::currentPath());
	dir.cdUp(); //ת����һ��Ŀ¼
	QString appDirPath = dir.absolutePath(); //��һ��Ŀ¼�ľ���·��

	this->errorCode = Uncheck; //�������
	this->SampleDirPath = appDirPath + "/sample"; //�����ļ��洢·��
	this->TemplDirPath = appDirPath + "/template";//ģ���ļ��Ĵ洢·��
	this->OutputDirPath = appDirPath + "/output";//������洢·��
	this->ImageFormat = ".bmp"; //ͼ���׺
	this->ActualProductSize_W = 500;//��Ʒʵ�ʿ��
	this->ActualProductSize_H = 600;//��Ʒʵ�ʸ߶�
	this->nBasicUnitInRow = 4; //ÿһ���еĻ�����Ԫ��
	this->nBasicUnitInCol = 6; //ÿһ���еĻ�����Ԫ��
}

//������Ч�Լ��
DetectConfig::ErrorCode DetectConfig::checkValidity(ConfigIndex index)
{
	if (this->errorCode == ValidConfig)
		return this->errorCode;

	ErrorCode code = Uncheck;
	switch (index)
	{
	case pcb::DetectConfig::Index_All:
	case pcb::DetectConfig::Index_SampleDirPath: //����·��
		if (SampleDirPath == "" || !QFileInfo(SampleDirPath).isDir())
			code = Invalid_SampleDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_TemplDirPath: //ģ��·��
		if (TemplDirPath == "" || !QFileInfo(TemplDirPath).isDir())
			code = Invalid_TemplDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_OutputDirPath: //���·��
		if (OutputDirPath == "" || !QFileInfo(OutputDirPath).isDir())
			code = Invalid_OutputDirPath;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ImageFormat: //ͼ���ʽ
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ActualProductSize_W: //��Ʒʵ�ʿ��
		if (ActualProductSize_W < 1)
			code = Invalid_ActualProductSize_W;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_ActualProductSize_H: //��Ʒʵ�ʸ߶�
		if (ActualProductSize_H < 1)
			code = Invalid_ActualProductSize_H;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nBasicUnitInRow: //ÿһ���еĻ�����Ԫ��
		if (nBasicUnitInRow < 1)
			code = Invalid_nBasicUnitInRow;
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectConfig::Index_nBasicUnitInCol: //ÿһ���еĻ�����Ԫ��
		if (nBasicUnitInCol < 1)
			code = Invalid_nBasicUnitInCol;
		if (code != Uncheck || index != Index_All) break;
	}

	if (code == Uncheck) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) this->errorCode = code;
	return code;
}

//�жϲ����Ƿ���Ч
bool DetectConfig::isValid() {
	if (this->errorCode == DetectConfig::Uncheck)
		checkValidity(Index_All);
	return this->errorCode == ValidConfig;
}

//���������תΪ��������
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

//��������
bool DetectConfig::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	if (tempCode == ConfigFileMissing) {
		QString message = pcb::chinese(".user.config�ļ���ʧ��������Ĭ���ļ�!    \n")
			+ pcb::chinese("���ڲ������ý���ȷ�ϲ����Ƿ���Ч ...   \n");
		QMessageBox::warning(parent, pcb::chinese("����"),
			message + "Config: User: ErrorCode: " + QString::number(tempCode),
			pcb::chinese("ȷ��"));
		return true;
	}

	switch (tempCode)
	{
	case pcb::DetectConfig::Invalid_SampleDirPath:
		valueName = pcb::chinese("\"����·��\""); break;
	case pcb::DetectConfig::Invalid_TemplDirPath:
		valueName = pcb::chinese("\"ģ��·��\""); break;
	case pcb::DetectConfig::Invalid_OutputDirPath:
		valueName = pcb::chinese("\"���·��\""); break;
	case pcb::DetectConfig::Invalid_ImageFormat:
		valueName = pcb::chinese("\"ͼ���ʽ\""); break;
	case pcb::DetectConfig::Invalid_ActualProductSize_W:
	case pcb::DetectConfig::Invalid_ActualProductSize_H:
		valueName = pcb::chinese("\"��Ʒʵ�ʳߴ�\""); break;
	case pcb::DetectConfig::Invalid_nBasicUnitInRow:
	case pcb::DetectConfig::Invalid_nBasicUnitInCol:
		valueName = pcb::chinese("\"������Ԫ��\""); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("�û�������Ч�����ڲ������ý�����������") + valueName + "!        \n" +
		"Config: User: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}

//������ж�
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

//���ܣ���ȡϵͳ���ô���
//���������һ�����ô��룬���벻ͬ�Ķ�����λ������ͬ�����ò���
//      0b1234 ��1λ��λ�����ʾ����ģ����ȡģ��
//             ��2λ��λ�����ʾ���ü��ģ��
//             ��3λ��λ�����ʾ�����˶��ṹ
//             ��4λ��λ�����ʾ�������
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

//�����ṹ��
void DetectConfig::copyTo(DetectConfig *dst)
{
	dst->errorCode = this->errorCode; //������Ч��
	dst->SampleDirPath = this->SampleDirPath; //�����ļ��洢·��
	dst->TemplDirPath = this->TemplDirPath;//ģ���ļ��Ĵ洢·��
	dst->OutputDirPath = this->OutputDirPath;//������洢·��
	dst->ImageFormat = this->ImageFormat; //ͼ���׺
	dst->ActualProductSize_W = this->ActualProductSize_W; //��Ʒʵ�ʿ��
	dst->ActualProductSize_H = this->ActualProductSize_H; //��Ʒʵ�ʸ߶�
	dst->nBasicUnitInRow = this->nBasicUnitInRow; //ÿһ���еĻ�����Ԫ��
	dst->nBasicUnitInCol = this->nBasicUnitInCol; //ÿһ���еĻ�����Ԫ��
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

/************** ����Ĭ�ϵĲ��������ļ� **************/

//����Ĭ�ϵĲ��������ļ�
void Configurator::createConfigFile(QString filePath)
{
	QFileInfo config(filePath);
	if (!config.isFile()) { //û�������ļ�  �򴴽��ļ� ; ���������ļ�
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


/********** ��ĳ��������д��config�ļ��� ************/

//������д�������ļ��� - QString
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
				QString encodeKey = encrypt(key); //����
				QString encodeValue = encrypt(value);
				obj[encodeKey] = encodeValue;
				QJsonDocument document = QJsonDocument::fromVariant(obj.toVariantMap());
				configFile->resize(0);
				textStrteam << document.toJson();
				updateKeys();//������Կ
				return true;
			}
		}
		else { //�ļ�Ϊ��
			qDebug() << "�ļ���";
		}
	}
	return false;
}

//������д�������ļ��� - int
bool Configurator::jsonSetValue(const QString &key, int &value)
{
	return jsonSetValue(key, QString::number(value));
}

//������д�������ļ��� - double
bool Configurator::jsonSetValue(const QString &key, double &value)
{
	return jsonSetValue(key, QString::number(value, 'g', 7));
}


/************* ��config�ļ��ж�ȡĳ������ ************/

//�������ļ��ж�ȡ���� - QString
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
				QString encodeKey = encrypt(key); //����
				QString encodeValue = obj[encodeKey].toString();
				value = decrypt(encodeValue); //����
				QByteArray();
				return true;
			}
		}
		else {
			qDebug() << "�ļ���";
			value = "";
		}
	}
	return false;
}

//�������ļ��ж�ȡ���� - double
bool Configurator::jsonReadValue(const QString &key, double &value)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr)) {
		value = valueStr.toDouble(); return true;
	}
	return false;
}

//�������ļ��ж�ȡ���� - int
bool Configurator::jsonReadValue(const QString &key, int &value)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr)) {
		value = valueStr.toInt(); return true;
	}
	return false;
}


/******************* ��������� ********************/

//�޸���Կ
void Configurator::updateKeys()
{
	QFileInfo fileInfo = QFileInfo(*configFile);

	//���������޸�ʱ��Ϊ�գ���ʹ���ļ��Ĵ���ʱ��
	if (fileDateTime.isNull() || fileDateTime != fileInfo.lastModified()) {
		fileDateTime = fileInfo.lastModified();
		if (fileDateTime.isNull()) fileDateTime = fileInfo.created();
		keys[0] = fileDateTime.toString("dd").toInt() % 9;
		keys[1] = fileDateTime.toString("MM").toInt() % 9;
		keys[2] = fileDateTime.toString("yyyy").toInt() % 9;
		keys[3] = (keys[0] + keys[1] + keys[2] + 2019) % 9;
	}
}

//����
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

//����
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


/****************** ������Ķ�д ********************/

//�������ļ��еĲ������ص�DetectConfig��
bool Configurator::loadConfigFile(const QString &fileName, DetectConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) { //�ж������ļ���дȨ��
		createConfigFile(configFilePath);//���������ļ�
		config->loadDefaultValue();//����Ĭ��ֵ
		config->markConfigFileMissing();//����ļ���ʧ
		saveConfigFile(fileName, config);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
		Configurator configurator(&configFile);
		configurator.jsonReadValue("SampleDirPath", config->SampleDirPath);
		configurator.jsonReadValue("TemplDirPath", config->TemplDirPath);
		configurator.jsonReadValue("OutputDirPath", config->OutputDirPath);
		configurator.jsonReadValue("ImageFormat", config->ImageFormat);

		configurator.jsonReadValue("ActualProductSize_W", config->ActualProductSize_W);
		configurator.jsonReadValue("ActualProductSize_H", config->ActualProductSize_H);
		configurator.jsonReadValue("nBasicUnitInRow", config->nBasicUnitInRow);
		configurator.jsonReadValue("nBasicUnitInCol", config->nBasicUnitInCol);
		configFile.close();
	}
	return success;
}

//�� DetectConfig �еĲ������浽�����ļ���
bool Configurator::saveConfigFile(const QString &fileName, DetectConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		DetectConfig defaultConfig;
		defaultConfig.loadDefaultValue();//����Ĭ��ֵ
		saveConfigFile(fileName, &defaultConfig);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
		Configurator configurator(&configFile);
		configurator.jsonSetValue("SampleDirPath", config->SampleDirPath);//�����ļ���
		configurator.jsonSetValue("TemplDirPath", config->TemplDirPath);//ģ���ļ���
		configurator.jsonSetValue("OutputDirPath", config->OutputDirPath);//����ļ���
		configurator.jsonSetValue("ImageFormat", config->ImageFormat);//ͼ���ʽ

		configurator.jsonSetValue("ActualProductSize_W", QString::number(config->ActualProductSize_W));//��Ʒʵ�ʿ��
		configurator.jsonSetValue("ActualProductSize_H", QString::number(config->ActualProductSize_H));//��Ʒʵ�ʸ߶�
		configurator.jsonSetValue("nBasicUnitInRow", QString::number(config->nBasicUnitInRow)); //ÿһ���еĻ�����Ԫ��
		configurator.jsonSetValue("nBasicUnitInCol", QString::number(config->nBasicUnitInCol)); //ÿһ���еĻ�����Ԫ��
		configFile.close();
	}
	return success;
}

//�������ļ��еĲ������ص� AdminConfig ��
bool Configurator::loadConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) { //�ж������ļ���дȨ��
		createConfigFile(configFilePath);//���������ļ�
		config->loadDefaultValue();//����Ĭ��ֵ
		config->markConfigFileMissing();//����ļ���ʧ
		saveConfigFile(fileName, config);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
		Configurator configurator(&configFile);
		configurator.jsonReadValue("MaxMotionStroke", config->MaxMotionStroke);
		configurator.jsonReadValue("MaxCameraNum", config->MaxCameraNum);
		configurator.jsonReadValue("PixelsNumPerUnitLength", config->PixelsNumPerUnitLength);
		configurator.jsonReadValue("ImageOverlappingRate", config->ImageOverlappingRate);
		configurator.jsonReadValue("ImageSize_W", config->ImageSize_W);
		configurator.jsonReadValue("ImageSize_H", config->ImageSize_H);
		configFile.close();
	}
	return success;
}

//�� AdminConfig �еĲ������浽�����ļ���
bool Configurator::saveConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		DetectConfig defaultConfig;
		defaultConfig.loadDefaultValue();//����Ĭ��ֵ
		saveConfigFile(fileName, &defaultConfig);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
		Configurator configurator(&configFile);
		configurator.jsonSetValue("MaxMotionStroke", config->MaxMotionStroke);
		configurator.jsonSetValue("MaxCameraNum", config->MaxCameraNum);
		configurator.jsonSetValue("PixelsNumPerUnitLength", config->PixelsNumPerUnitLength);
		configurator.jsonSetValue("ImageOverlappingRate", config->ImageOverlappingRate);
		configurator.jsonSetValue("ImageSize_W", config->ImageSize_W);
		configurator.jsonSetValue("ImageSize_H", config->ImageSize_H);
		configFile.close();
	}
	return success;
}

/******************* ��ʱû�� ********************/

//��ȡ��ǰ����ʣ��ռ�
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

//��ʱû��
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
		if (!config.isDir()) return false; //û�������ļ� �򴴽��ļ�
	}
	return true;
}
