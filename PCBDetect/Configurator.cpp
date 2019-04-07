#include "Configurator.h"

using pcb::DetectConfig;
using pcb::AdminConfig;
using pcb::Configurator;
using pcb::DetectParams;


/****************************************************/
/*                   DetectConfig                   */
/****************************************************/

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
	this->nCamera = 5; //�������
	this->nPhotographing = 4; //���մ���
	this->nBasicUnitInRow = 4; //ÿһ���еĻ�����Ԫ��
	this->nBasicUnitInCol = 6; //ÿһ���еĻ�����Ԫ��
	this->ImageAspectRatio_W = 4; //��߱��еĿ�
	this->ImageAspectRatio_H = 3; //��߱��еĸ�
	this->ImageAspectRatio = 4.0 / 3.0; //����ͼ��Ŀ�߱�
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

//��������
bool DetectConfig::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	if (tempCode == ConfigFileMissing) {
		QString message = QString::fromLocal8Bit(".user.config�ļ���ʧ��������Ĭ���ļ�!    \n")
			+ QString::fromLocal8Bit("���ڲ������ý���ȷ�ϲ����Ƿ���Ч ...   \n");
		QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
			message + "Config: User: ErrorCode: " + QString::number(tempCode),
			QString::fromLocal8Bit("ȷ��"));
		return true;
	}

	switch (tempCode)
	{
	case pcb::DetectConfig::Invalid_SampleDirPath:
		valueName = QString::fromLocal8Bit("����·��"); break;
	case pcb::DetectConfig::Invalid_TemplDirPath:
		valueName = QString::fromLocal8Bit("ģ��·��"); break;
	case pcb::DetectConfig::Invalid_OutputDirPath:
		valueName = QString::fromLocal8Bit("���·��"); break;
	case pcb::DetectConfig::Invalid_ImageFormat:
		valueName = QString::fromLocal8Bit("ͼ���ʽ"); break;
	case pcb::DetectConfig::Invalid_nCamera:
		valueName = QString::fromLocal8Bit("�������"); break;
	case pcb::DetectConfig::Invalid_nPhotographing:
		valueName = QString::fromLocal8Bit("�������"); break;
	case pcb::DetectConfig::Invalid_nBasicUnitInRow:
	case pcb::DetectConfig::Invalid_nBasicUnitInCol:
		valueName = QString::fromLocal8Bit("������Ԫ��"); break;
	case pcb::DetectConfig::Invalid_ImageAspectRatio_W:
	case pcb::DetectConfig::Invalid_ImageAspectRatio_H:
	case pcb::DetectConfig::Invalid_ImageAspectRatio:
		valueName = QString::fromLocal8Bit("ͼ���߱�"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
		QString::fromLocal8Bit("������Ч�����ڲ������ý�����������") + valueName + "!        \n" +
		"Config: User: ErrorCode: " + QString::number(tempCode),
		QString::fromLocal8Bit("ȷ��"));
	return true;
}

//�����߱�
DetectConfig::ErrorCode DetectConfig::calcImageAspectRatio() {
	ErrorCode code = checkValidity(Index_ImageAspectRatio_W);
	if (code != ValidValue) return this->errorCode = code;
	code = checkValidity(Index_ImageAspectRatio_H);
	if (code != ValidValue) return this->errorCode = code;
	ImageAspectRatio = 1.0 * ImageAspectRatio_W / ImageAspectRatio_H;
	return ValidValue;
}

//������ж�
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

//���ܣ���ȡϵͳ���ô���
//���������һ�����ô��룬���벻ͬ�Ķ�����λ������ͬ�����ò���
//      0b1234 ��1λ��λ�����ʾ����ģ����ȡģ��
//             ��2λ��λ�����ʾ���ü��ģ��
//             ��3λ��λ�����ʾ�����˶��ṹ
//             ��4λ��λ�����ʾ�������
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

