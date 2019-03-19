#include "SettingUI.h"

using Ui::DetectConfig;

typedef Ui::DetectConfig::ErrorCode ConfigErrorCode;
typedef Ui::DetectConfig::ConfigIndex ConfigIndex;

SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//设置界面初始化
	this->setFocusPolicy(Qt::ClickFocus);
	initSettingUI();

	//参数下拉框的槽函数连接
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imageFormat()));
	//connect(ui.comboBox_imageAspectRatio, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_aspectRatio()));
}

SettingUI::~SettingUI()
{
}


/************* 界面的设置、输入 **************/

void SettingUI::initSettingUI()
{
	//限制参数的输入范围
	ui.lineEdit_nCamera->setValidator(&intValidator);
	ui.lineEdit_nPhotographing->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInRow->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInCol->setValidator(&intValidator);
	ui.lineEdit_ImageAspectRatio_W->setValidator(&intValidator);
	ui.lineEdit_ImageAspectRatio_H->setValidator(&intValidator);
}

//更新界面
void SettingUI::refreshSettingUI()
{
	ui.lineEdit_SampleDirPath->setText(config->SampleDirPath);
	ui.lineEdit_TemplDirPath->setText(config->TemplDirPath);
	ui.lineEdit_OutputDirPath->setText(config->OutputDirPath);

	QString format = config->ImageFormat.toLower();
	if (format == ".bmp") ui.comboBox_ImageFormat->setCurrentText("    *.bmp");
	else if (format == ".jpg") ui.comboBox_ImageFormat->setCurrentText("    *.jpg");
	else if (format == ".png") ui.comboBox_ImageFormat->setCurrentText("    *.png");
	else if (format == ".tif" || format == ".tiff") ui.comboBox_ImageFormat->setCurrentText("    *.tif");

	ui.lineEdit_nCamera->setText(QString::number(config->nCamera));
	ui.lineEdit_nPhotographing->setText(QString::number(config->nPhotographing));
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(config->nBasicUnitInRow));
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(config->nBasicUnitInCol));

	ui.lineEdit_ImageAspectRatio_W->setText(QString::number(config->ImageAspectRatio_W));
	ui.lineEdit_ImageAspectRatio_H->setText(QString::number(config->ImageAspectRatio_H));
}


/***************** 按键响应 *****************/

//选择样本文件夹的路径
void SettingUI::on_pushButton_SampleDirPath_clicked()
{
	selectDirPath(config->SampleDirPath);
	ui.lineEdit_SampleDirPath->setText(config->SampleDirPath);
}

//选择模板文件夹的路径
void SettingUI::on_pushButton_TemplDirPath_clicked()
{
	selectDirPath(config->TemplDirPath);
	ui.lineEdit_TemplDirPath->setText(config->TemplDirPath);
}

//选择输出文件夹的路径
void SettingUI::on_pushButton_OutputDirPath_clicked()
{
	selectDirPath(config->OutputDirPath);
	ui.lineEdit_OutputDirPath->setText(config->OutputDirPath);
}

//确认键
void SettingUI::on_pushButton_confirm_clicked()
{
	//将参数设置界面的确认按键、返回按键设为不可点击
	setPushButtonsToEnabled(false);

	//获取界面上的config参数
	getConfigFromSettingUI();

	//if (config->unequals(tempConfig) != DetectConfig::Index_None) {

	//检查界面上config的有效性
	ConfigErrorCode code = tempConfig.checkValidity(DetectConfig::Index_All);
	if (code != DetectConfig::ValidConfig) { //参数无效，报错
		DetectConfig::showMessageBox(this, code);
		this->setPushButtonsToEnabled(true);
		ConfigIndex index = DetectConfig::convertCodeToIndex(code);
		this->setCursorLocation(index);
		return;
	}

	//设置聚焦位置
	this->setCursorLocation(DetectConfig::Index_None);

	//将临时配置拷贝到config中
	tempConfig.copyTo(*config);

	//将参数保存到config文件中
	QString configFileName = ".config";
	if (!writeConfigFile(configFileName)) setPushButtonsToEnabled(true);
	//writeConfigFile(configFileName);

	//向主界面发送消息
	emit enableButtonsOnDetectMainUI_settingUI(); //将主界面上的按键设为可点击
	if (config->getResetFlag(tempConfig)) emit resetDetectSystem();//判断是否重置检测系统
	this->setPushButtonsToEnabled(true);
}

//返回键
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}

//设置按键的可点击状态
void SettingUI::setPushButtonsToEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);
	ui.pushButton_return->setEnabled(code);
}

/************** comboBox的槽函数 *****************/

void SettingUI::on_currentIndexChanged_imageFormat()
{
	switch (ui.comboBox_ImageFormat->currentIndex())
	{
	case 0:
		tempConfig.ImageFormat = ""; break;
	case 1:
		tempConfig.ImageFormat = ".bmp"; break;
	case 2:
		tempConfig.ImageFormat = ".jpg"; break;
	case 3:
		tempConfig.ImageFormat = ".png"; break;
	case 4:
		tempConfig.ImageFormat = ".tif"; break;
	default:
		break;
	}
}

