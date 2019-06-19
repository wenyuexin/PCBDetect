#include "RuntimeParams.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;
using std::vector;


RuntimeParams::RuntimeParams()
{
	errorCode = Unchecked;
	errorCode_serialNum = Unchecked;
	errorCode_sysInit = Unchecked;

	nCamera_raw = -1; //ԭʼ���������
	nPhotographing_raw = -1; //ԭʼ�����մ���

	serialNum = ""; //�������
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������

	currentRow_extract = -1; //��ȡ�к�
	maskRoi_tl = QPoint(-1, -1); //ģ����Ĥ���Ͻ��ڷ�ͼ�ϵ�����λ��
	maskRoi_br = QPoint(-1, -1); //ģ����Ĥ���½��ڷ�ͼ�ϵ�����λ��

	currentRow_detect = -1; //����к�

	singleMotionStroke = -1; //�˹����ṹ�ĵ����г�
	nCamera = 0; //�������
	nPhotographing = 0; //���մ���

	//ϵͳ��������
	AppDirPath = ""; //��������Ŀ¼
	BufferDirPath = ""; //�����ļ���
	currentSampleDir = ""; //��ǰ����ͼ����Ŀ¼
	currentTemplDir = ""; //��ǰģ��ͼ����Ŀ¼
	currentOutputDir = ""; //��ǰ�Ľ���洢Ŀ¼
	DeveloperMode = false; //����������ģʽ
	
	updateScreenRect();
}

RuntimeParams::~RuntimeParams()
{
	qDebug() << "~RuntimeParams";
}


//���ò�Ʒ���
void RuntimeParams::resetSerialNum()
{
	serialNum = ""; //�������
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������
}

//����Ĭ�ϵ����в���
void RuntimeParams::loadDefaultValue()
{
	errorCode = Unchecked;
	errorCode_serialNum = Unchecked;
	errorCode_sysInit = Unchecked;

	nCamera_raw = 0; //ԭʼ���������
	nPhotographing_raw = 0; //ԭʼ�����մ���

	resetSerialNum(); //���ò�Ʒ���
	currentRow_detect = -1; //����к�

	currentRow_extract = -1; //��ȡ�к�
	maskRoi_tl = QPoint(0, 0); //ģ����Ĥ���Ͻ��ڷ�ͼ�ϵ�����λ��
	maskRoi_br = QPoint(0, 0); //ģ����Ĥ���½��ڷ�ͼ�ϵ�����λ��

	errorCode_sysInit = Unchecked;
	singleMotionStroke = 79.0; //�˹����ṹ�ĵ����г�
	nCamera = 5; //�������
	nPhotographing = 4; //���մ���
	initialPhotoPos = 245.0; //��ʼ����λ��

	AppDirPath = QDir::currentPath(); //��������Ŀ¼
	BufferDirPath = QDir::currentPath() + "/buffer"; //�����ļ���
	QDir bufferDir(BufferDirPath);
	if (!bufferDir.exists()) bufferDir.mkdir(BufferDirPath);

	currentSampleDir = ""; //��ǰ����ͼ����Ŀ¼
	currentTemplDir = ""; //��ǰģ��ͼ����Ŀ¼
	currentOutputDir = ""; //��ǰ�Ľ���洢Ŀ¼

	updateScreenRect();
}

//��������
void RuntimeParams::copyTo(RuntimeParams *dst)
{
	dst->errorCode = this->errorCode;
	dst->errorCode_serialNum = this->errorCode_serialNum;
	dst->errorCode_sysInit = this->errorCode_sysInit;
	dst->systemState = this->systemState;

	dst->DeveloperMode = this->DeveloperMode; //����������ģʽ
	dst->AppDirPath = this->AppDirPath; //��������Ŀ¼
	dst->BufferDirPath = this->BufferDirPath; //�����ļ���
	dst->currentSampleDir = this->currentSampleDir; //��ǰ����ͼ����Ŀ¼
	dst->currentTemplDir = this->currentTemplDir; //��ǰģ��ͼ����Ŀ¼
	dst->currentOutputDir = this->currentOutputDir; //��ǰ�Ľ���洢Ŀ¼
	dst->ScreenRect = this->ScreenRect; //�������ڵ���Ļ����

	dst->serialNum = this->serialNum; //��Ʒ���
	dst->sampleModelNum = this->sampleModelNum; //�ͺ�
	dst->sampleBatchNum = this->sampleBatchNum; //���κ�
	dst->sampleNum = this->sampleNum; //�������

	dst->currentRow_detect = this->currentRow_detect; //����к�
	dst->currentRow_extract = this->currentRow_extract; //��ȡ�к�
	dst->maskRoi_tl = this->maskRoi_tl; //ģ����Ĥ���Ͻ��ڷ�ͼ�ϵ�����λ��
	dst->maskRoi_br = this->maskRoi_br; //ģ����Ĥ���½��ڷ�ͼ�ϵ�����λ��

	dst->singleMotionStroke = this->singleMotionStroke; //�˹����ṹ�ĵ����г� mm
	dst->nCamera = this->nCamera; //�������
	dst->nPhotographing = this->nPhotographing; //���մ���
	dst->initialPhotoPos = this->initialPhotoPos; //��ʼ����λ��
}