//�����ṹ��
void DetectConfig::copyTo(DetectConfig *dst) 
{
	dst->errorCode = this->errorCode; //������Ч��
	dst->SampleDirPath = this->SampleDirPath; //�����ļ��洢·��
	dst->TemplDirPath = this->TemplDirPath;//ģ���ļ��Ĵ洢·��
	dst->OutputDirPath = this->OutputDirPath;//������洢·��
	dst->ImageFormat = this->ImageFormat; //ͼ���׺
	dst->nCamera = this->nCamera; //�������
	dst->nPhotographing = this->nPhotographing; //���մ���
	dst->nBasicUnitInRow = this->nBasicUnitInRow; //ÿһ���еĻ�����Ԫ��
	dst->nBasicUnitInCol = this->nBasicUnitInCol; //ÿһ���еĻ�����Ԫ��
	dst->ImageAspectRatio_W = this->ImageAspectRatio_W; //��߱��еĿ�
	dst->ImageAspectRatio_H = this->ImageAspectRatio_H; //��߱��еĸ�
	dst->ImageAspectRatio = this->ImageAspectRatio; //����ͼ��Ŀ�߱�
}



/****************************************************/
/*                   AdminConfig                    */
/****************************************************/

//����Ĭ�ϲ���
void AdminConfig::loadDefaultValue()
{
	this->errorCode = Uncheck; //�������
	this->MaxMotionStroke = 80*5; //��е�ṹ������˶��г�
	this->MaxCameraNum = 5; //�������������
	this->ImageResolutionRatio = 40; //ͼ��ֱ���
	this->ImageOverlappingRate = 0.05; //��ͼ�ص���
}

//������Ч�Լ��
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

//��������
bool AdminConfig::showMessageBox(QWidget *parent, AdminConfig::ErrorCode code)
{
	AdminConfig::ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	if (tempCode == AdminConfig::ConfigFileMissing) {
		QString message = QString::fromLocal8Bit(".admin.config�ļ���ʧ��������Ĭ���ļ�!  \n")
			+ QString::fromLocal8Bit("����ϵ����Աȷ�ϲ����Ƿ���Ч ...  \n");
		QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
			message + "Config: Admin: ErrorCode: " + QString::number(tempCode),
			QString::fromLocal8Bit("ȷ��"));
		return true;
	}

	switch (code)
	{
	case pcb::AdminConfig::Invalid_MaxMotionStroke:
		valueName = QString::fromLocal8Bit("��е�ṹ����г�"); break;
	case pcb::AdminConfig::Invalid_MaxCameraNum:
		valueName = QString::fromLocal8Bit("�����������"); break;
	case pcb::AdminConfig::Invalid_ImageResolutionRatio:
		valueName = QString::fromLocal8Bit("ͼ��ֱ���"); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate:
		valueName = QString::fromLocal8Bit("��ͼ�ص���"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, QString::fromLocal8Bit("����"),
		QString::fromLocal8Bit("������Ч������ϵ����Ա��������") + valueName + "!        \n" +
		"Config: Admin: ErrorCode: " + QString::number(tempCode),
		QString::fromLocal8Bit("ȷ��"));
	return true;
}

