#include "RuntimeLibrary.h"

using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


DetectParams::DetectParams()
{
	errorCode = Uncheck;
	sampleModelNum = ""; //型号
	sampleBatchNum = ""; //批次号
	sampleNum = ""; //样本编号
	currentRow_detect = -1; //检测行号
	currentRow_extract = -1; //提取行号
	nCamera = 0; //相机个数
	nPhotographing = 0; //拍照次数
}

DetectParams::~DetectParams()
{
}


//重置产品序号
void DetectParams::resetSerialNum()
{
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
	nCamera = 5; //相机个数
	nPhotographing = 4; //拍照次数
}

//计算机械结构的单步运动距离 singleMotionStroke
int DetectParams::calcSingleMotionStroke(pcb::AdminConfig *adminConfig)
{
	int sysResetCode = 0b000000000; //系统重置代码
	if (!adminConfig->isValid()) return sysResetCode;

	int singleMotionStroke_old = singleMotionStroke;

	double overlap = adminConfig->ImageOverlappingRate; //图像重叠率
	double stroke = 1.0 * adminConfig->ImageSize_H / (1 - overlap);
	stroke /= adminConfig->PixelsNumPerUnitLength;
	this->singleMotionStroke = (int) ceil(stroke);

	//判断参数有效性
	ErrorCode code = checkValidity(ParamsIndex::Index_singleMotionStroke, adminConfig);
	if (code != ValidParams) return code;

	//判断是否需要重置系统
	if (this->singleMotionStroke != singleMotionStroke_old)
		sysResetCode |= 0b000100000;
	return sysResetCode;
}

//计算nCamera、nPhotographing
int DetectParams::calcItemGridSize(AdminConfig *adminConfig, DetectConfig *detectConfig)
{
	int sysResetCode = 0b000000000; //系统重置代码
	if (!adminConfig->isValid() || !detectConfig->isValid())
		return sysResetCode;

	int nCamera_old = this->nCamera;
	int nPhotographing_old = this->nPhotographing;
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
	ErrorCode code;
	code = checkValidity(ParamsIndex::Index_nCamera, adminConfig);
	if (code != ValidParams) return code;
	code = checkValidity(ParamsIndex::Index_nPhotographing, adminConfig);
	if (code != ValidParams) return code;

	//判断是否需要重置系统
	if (this->nCamera != nCamera_old)
		sysResetCode |= 0b000010110;
	if (this->nPhotographing != nPhotographing_old)
		sysResetCode |= 0b000000110;
	return sysResetCode;
}

//参数有效性检查
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

//判断参数是否有效
bool DetectParams::isValid(AdminConfig *adminConfig)
{
	if (this->errorCode == DetectParams::Uncheck) {
		checkValidity(Index_All, adminConfig);
	}
	return this->errorCode == ValidParams;
}

//弹窗报错
bool DetectParams::showMessageBox(QWidget *parent, ErrorCode code)
{
	DetectParams::ErrorCode tempCode = (code == Default) ? errorCode : code;
	if (tempCode == DetectConfig::ValidConfig) return false;

	QString valueName;
	switch (tempCode)
	{
	case pcb::DetectParams::ValidParams:
	case pcb::DetectParams::Uncheck:
		valueName = pcb::chinese("\"参数未验证\""); break;
	case pcb::DetectParams::Invalid_nCamera:
		valueName = pcb::chinese("\"相机个数\""); break;
	case pcb::DetectParams::Default:
		valueName = pcb::chinese("\"未知错误\""); break;
	}

	QMessageBox::warning(parent, pcb::chinese("警告"),
		pcb::chinese("运行参数无效，请检查参数设置或图像数据！ \n") +
		pcb::chinese("错误提示：") + valueName + "!        \n" +
		"Config: Runtime: ErrorCode: " + QString::number(tempCode),
		pcb::chinese("确定"));
	return true;
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