/*************** �����ĸ��¡����� ****************/

//������Ļ����
//����ѡ��������������ֱ���С��1440*900����������ʾ
void RuntimeParams::updateScreenRect()
{
	QDesktopWidget *desktop = QApplication::desktop();
	ScreenRect = desktop->screenGeometry(1); //�������ڵ���Ļ����
	if (ScreenRect.width() < 1440 || ScreenRect.height() < 900) {
		ScreenRect = desktop->screenGeometry(0);//��������
	}
}

//�����е�ṹ�ĵ����˶����� singleMotionStroke
RuntimeParams::ErrorCode RuntimeParams::calcSingleMotionStroke(AdminConfig *adminConfig)
{
	if (!adminConfig->isValid(true)) return RuntimeParams::Default;

	double overlap = adminConfig->ImageOverlappingRate_W; //ͼ���ص���
	double stroke = 1.0 * adminConfig->ImageSize_H * (1 - overlap);
	stroke /= adminConfig->PixelsNumPerUnitLength;
	this->singleMotionStroke = stroke;
	//this->singleMotionStroke = 79;

	//�жϲ�����Ч��
	ErrorCode code = ErrorCode::Unchecked;
	code = checkValidity(ParamsIndex::Index_singleMotionStroke, adminConfig);
	return code;
}

//����nCamera��nPhotographing
RuntimeParams::ErrorCode RuntimeParams::calcItemGridSize(AdminConfig *adminConfig, UserConfig *userConfig)
{
	if (!adminConfig->isValid(true) || !userConfig->isValid(adminConfig)) return Default;

	//������Ҫ�������������
	double overlap_W = adminConfig->ImageOverlappingRate_W; //��ֵ��Ҫ�����֮��ľ������
	double nPixels_W = userConfig->ActualProductSize_W * adminConfig->PixelsNumPerUnitLength;
	double nW = nPixels_W / adminConfig->ImageSize_W;
	nCamera_raw = (nW - overlap_W) / (1 - overlap_W);
	this->nCamera = (int) ceil(nCamera_raw);

	//�����������
	double overlap_H = adminConfig->ImageOverlappingRate_H; //ͼ���ص���
	double nPixels_H = userConfig->ActualProductSize_H * adminConfig->PixelsNumPerUnitLength;
	double nH = nPixels_H / adminConfig->ImageSize_H;
	nPhotographing_raw = (nH - overlap_H) / (1 - overlap_H);
	this->nPhotographing = (int) ceil(nPhotographing_raw);

	//�жϲ�����Ч��
	ErrorCode code = ErrorCode::Unchecked;
	code = checkValidity(ParamsIndex::Index_nCamera, adminConfig);
	if (code != ValidValue) return code;
	code = checkValidity(ParamsIndex::Index_nPhotographing, adminConfig);
	if (code != ValidValue) return code;

	return ValidValue;
}

//�����ʼ����λ��
RuntimeParams::ErrorCode RuntimeParams::calcInitialPhotoPos(pcb::AdminConfig *adminConfig)
{
	if (!adminConfig->isValid(true)) return Default;

	//����λ�ã��豣֤�պ�¶������PCB��λ�õĽ�����
	//���豣֤���������һ�з�ͼʱ���˶��ṹ��������λ����ײ
	double initPos = 6; 

	//�����ʼ����λ��
	initPos += ((nPhotographing - 1) * singleMotionStroke); 
	if (nPhotographing_raw - nPhotographing < 0.4) {
		//initPos -= 0.4*this->singleMotionStroke;
	}
	this->initialPhotoPos = initPos;
	//this->initialPhotoPos = 245 - 80;

	//�жϲ�����Ч��
	ErrorCode code = ErrorCode::Unchecked;
	code = checkValidity(ParamsIndex::Index_initialPhotoPos, adminConfig);
	return code;
}

