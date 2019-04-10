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

//����nCamera��nPhotographing
int DetectParams::updateGridSize(AdminConfig *adminConfig, DetectConfig *detectConfig)
{
	int sysResetCode = 0b000000000; //ϵͳ���ô���
	if (adminConfig->getErrorCode() != AdminConfig::ValidConfig ||
		detectConfig->getErrorCode() != DetectConfig::ValidConfig)
		return sysResetCode;

	errorCode = Uncheck;
	int nCamera_old = nCamera;
	int nPhotographing_old = nPhotographing;
	double overlap = adminConfig->ImageOverlappingRate; //ͼ���ص���

	//������Ҫ�������������
	double nPixels_W = detectConfig->ActualProductSize_W * adminConfig->PixelsNumPerUnitLength;
	double nW = nPixels_W / adminConfig->ImageSize_W;
	this->nCamera = ceil((nW - overlap) / (1 - overlap));

	//�����������
	double nPixels_H = detectConfig->ActualProductSize_H * adminConfig->PixelsNumPerUnitLength;
	double nH = nPixels_H / adminConfig->ImageSize_H;
	this->nPhotographing = ceil((nH - overlap) / (1 - overlap));

	//�жϲ�����Ч��
	if (this->nCamera <= 0 || this->nCamera > adminConfig->MaxCameraNum) {
		errorCode = Invalid_nCamera; return errorCode;
	}
	else {
		errorCode = ValidParams;
	}

	//�ж��Ƿ���Ҫ����ϵͳ
	if (this->nCamera != nCamera_old)
		sysResetCode |= 0b000010110;
	if (this->nPhotographing != nPhotographing_old)
		sysResetCode |= 0b000000110;
	return sysResetCode;
}

//�жϲ����Ƿ���Ч
bool DetectParams::isValid() 
{
	if (this->errorCode == DetectParams::Uncheck) {
		//checkValidity(Index_All);
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
		pcb::chinese("���������") + valueName + "!        \n" +
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
