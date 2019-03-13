#include "SettingUI.h"

using Ui::DetectConfig;


SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//设置界面初始化
	initSettingUI();

	//槽函数
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imageFormat()));
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
}

//更新界面
void SettingUI::refreshSettingUI()
{
	ui.lineEdit_SampleDirPath->setText(config->SampleDirPath);
	ui.lineEdit_TemplDirPath->setText(config->TemplDirPath);
	ui.lineEdit_OutputDirPath->setText(config->OutputDirPath);

	QString format = config->ImageFormat;
	if (format == ".bmp" || format == ".BMP") ui.comboBox_ImageFormat->setCurrentText("    *.bmp");
	else if (format == ".jpg" || format == ".JPG") ui.comboBox_ImageFormat->setCurrentText("    *.jpg");

	ui.lineEdit_nCamera->setText(QString::number(config->nCamera));
	ui.lineEdit_nPhotographing->setText(QString::number(config->nPhotographing));
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(config->nBasicUnitInRow));
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(config->nBasicUnitInCol));
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

//确认
void SettingUI::on_pushButton_confirm_clicked()
{
	//样本路径
	QString SampleDirPath = ui.lineEdit_SampleDirPath->text();
	if (!(QFileInfo(config->SampleDirPath).isDir())) {
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("样本路径无效，请重新设置!  \nConfigCode: "),
			QString::fromLocal8Bit("确定"));
		return;
	}
	config->SampleDirPath = SampleDirPath;

	//模板路径
	QString TemplDirPath = ui.lineEdit_TemplDirPath->text();
	if (!(QFileInfo(config->TemplDirPath).isDir())) {
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("模板路径无效，请重新设置!  \nConfigCode: "),
			QString::fromLocal8Bit("确定"));
		return;
	}
	config->TemplDirPath = TemplDirPath;
	
	//输出路径
	QString OutputDirPath = ui.lineEdit_OutputDirPath->text();
	if (!(QFileInfo(OutputDirPath).isDir())) {
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("输出路径无效，请重新设置!  \nConfigCode: "),
			QString::fromLocal8Bit("确定"));
		return;
	}
	config->OutputDirPath = OutputDirPath;

	//样本图像的尺寸
	//QSize imageSize; 
	
	//相机个数
	int nCamera = ui.lineEdit_nCamera->text().toInt();
	config->nCamera = nCamera;

	//拍照次数
	int nPhotographing = ui.lineEdit_nPhotographing->text().toInt();
	config->nPhotographing = nPhotographing;

	//每一行中的基本单元数
	int nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();
	config->nBasicUnitInRow = nBasicUnitInRow;

	//每一列中的基本单元数
	int nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();
	config->nBasicUnitInCol = nBasicUnitInCol;


	//将参数保存到config文件中
	QString configFileName = ".config";
	writeConfigFile(configFileName);

}

//返回
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}


/******************************************/

void SettingUI::on_currentIndexChanged_imageFormat()
{
	switch (ui.comboBox_ImageFormat->currentIndex())
	{
	case 0:
		config->ImageFormat = "";
	case 1:
		config->ImageFormat = ".bmp";
	case 2:
		config->ImageFormat = ".jpg";
	default:
		break;
	}
}


/****************** 其他 ******************/

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
void SettingUI::writeConfigFile(QString &fileName)
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
		configurator.jsonSetValue("imageAspectRatio", QString::number(config->imageAspectRatio)); //样本图像的宽高比
		configFile.close();
	}
}
