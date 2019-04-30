#include "RuntimeLib.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


RuntimeParams::RuntimeParams()
{
	errorCode = Uncheck;
	errorCode_serialNum = Uncheck;
	errorCode_sysInit = Uncheck;

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

RuntimeParams::~RuntimeParams()
{
	qDebug() << "~RuntimeParams";
}


//重置产品序号
void RuntimeParams::resetSerialNum()
{
	errorCode_serialNum = Uncheck;
	serialNum = ""; //样本编号
	sampleModelNum = ""; //型号
	sampleBatchNum = ""; //批次号
	sampleNum = ""; //样本编号
}

//加载默认的运行参数
void RuntimeParams::loadDefaultValue()
{
	errorCode = Uncheck;
	resetSerialNum(); //重置产品序号
	currentRow_detect = -1; //检测行号
	currentRow_extract = -1; //提取行号

	errorCode_sysInit = Uncheck;
	singleMotionStroke = 79.0; //运功动结构的单步行程
	nCamera = 5; //相机个数
	nPhotographing = 4; //拍照次数
	initialPhotoPos = 245.0; //初始拍照位置
}

//计算机械结构的单步运动距离 singleMotionStroke
RuntimeParams::ErrorCode RuntimeParams::calcSingleMotionStroke(AdminConfig *adminConfig)
{
	if (!adminConfig->isValid(true)) return RuntimeParams::Default;

	double overlap = adminConfig->ImageOverlappingRate_W; //图像重叠率
	double stroke = 1.0 * adminConfig->ImageSize_H * (1 - overlap);
	stroke /= adminConfig->PixelsNumPerUnitLength;
	this->singleMotionStroke = stroke;
	//this->singleMotionStroke = 79;

	//判断参数有效性
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_singleMotionStroke, adminConfig);
	return code;
}

//计算nCamera、nPhotographing
RuntimeParams::ErrorCode RuntimeParams::calcItemGridSize(AdminConfig *adminConfig, UserConfig *userConfig)
{
	if (!adminConfig->isValid(true) || !userConfig->isValid(adminConfig)) return Default;

	//计算需要开启的相机个数
	double overlap_W = adminConfig->ImageOverlappingRate_W; //该值主要由相机之间的距离决定
	double nPixels_W = userConfig->ActualProductSize_W * adminConfig->PixelsNumPerUnitLength;
	double nW = nPixels_W / adminConfig->ImageSize_W;
	this->nCamera = (int) ceil((nW - overlap_W) / (1 - overlap_W));
	//this->nCamera = 3;

	//计算拍摄次数
	double overlap_H = adminConfig->ImageOverlappingRate_H; //图像重叠率
	double nPixels_H = userConfig->ActualProductSize_H * adminConfig->PixelsNumPerUnitLength;
	double nH = nPixels_H / adminConfig->ImageSize_H;
	this->nPhotographing = (int) ceil((nH - overlap_H) / (1 - overlap_H));
	//this->nPhotographing = 3;

	//判断参数有效性
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_nCamera, adminConfig);
	if (code != ValidValue) return code;
	code = checkValidity(ParamsIndex::Index_nPhotographing, adminConfig);
	if (code != ValidValue) return code;

	return ValidValue;
}

//计算初始拍照位置
RuntimeParams::ErrorCode RuntimeParams::calcInitialPhotoPos(pcb::AdminConfig *adminConfig)
{
	if (!adminConfig->isValid(true)) return Default;

	//参照位置，需保证刚好露出限制PCB板位置的金属条
	//还需保证在拍摄最后一行分图时，运动结构不会与限位器碰撞
	double initPos = 5; 

	//计算初始拍照位置
	initPos += ((nPhotographing - 1) * singleMotionStroke); 
	this->initialPhotoPos = initPos;
	//this->initialPhotoPos = 245 - 80;

	//判断参数有效性
	ErrorCode code = ErrorCode::Uncheck;
	code = checkValidity(ParamsIndex::Index_initialPhotoPos, adminConfig);
	return code;
}


