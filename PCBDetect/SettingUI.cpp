#include "SettingUI.h"

using Ui::DetectConfig;


SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//���ý����ʼ��
	initSettingUI();

	//�ۺ���
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imageFormat()));
}

SettingUI::~SettingUI()
{
}


/************* ��������á����� **************/

void SettingUI::initSettingUI()
{
	//���Ʋ��������뷶Χ
	ui.lineEdit_nCamera->setValidator(&intValidator);
	ui.lineEdit_nPhotographing->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInRow->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInCol->setValidator(&intValidator);
}

//���½���
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


/***************** ������Ӧ *****************/

//ѡ�������ļ��е�·��
void SettingUI::on_pushButton_SampleDirPath_clicked()
{
	selectDirPath(config->SampleDirPath);
	ui.lineEdit_SampleDirPath->setText(config->SampleDirPath);
}

//ѡ��ģ���ļ��е�·��
void SettingUI::on_pushButton_TemplDirPath_clicked()
{
	selectDirPath(config->TemplDirPath);
	ui.lineEdit_TemplDirPath->setText(config->TemplDirPath);
}

//ѡ������ļ��е�·��
void SettingUI::on_pushButton_OutputDirPath_clicked()
{
	selectDirPath(config->OutputDirPath);
	ui.lineEdit_OutputDirPath->setText(config->OutputDirPath);
}

//ȷ��
void SettingUI::on_pushButton_confirm_clicked()
{
	//����·��
	QString SampleDirPath = ui.lineEdit_SampleDirPath->text();
	if (!(QFileInfo(config->SampleDirPath).isDir())) {
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("����·����Ч������������!  \nConfigCode: "),
			QString::fromLocal8Bit("ȷ��"));
		return;
	}
	config->SampleDirPath = SampleDirPath;

	//ģ��·��
	QString TemplDirPath = ui.lineEdit_TemplDirPath->text();
	if (!(QFileInfo(config->TemplDirPath).isDir())) {
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("ģ��·����Ч������������!  \nConfigCode: "),
			QString::fromLocal8Bit("ȷ��"));
		return;
	}
	config->TemplDirPath = TemplDirPath;
	
	//���·��
	QString OutputDirPath = ui.lineEdit_OutputDirPath->text();
	if (!(QFileInfo(OutputDirPath).isDir())) {
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���·����Ч������������!  \nConfigCode: "),
			QString::fromLocal8Bit("ȷ��"));
		return;
	}
	config->OutputDirPath = OutputDirPath;

	//����ͼ��ĳߴ�
	//QSize imageSize; 
	
	//�������
	int nCamera = ui.lineEdit_nCamera->text().toInt();
	config->nCamera = nCamera;

	//���մ���
	int nPhotographing = ui.lineEdit_nPhotographing->text().toInt();
	config->nPhotographing = nPhotographing;

	//ÿһ���еĻ�����Ԫ��
	int nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();
	config->nBasicUnitInRow = nBasicUnitInRow;

	//ÿһ���еĻ�����Ԫ��
	int nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();
	config->nBasicUnitInCol = nBasicUnitInCol;


	//���������浽config�ļ���
	QString configFileName = ".config";
	writeConfigFile(configFileName);

}

//����
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


/****************** ���� ******************/

void SettingUI::selectDirPath(QString &path)
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QString::fromLocal8Bit("��ѡ��洢·��")); //�����ļ�����Ի���ı���
	fileDialog->setFileMode(QFileDialog::Directory); //�����ļ��Ի��򵯳���ʱ����ʾ�ļ���
	fileDialog->setViewMode(QFileDialog::Detail); //�ļ�����ϸ����ʽ��ʾ����ʾ�ļ�������С���������ڵ���Ϣ
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) { //ѡ��·��
		path = fileDialog->selectedFiles()[0];
	}
	delete fileDialog;
}


//���������浽�����ļ���
void SettingUI::writeConfigFile(QString &fileName)
{
	QString configFilePath = QDir::currentPath() + "/" + fileName;
	QFile configFile(configFilePath);
	if (!configFile.open(QIODevice::ReadWrite)) { //�ж������ļ��Ƿ�ɶ���д
		Configurator::init(configFilePath);
	}
	else {
		Configurator configurator(&configFile);
		configurator.jsonSetValue("SampleDirPath", config->SampleDirPath);//�����ļ���
		configurator.jsonSetValue("TemplDirPath", config->TemplDirPath);//ģ���ļ���
		configurator.jsonSetValue("OutputDirPath", config->OutputDirPath);//����ļ���
		configurator.jsonSetValue("ImageFormat", config->ImageFormat);//ͼ���ʽ
		configurator.jsonSetValue("nCamera", QString::number(config->nCamera)); //�������
		configurator.jsonSetValue("nPhotographing", QString::number(config->nPhotographing)); //���մ���
		configurator.jsonSetValue("nBasicUnitInRow", QString::number(config->nBasicUnitInRow)); //ÿһ���еĻ�����Ԫ��
		configurator.jsonSetValue("nBasicUnitInCol", QString::number(config->nBasicUnitInCol)); //ÿһ���еĻ�����Ԫ��
		configurator.jsonSetValue("imageAspectRatio", QString::number(config->imageAspectRatio)); //����ͼ��Ŀ�߱�
		configFile.close();
	}
}
