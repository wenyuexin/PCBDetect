#include "Configurator.h"

using pcb::UserConfig;
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
	ImageOverlappingRate_W = -1; //��ͼ�ص���(��)
	ImageOverlappingRate_H = -1; //��ͼ�ص���(��)
	ImageSize_W = -1; //��ͼ���
	ImageSize_H = -1; //��ͼ�߶�
	ImageAspectRatio = -1; //ͼ���߱�
}

AdminConfig::~AdminConfig()
{
	qDebug() << "~AdminConfig";
}

//����Ĭ�ϲ���
void AdminConfig::loadDefaultValue()
{
	this->errorCode = Unchecked; //�������
	this->MaxMotionStroke = 80 * 5; //��е�ṹ������˶��г�
	this->PulseNumInUnitTime = 85; //��λʱ���ڵ�������
	this->MaxCameraNum = 5; //�������������
	this->PixelsNumPerUnitLength = 40; //��λ�����ڵ����ظ���
	this->ImageOverlappingRate_W = 345.0 / 4384.0; //��ͼ�ص���(��)
	this->ImageOverlappingRate_H = 0.050000; //��ͼ�ص���(��)
	this->ImageSize_W = 4384; //��߱��еĿ�
	this->ImageSize_H = 3288; //��߱��еĸ�
	this->ImageAspectRatio = 1.0 * ImageSize_W / ImageSize_H; //����ͼ��Ŀ�߱�
}

//������Ч�Լ��
AdminConfig::ErrorCode AdminConfig::checkValidity(AdminConfig::ConfigIndex index)
{
	AdminConfig::ErrorCode code = Unchecked;
	switch (index)
	{
	case pcb::AdminConfig::Index_All:
	case pcb::AdminConfig::Index_MaxMotionStroke: //��е�ṹ������˶��г�
		if (MaxMotionStroke <= 0 || MaxMotionStroke > 650)
			code = Invalid_MaxMotionStroke;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_PulseNumInUnitTime: //��е�ṹ������˶��г�
		if (PulseNumInUnitTime <= 0 || PulseNumInUnitTime > 300)
			code = Invalid_PulseNumInUnitTime;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_MaxCameraNum: //�����������
		if (MaxCameraNum <= 0 || MaxCameraNum > 10)
			code = Invalid_MaxCameraNum;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength: //��λ���ȵ�����
		if (PixelsNumPerUnitLength <= 0 || PixelsNumPerUnitLength > 1000)
			code = Invalid_PixelsNumPerUnitLength;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_W: //��ͼ�ص���(��)
		if (ImageOverlappingRate_W <= 0 || ImageOverlappingRate_W >= 1) 
			code = Invalid_ImageOverlappingRate_W;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_H: //��ͼ�ص���(��)
		if (ImageOverlappingRate_H <= 0 || ImageOverlappingRate_H >= 1)
			code = Invalid_ImageOverlappingRate_H;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_W: //��ͼ���
		if (ImageSize_W <= 0 || ImageSize_W > MaxResolutionOfCamera.width())
			code = Invalid_ImageSize_W;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageSize_H: //��ͼ�߶�
		if (ImageSize_H <= 0 || ImageSize_H > MaxResolutionOfCamera.height())
			code = Invalid_ImageSize_H;
		if (code != Unchecked || index != Index_All) break;
	case pcb::AdminConfig::Index_ImageAspectRatio: //ͼ���߱�
		if (code != Unchecked || index != Index_All) break;
	}

	if (code == Unchecked) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) errorCode = code;
	return code;
}

