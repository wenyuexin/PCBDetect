#include "RuntimeLibrary.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


DetectParams::DetectParams()
{
	errorCode = Uncheck;
	serialNum = ""; //样本编号
	sampleModelNum = ""; //型号
	sampleBatchNum = ""; //批次号
	sampleNum = ""; //样本编号
	currentRow_detect = -1; //检测行号
	currentRow_extract = -1; //提取行号
	singleMotionStroke = -1; //运功动结构的单步行程
	nCamera = 0; //相机个数
	nPhotographing = 0; //拍照次数

	//缓存文件夹
	bufferDirPath = QDir::currentPath() + "/buffer"; 
	QDir bufferDir(bufferDirPath);
	if (!bufferDir.exists()) bufferDir.mkdir(bufferDirPath);
}

DetectParams::~DetectParams()
{
}


//重置产品序号
void DetectParams::resetSerialNum()
{
	serialNum = ""; //样本编号
	QString sampleModelNum = ""; //型号
	QString sampleBatchNum = ""; //批次号
	QString sampleNum = ""; //样本编号
}

//加载默认的运行参数
void DetectParams::loadDefaultValue()
{
	resetSerialNum();
	currentRow_detect = -1; //检测行号
	currentRow_extract = -1; //提取行号
	singleMotionStroke = 80; //运功动结构的单步行程
	nCamera = 5; //相机个数
	nPhotographing = 4; //拍照次数
}

//计算机械结构的单步运动距离 singleMotionStroke
DetectParams::ErrorCode DetectParams::calcSingleMotionStroke(AdminConfig *adminConfig)
{
	if (!adminConfig->isValid()) return DetectParams::Default;

	double overlap = adminConfig->ImageOverlappingRate; //图像重叠率
	double stroke = 1.0 * adminConfig->ImageSize_H / (1 - overlap);
	stroke /= adminConfig->PixelsNumPerUnitLength;
	this->singleMotionStroke = (int) ceil(stroke);

	//判断参数有效性
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_singleMotionStroke, adminConfig);
	return code;
}

//计算nCamera、nPhotographing
DetectParams::ErrorCode DetectParams::calcItemGridSize(AdminConfig *adminConfig, DetectConfig *detectConfig)
{
	if (!adminConfig->isValid() || !detectConfig->isValid(adminConfig)) return Default;

	double overlap = adminConfig->ImageOverlappingRate; //图像重叠率

	//计算需要开启的相机个数
	double nPixels_W = detectConfig->ActualProductSize_W * adminConfig->PixelsNumPerUnitLength;
	double nW = nPixels_W / adminConfig->ImageSize_W;
	this->nCamera = (int) ceil((nW - overlap) / (1 - overlap));

	//计算拍摄次数
	double nPixels_H = detectConfig->ActualProductSize_H * adminConfig->PixelsNumPerUnitLength;
	double nH = nPixels_H / adminConfig->ImageSize_H;
	this->nPhotographing = (int) ceil((nH - overlap) / (1 - overlap));

	//判断参数有效性
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_nCamera, adminConfig);
	if (code != ValidValue) return code;
	code = checkValidity(ParamsIndex::Index_nPhotographing, adminConfig);
	if (code != ValidValue) return code;

	return ValidValue;
}

//产品序号解析
DetectParams::ErrorCode DetectParams::parseSerialNum()
{
	if (checkValidity(Index_serialNum) != ValidValue) {
		return ErrorCode::Invalid_serialNum;
	}

	int begin = 0;
	sampleModelNum = serialNum.mid(begin, serialNumSlice[1]); //型号
	sampleModelNum = QString::number(sampleModelNum.toInt());//去除数字0

	begin += serialNumSlice[1];
	sampleBatchNum = serialNum.mid(begin, serialNumSlice[2]); //批次号
	sampleBatchNum = QString::number(sampleBatchNum.toInt());

	begin += serialNumSlice[2];
	sampleNum = serialNum.mid(begin, serialNumSlice[3]); //样本编号
	sampleNum = QString::number(sampleNum.toInt());

	return ValidValue;
}