//产品序号解析
RuntimeParams::ErrorCode RuntimeParams::parseSerialNum()
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
RuntimeParams::ErrorCode RuntimeParams::checkValidity(ParamsIndex index, AdminConfig *adminConfig)
{
	ErrorCode code = ErrorCode::Uncheck;
	switch (index)
	{
	case pcb::RuntimeParams::Index_All:

	//产品序号相关参数
	case pcb::RuntimeParams::Index_All_SerialNum:
	case pcb::RuntimeParams::Index_serialNum:
		if (serialNum.size() != serialNumSlice[0] || serialNum.toDouble() == 0) {
			code = Invalid_serialNum;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::RuntimeParams::Index_sampleModelNum:
		if (sampleModelNum == "" || sampleModelNum.size() > serialNumSlice[1]) {
			code = Invalid_sampleModelNum;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::RuntimeParams::Index_sampleBatchNum:
		if (sampleBatchNum == "" || sampleBatchNum.size() > serialNumSlice[2]) {
			code = Invalid_sampleBatchNum;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SerialNum) break;
	case pcb::RuntimeParams::Index_sampleNum:
		if (sampleNum == "" || sampleNum.size() > serialNumSlice[3]) {
			code = Invalid_sampleNum;
		}
		if (code != Uncheck || index != Index_All) break;

	//检测行号与提取行号
	case pcb::RuntimeParams::Index_currentRow_detect:
		if (code != Uncheck || index != Index_All) break;
	case pcb::RuntimeParams::Index_currentRow_extract:
		if (code != Uncheck || index != Index_All) break;

	//初始化相关参数
	case pcb::RuntimeParams::Index_All_SysInit:
	case pcb::RuntimeParams::Index_singleMotionStroke: //单步前进距离
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (singleMotionStroke <= 0 || singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_singleMotionStroke;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SysInit) break;
	case pcb::RuntimeParams::Index_nCamera: //相机个数
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (nCamera <= 0 || nCamera > adminConfig->MaxCameraNum) {
			code = Invalid_nCamera;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SysInit) break;
	case pcb::RuntimeParams::Index_nPhotographing: //拍照次数
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (nPhotographing * singleMotionStroke > adminConfig->MaxMotionStroke) {
			code = Invalid_nPhotographing; 
		}
		if (code != Uncheck || index != Index_All) break;
	case pcb::RuntimeParams::Index_initialPhotoPos: //初始拍照位置
		if (adminConfig == Q_NULLPTR)
			qDebug() << "Warning: RuntimeParams: checkValidity: adminConfig is NULL !";
		if (initialPhotoPos <= 0 || initialPhotoPos > adminConfig->MaxMotionStroke
			|| (initialPhotoPos - (nPhotographing - 1) * singleMotionStroke) < 3)
		{
			code = Invalid_initialPhotoPos;
		}
		if (code != Uncheck || index != Index_All || index != Index_All_SysInit) break;
	}

	//代码值等于Uncheck表示检测的参数有效
	if (code == Uncheck) code = ValidParams;

	//更新错误代码
	if (code != ValidParams || index == Index_All) {
		errorCode = code;
	}

	//更新错误代码 - 初始化相关参数
	if (index == Index_All_SysInit || 
		(code != ValidParams && index >= Index_singleMotionStroke && index <= Index_nPhotographing))
	{
		errorCode_sysInit = code;
	}

	//更新错误代码 - 产品序号相关参数
	if (index == Index_All_SerialNum || 
		(code != ValidParams && index >= Index_serialNum && index <= Index_sampleNum))
	{
		errorCode_serialNum = code;
	}
	return code;
}

//功能：判断参数是否有效
//参数：index 需要判断有效性的参数范围
//      doCheck 当值为true时，如果未检查过有效性，则先检查
//      adminConfig 判断初始化相关参数的有效性时，需要读取系统参数
bool RuntimeParams::isValid(ParamsIndex index, bool doCheck, AdminConfig *adminConfig)
{
	if (errorCode == ValidParams) return true;
	
	//所有参数
	if (index == Index_All) {
		if (doCheck && errorCode == RuntimeParams::Uncheck)
			checkValidity(index, adminConfig);
		return (errorCode == ValidParams);
	}

	//初始化相关参数
	if (index == Index_All_SysInit) {
		if (doCheck && errorCode_sysInit == RuntimeParams::Uncheck)
			checkValidity(index, adminConfig);
		return (errorCode_sysInit == ValidValues);
	}

	//产品序号相关参数
	if (index == Index_All_SerialNum) {
		if (doCheck && errorCode_serialNum == RuntimeParams::Uncheck)
			checkValidity(index);
		return (errorCode_serialNum == ValidValues);
	}
	return (errorCode == ValidParams);
}

//获取错误代码
RuntimeParams::ErrorCode RuntimeParams::getErrorCode(ParamsIndex index)
{ 
	if (index == Index_All) return errorCode;
	else if (index == Index_All_SysInit) return errorCode_sysInit;
	else if (index == Index_All_SerialNum) return errorCode_serialNum;
	return Default;
}

//重置错误代码
void RuntimeParams::resetErrorCode(ParamsIndex index)
{ 
	if (index == Index_All) errorCode = Uncheck; 
	else if (index == Index_All_SysInit) errorCode_sysInit = Uncheck;
	else if (index == Index_All_SerialNum) errorCode_serialNum = Uncheck;
}

//弹窗报错
bool RuntimeParams::showMessageBox(QWidget *parent, ErrorCode code)
{
	ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == UserConfig::ValidConfig) return false;

	QString valueName;
	switch (tempCode)
	{
	case pcb::RuntimeParams::Uncheck:
		valueName = pcb::chinese("\"参数未验证\""); break;
	case pcb::RuntimeParams::Invalid_serialNum:
	case pcb::RuntimeParams::Invalid_sampleModelNum:
	case pcb::RuntimeParams::Invalid_sampleBatchNum:
	case pcb::RuntimeParams::Invalid_sampleNum:
		valueName = pcb::chinese("\"产品序号\""); break;
	case pcb::RuntimeParams::Invalid_singleMotionStroke:
		valueName = pcb::chinese("\"单步前进距离\""); break;
	case pcb::RuntimeParams::Invalid_nCamera:
		valueName = pcb::chinese("\"相机个数\""); break;
	case pcb::RuntimeParams::Invalid_nPhotographing:
		valueName = pcb::chinese("\"拍照次数\""); break;
	case pcb::RuntimeParams::Invalid_initialPhotoPos:
		valueName = pcb::chinese("\"初始拍照位置\""); break;
	case pcb::RuntimeParams::Default:
		valueName = pcb::chinese("\"未知错误\""); break;
	default:
		valueName = "-"; break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("运行参数无效，请检查参数设置或图像数据！ \n") +
		pcb::chinese("错误来源：") + valueName + "        \n" +
		"Config: Runtime: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
}

//拷贝参数
void RuntimeParams::copyTo(RuntimeParams *dst)
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
int RuntimeParams::getSystemResetCode(RuntimeParams &newConfig)
{
	int sysResetCode = 0b000000000; //系统重置代码

	//运动结构的单步运动距离
	if (this->singleMotionStroke != newConfig.singleMotionStroke)
		sysResetCode |= 0b000100000;

	//运动结构的初始拍照位置
	if (this->initialPhotoPos != newConfig.initialPhotoPos)
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