//�жϲ����Ƿ���Ч
bool AdminConfig::isValid(bool doCheck) 
{
	if (doCheck && errorCode == AdminConfig::Unchecked)
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

//��������
bool AdminConfig::showMessageBox(QWidget *parent, AdminConfig::ErrorCode code)
{
	AdminConfig::ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == UserConfig::ValidConfig) return false;

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
		valueName = pcb::chinese("���нṹ������г�"); break;
	case pcb::AdminConfig::Invalid_PulseNumInUnitTime:
		valueName = pcb::chinese("�˶��ṹ�ĵ�λ������"); break;
	case pcb::AdminConfig::Invalid_MaxCameraNum:
		valueName = pcb::chinese("�����������"); break;
	case pcb::AdminConfig::Invalid_PixelsNumPerUnitLength:
		valueName = pcb::chinese("ÿ����������"); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate_W:
		valueName = pcb::chinese("��ͼ�ص���(��)"); break;
	case pcb::AdminConfig::Invalid_ImageOverlappingRate_H:
		valueName = pcb::chinese("��ͼ�ص���(��)"); break;
	case pcb::AdminConfig::Invalid_ImageSize_W:
	case pcb::AdminConfig::Invalid_ImageSize_H:
	case pcb::AdminConfig::Invalid_ImageAspectRatio:
		valueName = pcb::chinese("��ͼ�ߴ�"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("ϵͳ������Ч������ϵ����Ա��������") + valueName + "!        \n" +
		"Config: Admin: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}

//�����߱�
AdminConfig::ErrorCode AdminConfig::calcImageAspectRatio() 
{
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
	if (this->PulseNumInUnitTime != other.PulseNumInUnitTime) return Index_PulseNumInUnitTime;
	if (this->MaxCameraNum != other.MaxCameraNum) return Index_MaxCameraNum;
	if (this->PixelsNumPerUnitLength != other.PixelsNumPerUnitLength) return Index_PixelsNumPerUnitLength;
	if (this->ImageOverlappingRate_W != other.ImageOverlappingRate_W) return Index_ImageOverlappingRate_W;
	if (this->ImageOverlappingRate_H != other.ImageOverlappingRate_H) return Index_ImageOverlappingRate_H;
	if (this->ImageSize_W != other.ImageSize_W) return Index_ImageSize_W;
	if (this->ImageSize_H != other.ImageSize_H) return Index_ImageSize_H;
	return Index_None;
}

//���ܣ���ȡϵͳ���ô���
int AdminConfig::getSystemResetCode(AdminConfig &newConfig)
{
	int resetCode = 0b000000000;//���ô���

	//����ģ����ȡ��������
	if (ImageSize_W != newConfig.ImageSize_W || ImageSize_H != newConfig.ImageSize_H) {
		if (abs(ImageAspectRatio - newConfig.ImageAspectRatio) > 1E-6) {
			resetCode |= 0x000000011;
		}
	}
	//�����˶��ṹģ��
	if (this->MaxMotionStroke != newConfig.MaxMotionStroke ||
		this->PulseNumInUnitTime != newConfig.PulseNumInUnitTime) 
	{
		resetCode |= 0x000100000;
	}

	//�������ģ��
	if (this->MaxCameraNum != newConfig.MaxCameraNum ||
		this->ImageSize_W != newConfig.ImageSize_W ||
		this->ImageSize_H != newConfig.ImageSize_H)
	{
		resetCode |= 0x000010000;
	}

	return resetCode;
}

//�����ṹ��
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
	TemplDirPath = ""; //ģ��·��
	SampleDirPath = "";//����·��
	OutputDirPath = "";//���·��
	ImageFormat = ""; //ͼ���׺
	ActualProductSize_W = -1;//��Ʒʵ�ʿ��,��λmm
	ActualProductSize_H = -1;//��Ʒʵ�ʸ߶�,��λmm
	nBasicUnitInRow = -1; //ÿһ���еĻ�����Ԫ��
	nBasicUnitInCol = -1; //ÿһ���еĻ�����Ԫ��

	matchingAccuracyLevel = 1; //ƥ��ģʽ��1�߾��� 2�;���
	concaveRateThresh = 50; //��·ȱʧ�ʵ���ֵ
	convexRateThresh = 50; //��·͹���ʵ���ֵ
}

UserConfig::~UserConfig()
{
	qDebug() << "~UserConfig";
}

//����Ĭ��ֵ
void UserConfig::loadDefaultValue()
{
	QDir dir(QDir::currentPath());
	dir.cdUp(); //ת����һ��Ŀ¼
	QString appDirPath = dir.absolutePath(); //��һ��Ŀ¼�ľ���·��

	this->errorCode = Unchecked; //�������
	this->TemplDirPath = appDirPath + "/template";//ģ��·��
	this->SampleDirPath = appDirPath + "/sample"; //����·��
	this->OutputDirPath = appDirPath + "/output"; //���·��
	this->ImageFormat = ".bmp"; //ͼ���׺
	this->clusterComPort = "COM1"; //COM��

	this->ActualProductSize_W = 500;//��Ʒʵ�ʿ��
	this->ActualProductSize_H = 300;//��Ʒʵ�ʸ߶�
	this->nBasicUnitInRow = 4; //ÿһ���еĻ�����Ԫ��
	this->nBasicUnitInCol = 6; //ÿһ���еĻ�����Ԫ��

	this->matchingAccuracyLevel = 1; //ƥ�侫�ȵȼ���1�߾��� 2�;���
	this->concaveRateThresh = 50; //��·ȱʧ�ʵ���ֵ
	this->convexRateThresh = 50; //��·͹���ʵ���ֵ
}

//������Ч�Լ��
UserConfig::ErrorCode UserConfig::checkValidity(ConfigIndex index, AdminConfig *adminConfig)
{
	ErrorCode code = Unchecked;
	switch (index)
	{
	case pcb::UserConfig::Index_All:
	case pcb::UserConfig::Index_TemplDirPath: //ģ��·��
		if (TemplDirPath == "" || !QFileInfo(TemplDirPath).isDir())
			code = Invalid_TemplDirPath;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_SampleDirPath: //����·��
		if (SampleDirPath == "" || !QFileInfo(SampleDirPath).isDir())
			code = Invalid_SampleDirPath;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_OutputDirPath: //���·��
		if (OutputDirPath == "" || !QFileInfo(OutputDirPath).isDir())
			code = Invalid_OutputDirPath;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_ImageFormat: //ͼ���ʽ
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_clusterComPort: //COM����
		if (code != Unchecked || index != Index_All) break;

	case pcb::UserConfig::Index_ActualProductSize_W: //��Ʒʵ�ʿ��
		if (ActualProductSize_W < 1)
			code = Invalid_ActualProductSize_W;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_ActualProductSize_H: //��Ʒʵ�ʸ߶�
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: UserConfig: checkValidity: adminConfig is null !";
		if (ActualProductSize_H < 1 || 
			ActualProductSize_H > adminConfig->MaxMotionStroke)
			code = Invalid_ActualProductSize_H;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_nBasicUnitInRow: //ÿһ���еĻ�����Ԫ��
		if (nBasicUnitInRow < 1)
			code = Invalid_nBasicUnitInRow;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_nBasicUnitInCol: //ÿһ���еĻ�����Ԫ��
		if (nBasicUnitInCol < 1)
			code = Invalid_nBasicUnitInCol;
		if (code != Unchecked || index != Index_All) break;

	case pcb::UserConfig::Index_matchingAccuracyLevel: //ƥ��ģʽ
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_concaveRateThresh: //ȱʧ����ֵ
		if (concaveRateThresh <= 0 || concaveRateThresh >= 100)
			code = Invalid_concaveRateThresh;
		if (code != Unchecked || index != Index_All) break;
	case pcb::UserConfig::Index_convexRateThresh: //͹������ֵ
		if (concaveRateThresh <= 0 || concaveRateThresh >= 100)
			code = Invalid_convexRateThresh;
		if (code != Unchecked || index != Index_All) break;
	}

	if (code == Unchecked) code = ValidConfig;
	if (code != ValidConfig || index == Index_All) this->errorCode = code;
	return code;
}

//�жϲ����Ƿ���Ч
bool UserConfig::isValid(AdminConfig *adminConfig, bool doCheck) {
	if (doCheck && this->errorCode == UserConfig::Unchecked)
		checkValidity(Index_All, adminConfig);
	return this->errorCode == ValidConfig;
}

//���������תΪ��������
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

//��������
bool UserConfig::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == UserConfig::ValidConfig) return false;

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
	case pcb::UserConfig::Invalid_SampleDirPath:
		valueName = pcb::chinese("ģ��·��"); break;
	case pcb::UserConfig::Invalid_OutputDirPath:
		valueName = pcb::chinese("����·��"); break;
	case pcb::UserConfig::Invalid_TemplDirPath:
		valueName = pcb::chinese("���·��"); break;
	case pcb::UserConfig::Invalid_ImageFormat:
		valueName = pcb::chinese("ͼ���ʽ"); break;
	case pcb::UserConfig::Invalid_ActualProductSize_W:
	case pcb::UserConfig::Invalid_ActualProductSize_H:
		valueName = pcb::chinese("��Ʒʵ�ʳߴ�"); break;
	case pcb::UserConfig::Invalid_nBasicUnitInRow:
	case pcb::UserConfig::Invalid_nBasicUnitInCol:
		valueName = pcb::chinese("������Ԫ��"); break;
	case pcb::UserConfig::Invalid_clusterComPort:
		valueName = pcb::chinese("�˶����ƴ���"); break;
	default:
		valueName = ""; break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("�û�������Ч�����ڲ������ý�����������") + valueName + "!  \n" +
		pcb::chinese("������Դ��") + valueName + "\n"
		"Config: User: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}

