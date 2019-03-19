#include "SettingUI.h"

using Ui::DetectConfig;

typedef Ui::DetectConfig::ErrorCode ConfigErrorCode;
typedef Ui::DetectConfig::ConfigIndex ConfigIndex;

SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//���ý����ʼ��
	this->setFocusPolicy(Qt::ClickFocus);
	initSettingUI();

	//����������Ĳۺ�������
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imageFormat()));
	//connect(ui.comboBox_imageAspectRatio, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_aspectRatio()));
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
	ui.lineEdit_ImageAspectRatio_W->setValidator(&intValidator);
	ui.lineEdit_ImageAspectRatio_H->setValidator(&intValidator);
}

//���½���
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

//ȷ�ϼ�
void SettingUI::on_pushButton_confirm_clicked()
{
	//���������ý����ȷ�ϰ��������ذ�����Ϊ���ɵ��
	setPushButtonsToEnabled(false);

	//��ȡ�����ϵ�config����
	getConfigFromSettingUI();

	//if (config->unequals(tempConfig) != DetectConfig::Index_None) {

	//��������config����Ч��
	ConfigErrorCode code = tempConfig.checkValidity(DetectConfig::Index_All);
	if (code != DetectConfig::ValidConfig) { //������Ч������
		DetectConfig::showMessageBox(this, code);
		this->setPushButtonsToEnabled(true);
		ConfigIndex index = DetectConfig::convertCodeToIndex(code);
		this->setCursorLocation(index);
		return;
	}

	//���þ۽�λ��
	this->setCursorLocation(DetectConfig::Index_None);

	//����ʱ���ÿ�����config��
	tempConfig.copyTo(*config);

	//���������浽config�ļ���
	QString configFileName = ".config";
	if (!writeConfigFile(configFileName)) setPushButtonsToEnabled(true);
	//writeConfigFile(configFileName);

	//�������淢����Ϣ
	emit enableButtonsOnDetectMainUI_settingUI(); //���������ϵİ�����Ϊ�ɵ��
	if (config->getResetFlag(tempConfig)) emit resetDetectSystem();//�ж��Ƿ����ü��ϵͳ
	this->setPushButtonsToEnabled(true);
}

//���ؼ�
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}

//���ð����Ŀɵ��״̬
void SettingUI::setPushButtonsToEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);
	ui.pushButton_return->setEnabled(code);
}

/************** comboBox�Ĳۺ��� *****************/

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
	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //����·��
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //ģ��·��
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//���·��
	tempConfig.nCamera = ui.lineEdit_nCamera->text().toInt();//�������
	tempConfig.nPhotographing = ui.lineEdit_nPhotographing->text().toInt();//���մ���
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//ÿһ���еĻ�����Ԫ��
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//ÿһ���еĻ�����Ԫ��
	tempConfig.ImageAspectRatio_W = ui.lineEdit_ImageAspectRatio_W->text().toInt();//����ͼ��Ŀ�߱�
	tempConfig.ImageAspectRatio_H = ui.lineEdit_ImageAspectRatio_H->text().toInt();//����ͼ��Ŀ�߱�
	
	ConfigErrorCode code = tempConfig.calcImageAspectRatio();
	if (code != DetectConfig::ValidConfig) 
		DetectConfig::showMessageBox(this, DetectConfig::Invalid_ImageAspectRatio); 
}

/****************** ���� ******************/

//����ʽ�ļ���·��ѡ��
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
bool SettingUI::writeConfigFile(QString &fileName)
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
		configurator.jsonSetValue("ImageAspectRatio_W", QString::number(config->ImageAspectRatio_W)); //����ͼ��Ŀ�߱�
		configurator.jsonSetValue("ImageAspectRatio_H", QString::number(config->ImageAspectRatio_H)); //����ͼ��Ŀ�߱�
		configurator.jsonSetValue("ImageAspectRatio", QString::number(config->ImageAspectRatio, 'g', 7)); //����ͼ��Ŀ�߱�
		configFile.close();
		return true;
	}
	return false;
}

//���ù���λ��
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
