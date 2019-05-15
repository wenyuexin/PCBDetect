#include "RuntimeLib.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


RuntimeParams::RuntimeParams()
{
	errorCode = Unchecked;
	errorCode_serialNum = Unchecked;
	errorCode_sysInit = Unchecked;

	serialNum = ""; //�������
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�
	singleMotionStroke = -1; //�˹����ṹ�ĵ����г�
	nCamera = 0; //�������
	nPhotographing = 0; //���մ���

	//ϵͳ��������
	AppDirPath = ""; //��������Ŀ¼
	BufferDirPath = ""; //�����ļ���
	DeveloperMode = false; //����������ģʽ
	
	QDesktopWidget *desktop = QApplication::desktop();
	ScreenRect = desktop->screenGeometry(1); //�������ڵ���Ļ����
	if (ScreenRect.width() < 1440 || ScreenRect.height() < 900) {
		ScreenRect = desktop->screenGeometry(0);//��������
	}
}

RuntimeParams::~RuntimeParams()
{
	qDebug() << "~RuntimeParams";
}


//���ò�Ʒ���
void RuntimeParams::resetSerialNum()
{
	errorCode_serialNum = Unchecked;
	serialNum = ""; //�������
	sampleModelNum = ""; //�ͺ�
	sampleBatchNum = ""; //���κ�
	sampleNum = ""; //�������
}

//����Ĭ�ϵ����в���
void RuntimeParams::loadDefaultValue()
{
	errorCode = Unchecked;
	resetSerialNum(); //���ò�Ʒ���
	currentRow_detect = -1; //����к�
	currentRow_extract = -1; //��ȡ�к�

	errorCode_sysInit = Unchecked;
	singleMotionStroke = 79.0; //�˹����ṹ�ĵ����г�
	nCamera = 5; //�������
	nPhotographing = 4; //���մ���
	initialPhotoPos = 245.0; //��ʼ����λ��

	AppDirPath = QDir::currentPath(); //��������Ŀ¼
	BufferDirPath = QDir::currentPath() + "/buffer"; //�����ļ���
	QDir bufferDir(BufferDirPath);
	if (!bufferDir.exists()) bufferDir.mkdir(BufferDirPath);
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
	this->nCamera = (int) ceil((nW - overlap_W) / (1 - overlap_W));
	//this->nCamera = 3;

	//�����������
	double overlap_H = adminConfig->ImageOverlappingRate_H; //ͼ���ص���
	double nPixels_H = userConfig->ActualProductSize_H * adminConfig->PixelsNumPerUnitLength;
	double nH = nPixels_H / adminConfig->ImageSize_H;
	this->nPhotographing = (int) ceil((nH - overlap_H) / (1 - overlap_H));
	//this->nPhotographing = 3;

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
	double initPos = 5; 

	//�����ʼ����λ��
	initPos += ((nPhotographing - 1) * singleMotionStroke); 
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

//��������
void RuntimeParams::copyTo(RuntimeParams *dst)
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
	dst->AppDirPath = this->AppDirPath; //��������Ŀ¼
	dst->BufferDirPath = this->BufferDirPath; //�����ļ���
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

//ɾ���ַ�����β�ķ������ַ�
QString pcb::eraseNonDigitalCharInHeadAndTail(QString s)
{
	if (s == "") return "";
	int begin = 0;
	for (; begin < s.size() && !s.at(begin).isDigit(); begin++) {}
	int end = s.size() - 1;
	for (; end > begin && !s.at(end).isDigit(); end--) {}
	return s.mid(begin, end - begin + 1);
}


/* 
 * clearFiles():������ļ����ڵ��ļ�(���������ļ����ڵ��ļ�)
 * folderFullPath:�ļ���ȫ·��
 */
void pcb::clearFiles(const QString &folderFullPath)
{
	QDir dir(folderFullPath);
	dir.setFilter(QDir::Files);
	int fileCount = dir.count();
	for (int i = 0; i < fileCount; i++)
		dir.remove(dir[i]);
}

/*
 * clearFolder() - ɾ���ǿ��ļ���
 * folderFullPath - �ļ���ȫ·��
 * include - �Ƿ�Ҫɾ������·����Ӧ���ļ���
 */
void pcb::clearFolder(const QString &folderFullPath, bool included)
{
	QDir             dir(folderFullPath);
	QFileInfoList    fileList;
	QFileInfo        curFile;
	QFileInfoList    fileListTemp;
	int32_t          infoNum;
	int32_t          i;
	int32_t          j;

	/* ���Ȼ�ȡĿ���ļ����������ļ����ļ�����Ϣ */
	fileList = dir.entryInfoList(QDir::Dirs | QDir::Files
		| QDir::Readable | QDir::Writable
		| QDir::Hidden | QDir::NoDotAndDotDot
		, QDir::Name);

	while (fileList.size() > 0) {
		infoNum = fileList.size();
		for (i = infoNum - 1; i >= 0; i--) {
			curFile = fileList[i];
			if (curFile.isFile()) { /* ������ļ���ɾ���ļ� */
				QFile fileTemp(curFile.filePath());
				fileTemp.remove();
				fileList.removeAt(i);
			}

			if (curFile.isDir()) { /* ������ļ��� */
				QDir dirTemp(curFile.filePath());
				fileListTemp = dirTemp.entryInfoList(QDir::Dirs | QDir::Files
					| QDir::Readable | QDir::Writable
					| QDir::Hidden | QDir::NoDotAndDotDot
					, QDir::Name);

				if (fileListTemp.size() == 0) { /* �²�û���ļ����ļ��� ��ֱ��ɾ��*/
					dirTemp.rmdir(".");
					fileList.removeAt(i);
				}
				else { /* �²����ļ��л��ļ� ����Ϣ��ӵ��б�*/
					for (j = 0; j < fileListTemp.size(); j++) {
						if (!(fileList.contains(fileListTemp[j])))
							fileList.append(fileListTemp[j]);
					}
				}
			}
		}
	}

	//ɾ��Ŀ���ļ���
	//���ֻ������ļ���folderFullPath�����ݶ���ɾ��folderFullPath����,��ɾ�����м���
	//dir.rmdir(".");
}