//������ж�
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

//���ܣ���ȡϵͳ���ô���
int UserConfig::getSystemResetCode(UserConfig &newConfig)
{
	int resetCode = 0b000000000;
	if (clusterComPort != newConfig.clusterComPort) {
		resetCode |= 0b000100000;
	}
	return resetCode;
}

//�����ṹ��
void UserConfig::copyTo(UserConfig *dst)
{
	dst->errorCode = this->errorCode; //������Ч��
	dst->TemplDirPath = this->TemplDirPath;//ģ���ļ��Ĵ洢·��
	dst->SampleDirPath = this->SampleDirPath;//�����ļ��洢·��
	dst->OutputDirPath = this->OutputDirPath;//������洢·��
	dst->ImageFormat = this->ImageFormat; //ͼ���׺
	dst->clusterComPort = this->clusterComPort; //COM��

	dst->ActualProductSize_W = this->ActualProductSize_W; //��Ʒʵ�ʿ��
	dst->ActualProductSize_H = this->ActualProductSize_H; //��Ʒʵ�ʸ߶�
	dst->nBasicUnitInRow = this->nBasicUnitInRow; //ÿһ���еĻ�����Ԫ��
	dst->nBasicUnitInCol = this->nBasicUnitInCol; //ÿһ���еĻ�����Ԫ��

	dst->matchingAccuracyLevel = this->matchingAccuracyLevel; //ƥ��ģʽ��0�߾��� 1�;���
	dst->concaveRateThresh = this->concaveRateThresh; //��·ȱʧ�ʵ���ֵ
	dst->convexRateThresh = this->convexRateThresh; //��·͹���ʵ���ֵ
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
				if (encode) { //����
					QString encodeKey = encrypt(key); 
					QString encodeValue = encrypt(value);
					obj[encodeKey] = encodeValue;
				}
				else { //������
					obj[key] = value;
				}
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
bool Configurator::jsonSetValue(const QString &key, int &value, bool encode)
{
	return jsonSetValue(key, QString::number(value), encode);
}

//������д�������ļ��� - double
bool Configurator::jsonSetValue(const QString &key, double &value, bool encode)
{
	return jsonSetValue(key, QString::number(value, 'f', 6), encode);
}


/************* ��config�ļ��ж�ȡĳ������ ************/

//�������ļ��ж�ȡ���� - QString
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
				if (decode) { //����
					QString encodeKey = encrypt(key); 
					QString encodeValue = obj[encodeKey].toString();
					value = decrypt(encodeValue); 
				}
				else { //������
					value = obj[key].toString();
				}
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
bool Configurator::jsonReadValue(const QString &key, double &value, bool decode)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr, decode)) {
		value = valueStr.toDouble(); return true;
	}
	return false;
}