//参数有效性检查
DetectParams::ErrorCode DetectParams::checkValidity(ParamsIndex index, AdminConfig *adminConfig)
{
	if (this->errorCode == ValidParams)
		return this->errorCode;

	ErrorCode code = ErrorCode::Uncheck;
	switch (index)
	{
	case pcb::DetectParams::Index_All:

	//产品序号相关参数
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

	//检测行号与提取行号
	case pcb::DetectParams::Index_currentRow_detect:
		if (code != Uncheck || index != Index_All) break;
	case pcb::DetectParams::Index_currentRow_extract:
		if (code != Uncheck || index != Index_All) break;

	//初始化相关参数
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

	//代码值等于Uncheck表示检测的参数有效
	if (code == Uncheck) code = ValidParams;

	//更新错误代码
	if (code != ValidParams || index == Index_All) {
		errorCode = code;
	}
	if (code != ValidParams &&
		index >= Index_All_SysInit && index <= Index_nPhotographing)
	{
		errorCode_sysInit = code;
	}
	if (code != ValidParams && 
		index >= Index_All_SerialNum && index <= Index_sampleNum)
	{
		errorCode_serialNum = code;
	}
	return code;
}

//判断参数是否有效
bool DetectParams::isValid(ParamsIndex index, AdminConfig *adminConfig)
{
	if (errorCode == ValidParams) return errorCode;
	if (index == Index_All) {
		if (errorCode == DetectParams::Uncheck) 
			checkValidity(index, adminConfig);
		return errorCode;
	}

	if (index == Index_All_SysInit) {
		if (errorCode_sysInit == DetectParams::Uncheck) 
			checkValidity(index, adminConfig);
		return errorCode_sysInit;
	}

	if (index == Index_All_SerialNum) {
		if (errorCode_serialNum == DetectParams::Uncheck) 
			checkValidity(index, adminConfig);
		return errorCode_serialNum;
	}
	return errorCode;
}

//获取错误代码
DetectParams::ErrorCode DetectParams::getErrorCode(ParamsIndex index)
{ 
	if (index == Index_All) return errorCode;
	else if (index == Index_All_SysInit) return errorCode_sysInit;
	else if (index == Index_All_SerialNum) return errorCode_serialNum;
	return Default;
}

//重置错误代码
void DetectParams::resetErrorCode(ParamsIndex index)
{ 
	if (index == Index_All) errorCode = Uncheck; 
	else if (index == Index_All_SysInit) errorCode_sysInit = Uncheck;
	else if (index == Index_All_SerialNum) errorCode_serialNum = Uncheck;
}

//弹窗报错
bool DetectParams::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	switch (tempCode)
	{
	case pcb::DetectParams::Uncheck:
		valueName = pcb::chinese("\"参数未验证\""); break;
	case pcb::DetectParams::Invalid_serialNum:
		valueName = pcb::chinese("\"产品序号\""); break;
	case pcb::DetectParams::Invalid_nCamera:
		valueName = pcb::chinese("\"相机个数\""); break;
	case pcb::DetectParams::Default:
		valueName = pcb::chinese("\"未知错误\""); break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("运行参数无效，请检查参数设置或图像数据！ \n") +
		pcb::chinese("错误来源：") + valueName + "!        \n" +
		"Config: Runtime: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}

//拷贝参数
void DetectParams::copyTo(DetectParams *dst)
{
	dst->errorCode = this->errorCode;
	dst->sampleModelNum = this->sampleModelNum; //型号
	dst->sampleBatchNum = this->sampleBatchNum; //批次号
	dst->sampleNum = this->sampleNum; //样本编号
	dst->currentRow_detect = this->currentRow_detect; //检测行号
	dst->currentRow_extract = this->currentRow_extract; //提取行号
	dst->singleMotionStroke = this->singleMotionStroke; //运功动结构的单步行程 mm
	dst->nCamera = this->nCamera; //相机个数
	dst->nPhotographing = this->nPhotographing; //拍照次数
}

//获取系统重置代码
int DetectParams::getSystemResetCode(DetectParams &newConfig)
{
	int sysResetCode = 0b000000000; //系统重置代码

	//运动结构的单步运动距离
	if (this->singleMotionStroke != newConfig.singleMotionStroke)
		sysResetCode |= 0b000100000;
	
	//相机个数
	if (this->nCamera != newConfig.nCamera)
		sysResetCode |= 0b000010011;

	//拍照次数
	if (this->nPhotographing != newConfig.nPhotographing)
		sysResetCode |= 0b000000011;
	
	return sysResetCode;
}


/****************************************************/
/*                   namespace pcb                  */
/****************************************************/

//非阻塞延迟
void pcb::delay(unsigned long msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//交互式文件夹路径选择
QString pcb::selectDirPath(QWidget *parent, QString windowTitle)
{
	if (windowTitle == "") windowTitle = chinese("请选择路径");

	QFileDialog *fileDialog = new QFileDialog(parent);
	fileDialog->setWindowTitle(windowTitle); //设置文件保存对话框的标题
	fileDialog->setFileMode(QFileDialog::Directory); //设置文件对话框弹出的时候显示文件夹
	fileDialog->setViewMode(QFileDialog::Detail); //文件以详细的形式显示，显示文件名，大小，创建日期等信息

	QString path = "";
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) //选择路径
		path = fileDialog->selectedFiles()[0];
	delete fileDialog;
	return path;
}
