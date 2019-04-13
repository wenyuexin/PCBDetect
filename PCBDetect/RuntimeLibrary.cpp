#include "RuntimeLibrary.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


DetectParams::DetectParams()
{
	errorCode = Uncheck;
	errorCode_serialNum = Uncheck;
	errorCode_sysInit = Uncheck;

	serialNum = ""; //�������
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�
	singleMotionStroke = -1; //�˹����ṹ�ĵ����г�
	nCamera = 0; //�������
	nPhotographing = 0; //���մ���

	//�����ļ���
	bufferDirPath = QDir::currentPath() + "/buffer"; 
	QDir bufferDir(bufferDirPath);
	if (!bufferDir.exists()) bufferDir.mkdir(bufferDirPath);
}

DetectParams::~DetectParams()
{
}


//���ò�Ʒ���
void DetectParams::resetSerialNum()
{
	errorCode_serialNum = Uncheck;
	serialNum = ""; //�������
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������
}

//����Ĭ�ϵ����в���
void DetectParams::loadDefaultValue()
{
	errorCode = Uncheck;
	resetSerialNum(); //���ò�Ʒ���
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�

	errorCode_sysInit = Uncheck;
	singleMotionStroke = 80; //�˹����ṹ�ĵ����г�
	nCamera = 5; //�������
	nPhotographing = 4; //���մ���
}

//�����е�ṹ�ĵ����˶����� singleMotionStroke
DetectParams::ErrorCode DetectParams::calcSingleMotionStroke(AdminConfig *adminConfig)
{
	if (!adminConfig->isValid(true)) return DetectParams::Default;

	double overlap = adminConfig->ImageOverlappingRate; //ͼ���ص���
	double stroke = 1.0 * adminConfig->ImageSize_H / (1 - overlap);
	stroke /= adminConfig->PixelsNumPerUnitLength;
	this->singleMotionStroke = (int) ceil(stroke);

	//�жϲ�����Ч��
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_singleMotionStroke, adminConfig);
	return code;
}

//����nCamera��nPhotographing
DetectParams::ErrorCode DetectParams::calcItemGridSize(AdminConfig *adminConfig, DetectConfig *detectConfig)
{
	if (!adminConfig->isValid(true) || !detectConfig->isValid(adminConfig)) return Default;

	double overlap = adminConfig->ImageOverlappingRate; //ͼ���ص���

	//������Ҫ�������������
	double nPixels_W = detectConfig->ActualProductSize_W * adminConfig->PixelsNumPerUnitLength;
	double nW = nPixels_W / adminConfig->ImageSize_W;
	this->nCamera = (int) ceil((nW - overlap) / (1 - overlap));

	//�����������
	double nPixels_H = detectConfig->ActualProductSize_H * adminConfig->PixelsNumPerUnitLength;
	double nH = nPixels_H / adminConfig->ImageSize_H;
	this->nPhotographing = (int) ceil((nH - overlap) / (1 - overlap));

	//�жϲ�����Ч��
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_nCamera, adminConfig);
	if (code != ValidValue) return code;
	code = checkValidity(ParamsIndex::Index_nPhotographing, adminConfig);
	if (code != ValidValue) return code;

	return ValidValue;
}

//��Ʒ��Ž���
DetectParams::ErrorCode DetectParams::parseSerialNum()
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