//�������ļ��ж�ȡ���� - int
bool Configurator::jsonReadValue(const QString &key, int &value, bool decode)
{
	QString valueStr = "";
	if (jsonReadValue(key, valueStr, decode)) {
		value = valueStr.toInt(); return true;
	}
	return false;
}


/******************* ��������� ********************/

//������Կ
void Configurator::updateKeys()
{
	QFileInfo fileInfo = QFileInfo(*configFile);

	//���������޸�ʱ��Ϊ�գ���ʹ���ļ��Ĵ���ʱ��
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


//����
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


//����
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


/****************** ������Ķ�д ********************/

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

//��AdminConfig�еĲ������浽�����ļ���
bool Configurator::saveConfigFile(const QString &fileName, AdminConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		UserConfig defaultConfig;
		defaultConfig.loadDefaultValue();//����Ĭ��ֵ
		saveConfigFile(fileName, &defaultConfig);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
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


//�������ļ��еĲ������ص�UserConfig��
bool Configurator::loadConfigFile(const QString &fileName, UserConfig *config)
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

//�� UserConfig �еĲ������浽�����ļ���
bool Configurator::saveConfigFile(const QString &fileName, UserConfig *config)
{
	bool success = true;
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.exists() || !configFile.open(QIODevice::ReadWrite)) {
		createConfigFile(configFilePath);
		UserConfig defaultConfig;
		defaultConfig.loadDefaultValue();//����Ĭ��ֵ
		saveConfigFile(fileName, &defaultConfig);//����Ĭ��config
		success = false;
	}
	else { //�ļ����ڣ����ҿ���������д
		Configurator configurator(&configFile);
		configurator.jsonSetValue("SampleDirPath", config->SampleDirPath, false);//�����ļ���
		configurator.jsonSetValue("TemplDirPath", config->TemplDirPath, false);//ģ���ļ���
		configurator.jsonSetValue("OutputDirPath", config->OutputDirPath, false);//����ļ���
		configurator.jsonSetValue("ImageFormat", config->ImageFormat, false);//ͼ���ʽ
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


/******************* ��ʱû�� ********************/

//��ȡ��ǰ����ʣ��ռ�
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
