#include "RuntimeLibrary.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


DetectParams::DetectParams()
{
	errorCode = Uncheck;
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�
	nCamera = 0; //�������
	nPhotographing = 0; //���մ���
}

DetectParams::~DetectParams()
{
}


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
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�
	nCamera = 5; //�������
	nPhotographing = 4; //���մ���
}

//�����е�ṹ�ĵ����˶����� singleMotionStroke
int DetectParams::calcSingleMotionStroke(pcb::AdminConfig *adminConfig)
{
	int sysResetCode = 0b000000000; //ϵͳ���ô���
	if (!adminConfig->isValid()) return sysResetCode;

	int singleMotionStroke_old = singleMotionStroke;

	double overlap = adminConfig->ImageOverlappingRate; //ͼ���ص���
	double stroke = 1.0 * adminConfig->ImageSize_H / (1 - overlap);
	stroke /= adminConfig->PixelsNumPerUnitLength;
	this->singleMotionStroke = (int) ceil(stroke);

	//�жϲ�����Ч��
	ErrorCode code = checkValidity(ParamsIndex::Index_singleMotionStroke, adminConfig);
	if (code != ValidParams) return code;

	//�ж��Ƿ���Ҫ����ϵͳ
	if (this->singleMotionStroke != singleMotionStroke_old)
		sysResetCode |= 0b000100000;
	return sysResetCode;
}

//����nCamera��nPhotographing
int DetectParams::calcItemGridSize(AdminConfig *adminConfig, DetectConfig *detectConfig)
{
	int sysResetCode = 0b000000000; //ϵͳ���ô���
	if (!adminConfig->isValid() || !detectConfig->isValid())
		return sysResetCode;

	int nCamera_old = this->nCamera;
	int nPhotographing_old = this->nPhotographing;
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
	ErrorCode code;
	code = checkValidity(ParamsIndex::Index_nCamera, adminConfig);
	if (code != ValidParams) return code;
	code = checkValidity(ParamsIndex::Index_nPhotographing, adminConfig);
	if (code != ValidParams) return code;

	//�ж��Ƿ���Ҫ����ϵͳ
	if (this->nCamera != nCamera_old)
		sysResetCode |= 0b000010110;
	if (this->nPhotographing != nPhotographing_old)
		sysResetCode |= 0b000000110;
	return sysResetCode;
}

//������Ч�Լ��
DetectParams::ErrorCode DetectParams::checkValidity(ParamsIndex index, AdminConfig *adminConfig)
{
	if (this->errorCode == ValidParams)
		return this->errorCode;

	ErrorCode code = Uncheck;
	switch (index)
	{
	case pcb::DetectParams::Index_All:
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
			qDebug() << "DetectParams: checkValidity: adminConfig == Q_NULLPTR";
		if (singleMotionStroke <= 0 || singleMotionStroke > adminConfig->MaxMotionStroke) {
			errorCode = Invalid_singleMotionStroke;
		}
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_nCamera:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "DetectParams: checkValidity: adminConfig == Q_NULLPTR";
		if (nCamera <= 0 || nCamera > adminConfig->MaxCameraNum) {
			errorCode = Invalid_nCamera;
		}
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_nPhotographing:
		if (adminConfig == Q_NULLPTR)
			qDebug() << "DetectParams: checkValidity: adminConfig == Q_NULLPTR";
		if (nPhotographing * singleMotionStroke > adminConfig->MaxMotionStroke) {
			errorCode = Invalid_nPhotographing; 
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
	DetectParams::ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	switch (tempCode)
	{
	case pcb::DetectParams::ValidParams:
	case pcb::DetectParams::Uncheck:
		valueName = pcb::chinese("\"����δ��֤\""); break;
	case pcb::DetectParams::Invalid_nCamera:
		valueName = pcb::chinese("\"�������\""); break;
	case pcb::DetectParams::Default:
		valueName = pcb::chinese("\"δ֪����\""); break;
	}

	QMessageBox::warning(parent, pcb::chinese("����"),
		pcb::chinese("���в�����Ч������������û�ͼ�����ݣ� \n") +
		pcb::chinese("������ʾ��") + valueName + "!        \n" +
		"Config: Runtime: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("ȷ��"));
	return true;
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