//������Ч�Լ��
DetectParams::ErrorCode DetectParams::checkValidity(ParamsIndex index, AdminConfig *adminConfig)
{
	ErrorCode code = ErrorCode::Uncheck;
	switch (index)
	{
	case pcb::DetectParams::Index_All:

	//��Ʒ�����ز���
	case pcb::DetectParams::Index_All_SerialNum:
	case pcb::DetectParams::Index_serialNum:
		if (serialNum.size() != serialNumSlice[0]
			|| serialNum.toDouble() == 0)
		{
			code = Invalid_serialNum;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::DetectParams::Index_sampleModelNum:
		if (sampleModelNum == "" || sampleModelNum.size() > serialNumSlice[1]) {
			code = Invalid_sampleModelNum;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::DetectParams::Index_sampleBatchNum:
		if (sampleBatchNum == "" || sampleBatchNum.size() > serialNumSlice[2]) {
			code = Invalid_sampleBatchNum;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::DetectParams::Index_sampleNum:
		if (sampleNum == "" || sampleNum.size() > serialNumSlice[3]) {
			code = Invalid_sampleNum;
		}
		if (code != Uncheck || index != Index_All) break;

	//����к�����ȡ�к�
	case pcb::DetectParams::Index_currentRow_detect:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_currentRow_extract:
		if (code != Uncheck || index != Index_All) break;

	//��ʼ����ز���
	case pcb::DetectParams::Index_All_SysInit:
	case pcb::DetectParams::Index_singleMotionStroke:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectParams: checkValidity: adminConfig is NULL !";
		if (singleMotionStroke <= 0 || singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_singleMotionStroke;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SysInit) break;
	case pcb::DetectParams::Index_nCamera:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectParams: checkValidity: adminConfig is NULL !";
		if (nCamera <= 0 || nCamera > adminConfig->MaxCameraNum) {
			code = Invalid_nCamera;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SysInit) break;
	case pcb::DetectParams::Index_nPhotographing:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectParams: checkValidity: adminConfig is NULL !";
		if (nPhotographing * singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_nPhotographing; 
		}
		if (code != Uncheck || index != Index_All) break;
	}

	//����ֵ����Uncheck��ʾ���Ĳ�����Ч
	if (code == Uncheck) code = ValidParams;

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
bool DetectParams::isValid(ParamsIndex index, bool doCheck, AdminConfig *adminConfig)
{
	if (errorCode == ValidParams) return true;
	
	//���в���
	if (index == Index_All) {
		if (doCheck && errorCode == DetectParams::Uncheck)
			checkValidity(index, adminConfig);
		return (errorCode == ValidParams);
	}

	//��ʼ����ز���
	if (index == Index_All_SysInit) {
		if (doCheck && errorCode_sysInit == DetectParams::Uncheck)
			checkValidity(index, adminConfig);
		return (errorCode_sysInit == ValidValues);
	}

	//��Ʒ�����ز���
	if (index == Index_All_SerialNum) {
		if (doCheck && errorCode_serialNum == DetectParams::Uncheck)
			checkValidity(index);
		return (errorCode_serialNum == ValidValues);
	}
	return (errorCode == ValidParams);
}

//��ȡ�������
DetectParams::ErrorCode DetectParams::getErrorCode(ParamsIndex index)
{ 
	if (index == Index_All) return errorCode;
	else if (index == Index_All_SysInit) return errorCode_sysInit;
	else if (index == Index_All_SerialNum) return errorCode_serialNum;
	return Default;
}

//���ô������
void DetectParams::resetErrorCode(ParamsIndex index)
{ 
	if (index == Index_All) errorCode = Uncheck; 
	else if (index == Index_All_SysInit) errorCode_sysInit = Uncheck;
	else if (index == Index_All_SerialNum) errorCode_serialNum = Uncheck;
}

//��������
bool DetectParams::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	switch (tempCode)
	{
	case pcb::DetectParams::Uncheck:
		valueName = pcb::chinese("\"����δ��֤\""); break;
	case pcb::DetectParams::Invalid_serialNum:
		valueName = pcb::chinese("\"��Ʒ���\""); break;
	case pcb::DetectParams::Invalid_nCamera:
		valueName = pcb::chinese("\"�������\""); break;
	case pcb::DetectParams::Default:
		valueName = pcb::chinese("\"δ֪����\""); break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("���в�����Ч������������û�ͼ�����ݣ� \n") +
		pcb::chinese("������Դ��") + valueName + "!        \n" +
		"Config: Runtime: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
}

//��������
void DetectParams::copyTo(DetectParams *dst)
{
	dst->errorCode = this->errorCode;
	dst->sampleModelNum = this->sampleModelNum; //�ͺ�
	dst->sampleBatchNum = this->sampleBatchNum; //���κ�
	dst->sampleNum = this->sampleNum; //�������
	dst->currentRow_detect = this->currentRow_detect; //����к�
	dst->currentRow_extract = this->currentRow_extract; //��ȡ�к�
	dst->singleMotionStroke = this->singleMotionStroke; //�˹����ṹ�ĵ����г� mm
	dst->nCamera = this->nCamera; //�������
	dst->nPhotographing = this->nPhotographing; //���մ���
}

//��ȡϵͳ���ô���
int DetectParams::getSystemResetCode(DetectParams &newConfig)
{
	int sysResetCode = 0b000000000; //ϵͳ���ô���

	//�˶��ṹ�ĵ����˶�����
	if (this->singleMotionStroke != newConfig.singleMotionStroke)
		sysResetCode |= 0b000100000;
	
	//�������
	if (this->nCamera != newConfig.nCamera)
		sysResetCode |= 0b000010011;

	//���մ���
	if (this->nPhotographing != newConfig.nPhotographing)
		sysResetCode |= 0b000000011;
	
	return sysResetCode;
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
QString pcb::selectDirPath(QWidget *parent, QString windowTitle)
{
	if (windowTitle == "") windowTitle = chinese("��ѡ��·��");

	QFileDialog *fileDialog = new QFileDialog(parent);
	fileDialog->setWindowTitle(windowTitle); //�����ļ�����Ի���ı���
	fileDialog->setFileMode(QFileDialog::Directory); //�����ļ��Ի��򵯳���ʱ����ʾ�ļ���
	fileDialog->setViewMode(QFileDialog::Detail); //�ļ�����ϸ����ʽ��ʾ����ʾ�ļ�������С���������ڵ���Ϣ

	QString path = "";
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) //ѡ��·��
		path = fileDialog->selectedFiles()[0];
	delete fileDialog;
	return path;
}
