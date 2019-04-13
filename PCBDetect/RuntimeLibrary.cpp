#include "RuntimeLibrary.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


DetectParams::DetectParams()
{
	errorCode = Uncheck;
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
	serialNum = ""; //�������
	QString sampleModelNum = ""; //�ͺ�
	QString sampleBatchNum = ""; //���κ�
	QString sampleNum = ""; //�������
}

//����Ĭ�ϵ����в���
void DetectParams::loadDefaultValue()
{
	resetSerialNum();
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�
	singleMotionStroke = 80; //�˹����ṹ�ĵ����г�
	nCamera = 5; //�������
	nPhotographing = 4; //���մ���
}

//�����е�ṹ�ĵ����˶����� singleMotionStroke
DetectParams::ErrorCode DetectParams::calcSingleMotionStroke(AdminConfig *adminConfig)
{
	if (!adminConfig->isValid()) return DetectParams::Default;

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
	if (!adminConfig->isValid() || !detectConfig->isValid(adminConfig)) return Default;

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
	if (serialNumSlice[0] != serialNum.size()) {
		return Invalid_SerialNum;
	}

	int to = 0;
	int from = serialNumSlice[0];
	sampleModelNum = serialNum.mid(to, from); //�ͺ�
	sampleModelNum = QString::number(sampleModelNum.toInt());//ȥ������0

	to = from;
	from = to + serialNumSlice[1];
	sampleBatchNum = serialNum.mid(to, from); //���κ�
	sampleBatchNum = QString::number(sampleBatchNum.toInt());

	to = from;
	from = to + serialNumSlice[2];
	sampleNum = serialNum.mid(to, from); //�������
	sampleNum = QString::number(sampleNum.toInt());

	return ValidValue;
}

//������Ч�Լ��
DetectParams::ErrorCode DetectParams::checkValidity(ParamsIndex index, AdminConfig *adminConfig)
{
	if (this->errorCode == ValidParams)
		return this->errorCode;

	ErrorCode code = ErrorCode::Uncheck;
	switch (index)
	{
	case pcb::DetectParams::Index_All:
	case pcb::DetectParams::Index_SerialNum:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_sampleModelNum:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_sampleBatchNum:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_sampleNum:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_currentRow_detect:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_currentRow_extract:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_singleMotionStroke:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectParams: checkValidity: adminConfig is NULL !";
		if (singleMotionStroke <= 0 || singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_singleMotionStroke;
		}
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_nCamera:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectParams: checkValidity: adminConfig is NULL !";
		if (nCamera <= 0 || nCamera > adminConfig->MaxCameraNum) {
			code = Invalid_nCamera;
		}
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_nPhotographing:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: DetectParams: checkValidity: adminConfig is NULL !";
		if (nPhotographing * singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_nPhotographing; 
		}
		if (code != Uncheck || index != Index_All) break;
	}

	if (code == Uncheck) code = ValidParams;
	if (code != ValidParams || index == Index_All) this->errorCode = code;
	return code;
}

//�жϲ����Ƿ���Ч
bool DetectParams::isValid(AdminConfig *adminConfig)
{
	if (this->errorCode == DetectParams::Uncheck) {
		checkValidity(Index_All, adminConfig);
	}
	return this->errorCode == ValidParams;
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
	case pcb::DetectParams::Invalid_SerialNum:
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