void SettingUI::getConfigFromSettingUI()
{
	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //样本路径
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //模板路径
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//输出路径
	tempConfig.nCamera = ui.lineEdit_nCamera->text().toInt();//相机个数
	tempConfig.nPhotographing = ui.lineEdit_nPhotographing->text().toInt();//拍照次数
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//每一行中的基本单元数
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//每一列中的基本单元数
	tempConfig.ImageAspectRatio_W = ui.lineEdit_ImageAspectRatio_W->text().toInt();//样本图像的宽高比
	tempConfig.ImageAspectRatio_H = ui.lineEdit_ImageAspectRatio_H->text().toInt();//样本图像的宽高比
	
	ConfigErrorCode code = tempConfig.calcImageAspectRatio();
	if (code != DetectConfig::ValidConfig) 
		DetectConfig::showMessageBox(this, DetectConfig::Invalid_ImageAspectRatio); 
}

/****************** 其他 ******************/

//交互式文件夹路径选择
void SettingUI::selectDirPath(QString &path)
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QString::fromLocal8Bit("请选择存储路径")); //设置文件保存对话框的标题
	fileDialog->setFileMode(QFileDialog::Directory); //设置文件对话框弹出的时候显示文件夹
	fileDialog->setViewMode(QFileDialog::Detail); //文件以详细的形式显示，显示文件名，大小，创建日期等信息
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) { //选择路径
		path = fileDialog->selectedFiles()[0];
	}
	delete fileDialog;
}

//将参数保存到配置文件中
bool SettingUI::writeConfigFile(QString &fileName)
{
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.open(QIODevice::ReadWrite)) { //判断配置文件是否可读可写
		Configurator::init(configFilePath);
	}
	else {
		Configurator configurator(&configFile);
		configurator.jsonSetValue("SampleDirPath", config->SampleDirPath);//样本文件夹
		configurator.jsonSetValue("TemplDirPath", config->TemplDirPath);//模板文件夹
		configurator.jsonSetValue("OutputDirPath", config->OutputDirPath);//输出文件夹
		configurator.jsonSetValue("ImageFormat", config->ImageFormat);//图像格式
		configurator.jsonSetValue("nCamera", QString::number(config->nCamera)); //相机个数
		configurator.jsonSetValue("nPhotographing", QString::number(config->nPhotographing)); //拍照次数
		configurator.jsonSetValue("nBasicUnitInRow", QString::number(config->nBasicUnitInRow)); //每一行中的基本单元数
		configurator.jsonSetValue("nBasicUnitInCol", QString::number(config->nBasicUnitInCol)); //每一列中的基本单元数
		configurator.jsonSetValue("ImageAspectRatio_W", QString::number(config->ImageAspectRatio_W)); //样本图像的宽高比
		configurator.jsonSetValue("ImageAspectRatio_H", QString::number(config->ImageAspectRatio_H)); //样本图像的宽高比
		configurator.jsonSetValue("ImageAspectRatio", QString::number(config->ImageAspectRatio, 'g', 7)); //样本图像的宽高比
		configFile.close();
		return true;
	}
	return false;
}

//设置光标的位置
void SettingUI::setCursorLocation(ConfigIndex code)
{
	int textLen;
	switch (code)
	{
	case Ui::DetectConfig::Index_All:
		break;
	case Ui::DetectConfig::Index_None:
		ui.lineEdit_SampleDirPath->setFocus(); 
		ui.lineEdit_SampleDirPath->clearFocus();
		break;
	case Ui::DetectConfig::Index_SampleDirPath:
		textLen = ui.lineEdit_SampleDirPath->text().length();
		ui.lineEdit_SampleDirPath->setCursorPosition(textLen);
		ui.lineEdit_SampleDirPath->setFocus(); break;
	case Ui::DetectConfig::Index_TemplDirPath:
		ui.lineEdit_TemplDirPath->setFocus(); break;
	case Ui::DetectConfig::Index_OutputDirPath:
		ui.lineEdit_OutputDirPath->setFocus(); break;
	case Ui::DetectConfig::Index_ImageFormat:
		break;
	case Ui::DetectConfig::Index_nCamera:
		ui.lineEdit_nCamera->setFocus(); break;
	case Ui::DetectConfig::Index_nPhotographing:
		ui.lineEdit_nPhotographing->setFocus(); break;
	case Ui::DetectConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case Ui::DetectConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;
	case Ui::DetectConfig::Index_ImageAspectRatio_W:
		ui.lineEdit_ImageAspectRatio_W->setFocus(); break;
	case Ui::DetectConfig::Index_ImageAspectRatio_H:
		ui.lineEdit_ImageAspectRatio_H->setFocus(); break;
	case Ui::DetectConfig::Index_ImageAspectRatio:
		break;
	}
}