//������ж�
AdminConfig::ConfigIndex AdminConfig::unequals(AdminConfig &other)
{
	if (this->MaxMotionStroke != other.MaxMotionStroke) return Index_MaxMotionStroke;
	if (this->MaxCameraNum != other.MaxCameraNum) return Index_MaxCameraNum;
	if (this->ImageResolutionRatio != other.ImageResolutionRatio) return Index_ImageResolutionRatio;
	if (this->ImageOverlappingRate != other.ImageOverlappingRate) return Index_ImageOverlappingRate;
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
	int resetCode = 0b0000;

	//�����˶��ṹģ��
	if (this->MaxMotionStroke != newConfig.MaxMotionStroke) resetCode |= 0x0010;
	if (this->ImageResolutionRatio != newConfig.ImageResolutionRatio) resetCode |= 0x0010;
	if (this->ImageOverlappingRate != newConfig.ImageOverlappingRate) resetCode |= 0x0010;
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
		keys[0] = fileDateTime.toString("dd").toInt() % 10;
		keys[1] = fileDateTime.toString("MM").toInt() % 10;
		keys[2] = fileDateTime.toString("yyyy").toInt() % 10;
		keys[3] = (keys[1] + keys[2] + 2019) % 10;
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
		saveConfigFile(fileName, config);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
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
		configurator.jsonSetValue("nCamera", QString::number(config->nCamera)); //�������
		configurator.jsonSetValue("nPhotographing", QString::number(config->nPhotographing)); //���մ���
		configurator.jsonSetValue("nBasicUnitInRow", QString::number(config->nBasicUnitInRow)); //ÿһ���еĻ�����Ԫ��
		configurator.jsonSetValue("nBasicUnitInCol", QString::number(config->nBasicUnitInCol)); //ÿһ���еĻ�����Ԫ��
		configurator.jsonSetValue("ImageAspectRatio_W", QString::number(config->ImageAspectRatio_W)); //����ͼ��Ŀ�߱�
		configurator.jsonSetValue("ImageAspectRatio_H", QString::number(config->ImageAspectRatio_H)); //����ͼ��Ŀ�߱�
		configurator.jsonSetValue("ImageAspectRatio", QString::number(config->ImageAspectRatio, 'g', 7)); //����ͼ��Ŀ�߱�
		configFile.close();
	}
	return success;
}

//�������ļ��еĲ������ص� AdminConfig ��
bool Configurator::saveConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) { //�ж������ļ���дȨ��
		createConfigFile(configFilePath);//���������ļ�
		config->loadDefaultValue();//����Ĭ��ֵ
		saveConfigFile(fileName, config);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
		Configurator configurator(&configFile);
		configurator.jsonReadValue("MaxMotionStroke", config->MaxMotionStroke);
		configurator.jsonReadValue("MaxCameraNum", config->MaxCameraNum);
		configurator.jsonReadValue("ImageResolutionRatio", config->ImageResolutionRatio);
		configurator.jsonReadValue("ImageOverlappingRate", config->ImageOverlappingRate);
		configFile.close();
	}
	return success;
}

//�� AdminConfig �еĲ������浽�����ļ���
bool Configurator::loadConfigFile(const QString &fileName, AdminConfig *config)
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
		configurator.jsonSetValue("ImageResolutionRatio", config->ImageResolutionRatio);
		configurator.jsonSetValue("ImageOverlappingRate", config->ImageOverlappingRate);
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



/****************************************************/
/*                   DetectParams                   */
/****************************************************/

//���ò�Ʒ���
void DetectParams::resetSerialNum()
{
	QString sampleModelNum = ""; //�ͺ�
	QString sampleBatchNum = ""; //���κ�
	QString sampleNum = ""; //�������
}

//����Ĭ�ϵ����в���
void DetectParams::loadDefaultValue()
{
	resetSerialNum();
	imageSize = QSize(-1, -1);
	int currentRow_detect = -1; //����к�
	int currentRow_extract = -1; //��ȡ�к�
}



/****************************************************/
/*                   namespace pcb                  */
/****************************************************/

//�������ӳ�
void pcb::delay(unsigned long msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//����ʽ�ļ���·��ѡ��
QString pcb::selectDirPath(QString windowTitle)
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(windowTitle); //�����ļ�����Ի���ı���
	fileDialog->setFileMode(QFileDialog::Directory); //�����ļ��Ի��򵯳���ʱ����ʾ�ļ���
	fileDialog->setViewMode(QFileDialog::Detail); //�ļ�����ϸ����ʽ��ʾ����ʾ�ļ�������С���������ڵ���Ϣ

	QString path = "";
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) //ѡ��·��
		path = fileDialog->selectedFiles()[0];
	delete fileDialog;
	return path;
}