//��Ʒ��Ž���
RuntimeParams::ErrorCode RuntimeParams::parseSerialNum()
{
	if (checkValidity(Index_serialNum) != ValidValue) {
		return ErrorCode::Invalid_serialNum;
	}

	int begin = 0;
	sampleModelNum = serialNum.mid(begin, serialNumSlice[1]); //�ͺ�
	sampleModelNum = QString::number(sampleModelNum.toInt());//ȥ������0

	begin += serialNumSlice[1];
	sampleBatchNum = serialNum.mid(begin, serialNumSlice[2]); //���κ�
	sampleBatchNum = QString::number(sampleBatchNum.toInt());

	begin += serialNumSlice[2];
	sampleNum = serialNum.mid(begin, serialNumSlice[3]); //�������
	sampleNum = QString::number(sampleNum.toInt());

	return ValidValue;
}

//��ȡϵͳ���ô���
int RuntimeParams::getSystemResetCode(RuntimeParams &newConfig)
{
	int sysResetCode = 0b000000000; //ϵͳ���ô���

	//�˶��ṹ�ĵ����˶�����
	if (this->singleMotionStroke != newConfig.singleMotionStroke)
		sysResetCode |= 0b000100000;

	//�˶��ṹ�ĳ�ʼ����λ��
	if (this->initialPhotoPos != newConfig.initialPhotoPos)
		sysResetCode |= 0b000100000;
	
	//�������
	if (this->nCamera != newConfig.nCamera)
		sysResetCode |= 0b000010011;

	//���մ���
	if (this->nPhotographing != newConfig.nPhotographing)
		sysResetCode |= 0b000000011;
	
	return sysResetCode;
}


/*************** ���в�������쳣���� **************/

//������Ч�Լ��
RuntimeParams::ErrorCode RuntimeParams::checkValidity(ParamsIndex index, AdminConfig *adminConfig)
{
	ErrorCode code = ErrorCode::Unchecked;
	switch (index)
	{
	case pcb::RuntimeParams::Index_All:

		//��Ʒ�����ز���
	case pcb::RuntimeParams::Index_All_SerialNum:
	case pcb::RuntimeParams::Index_serialNum:
		if (serialNum.size() != serialNumSlice[0] || serialNum.toDouble() == 0) {
			code = Invalid_serialNum;
		}
		if (code != Unchecked || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::RuntimeParams::Index_sampleModelNum:
		if (sampleModelNum == "" || sampleModelNum.size() > serialNumSlice[1]) {
			code = Invalid_sampleModelNum;
		}
		if (code != Unchecked || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::RuntimeParams::Index_sampleBatchNum:
		if (sampleBatchNum == "" || sampleBatchNum.size() > serialNumSlice[2]) {
			code = Invalid_sampleBatchNum;
		}
		if (code != Unchecked || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::RuntimeParams::Index_sampleNum:
		if (sampleNum == "" || sampleNum.size() > serialNumSlice[3]) {
			code = Invalid_sampleNum;
		}
		if (code != Unchecked || index != Index_All) break;

		//����к�����ȡ�к�
	case pcb::RuntimeParams::Index_currentRow_detect:
		if (code != Unchecked || index != Index_All) break;
	case pcb::RuntimeParams::Index_currentRow_extract:
		if (code != Unchecked || index != Index_All) break;

		//��ʼ����ز���
	case pcb::RuntimeParams::Index_All_SysInit:
	case pcb::RuntimeParams::Index_singleMotionStroke: //����ǰ������
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (singleMotionStroke <= 0 || singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_singleMotionStroke;
		}
		if (code != Unchecked || index != Index_All || index != Index_All_SysInit) break;
	case pcb::RuntimeParams::Index_nCamera: //�������
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (nCamera <= 0 || nCamera > adminConfig->MaxCameraNum) {
			code = Invalid_nCamera;
		}
		if (code != Unchecked || index != Index_All || index != Index_All_SysInit) break;
	case pcb::RuntimeParams::Index_nPhotographing: //���մ���
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (nPhotographing * singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_nPhotographing;
		}
		if (code != Unchecked || index != Index_All) break;
	case pcb::RuntimeParams::Index_initialPhotoPos: //��ʼ����λ��
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (initialPhotoPos <= 0 || initialPhotoPos > adminConfig->MaxMotionStroke
			|| (initialPhotoPos - (nPhotographing - 1) * singleMotionStroke) < 3)
		{
			code = Invalid_initialPhotoPos;
		}
		if (code != Unchecked || index != Index_All || index != Index_All_SysInit) break;
	}

	//����ֵ����Uncheck��ʾ���Ĳ�����Ч
	if (code == Unchecked) code = ValidParams;

	//���´������
	if (code != ValidParams || index == Index_All) {
		errorCode = code;
	}

	//���´������ - ��ʼ����ز���
	if (index == Index_All_SysInit ||
		(code != ValidParams && index >= Index_singleMotionStroke && index <= Index_nPhotographing))
	{
		errorCode_sysInit = code;
	}

	//���´������ - ��Ʒ�����ز���
	//if (index == Index_All_SerialNum && code == ValidParams) {
	//	QString dirpath = userConfig->SampleDirPath + "/" + runtimeParams->sampleModelNum + "/"
	//		+ runtimeParams->sampleBatchNum + "/" + runtimeParams->sampleNum;
	//}

	if (index == Index_All_SerialNum ||
		(code != ValidParams && index >= Index_serialNum && index <= Index_sampleNum))
	{
		errorCode_serialNum = code;
	}
	return code;
}

//���ܣ��жϲ����Ƿ���Ч
//������index ��Ҫ�ж���Ч�ԵĲ�����Χ
//      doCheck ��ֵΪtrueʱ�����δ������Ч�ԣ����ȼ��
//      adminConfig �жϳ�ʼ����ز�������Ч��ʱ����Ҫ��ȡϵͳ����
bool RuntimeParams::isValid(ParamsIndex index, bool doCheck, AdminConfig *adminConfig)
{
	if (errorCode == ValidParams) return true;

	//���в���
	if (index == Index_All) {
		if (doCheck && errorCode == RuntimeParams::Unchecked)
			checkValidity(index, adminConfig);
		return (errorCode == ValidParams);
	}

	//��ʼ����ز���
	if (index == Index_All_SysInit) {
		if (doCheck && errorCode_sysInit == RuntimeParams::Unchecked)
			checkValidity(index, adminConfig);
		return (errorCode_sysInit == ValidValues);
	}

	//��Ʒ�����ز���
	if (index == Index_All_SerialNum) {
		if (doCheck && errorCode_serialNum == RuntimeParams::Unchecked)
			checkValidity(index);
		return (errorCode_serialNum == ValidValues);
	}
	return (errorCode == ValidParams);
}

//��ȡ�������
RuntimeParams::ErrorCode RuntimeParams::getErrorCode(ParamsIndex index)
{
	if (index == Index_All) return errorCode;
	else if (index == Index_All_SysInit) return errorCode_sysInit;
	else if (index == Index_All_SerialNum) return errorCode_serialNum;
	return Default;
}

//���ô������
void RuntimeParams::resetErrorCode(ParamsIndex index)
{
	if (index == Index_All) errorCode = Unchecked;
	else if (index == Index_All_SysInit) errorCode_sysInit = Unchecked;
	else if (index == Index_All_SerialNum) errorCode_serialNum = Unchecked;
}

//��������
bool RuntimeParams::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == UserConfig::ValidConfig) return false;

	QString valueName;
	switch (tempCode)
	{
	case pcb::RuntimeParams::Unchecked:
		valueName = pcb::chinese("\"����δ��֤\""); break;
	case pcb::RuntimeParams::Invalid_serialNum:
	case pcb::RuntimeParams::Invalid_sampleModelNum:
	case pcb::RuntimeParams::Invalid_sampleBatchNum:
	case pcb::RuntimeParams::Invalid_sampleNum:
		valueName = pcb::chinese("\"��Ʒ���\""); break;
	case pcb::RuntimeParams::Invalid_singleMotionStroke:
		valueName = pcb::chinese("\"����ǰ������\""); break;
	case pcb::RuntimeParams::Invalid_nCamera:
		valueName = pcb::chinese("\"�������\""); break;
	case pcb::RuntimeParams::Invalid_nPhotographing:
		valueName = pcb::chinese("\"���մ���\""); break;
	case pcb::RuntimeParams::Invalid_initialPhotoPos:
		valueName = pcb::chinese("\"��ʼ����λ��\""); break;
	case pcb::RuntimeParams::Default:
		valueName = pcb::chinese("\"δ֪����\""); break;
	default:
		valueName = "-"; break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("���в�����Ч������������û�ͼ�����ݣ� \n") +
		pcb::chinese("������Դ��") + valueName + "        \n" +
		"Config: Runtime: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}