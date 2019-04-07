#include "SettingUI.h"

using pcb::DetectConfig;
using pcb::Configurator;

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
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imgFormat()));

	//ϵͳ�������õĵ�¼����
	passWordUI.setWindowFlags(passWordUI.windowFlags() | Qt::Dialog);
	passWordUI.setWindowModality(Qt::ApplicationModal);
	connect(&passWordUI, SIGNAL(showAdminSettingUI_pswdUI()), this, SLOT(do_showAdminSettingUI_pswdUI()));
	connect(&passWordUI, SIGNAL(closePassWordUI_pswdUI()), this, SLOT(on_closePassWordUI_pswdUI()));

	//ϵͳ�������ý���
	adminSettingUI.setAdminConfig(adminConfig);
	connect(&adminSettingUI, SIGNAL(showSettingUI_adminUI()), this, SLOT(do_showSettingUI_adminUI()));
	connect(&adminSettingUI, SIGNAL(resetDetectSystem_adminUI()), this, SLOT(do_resetDetectSystem_adminUI()));
	connect(&adminSettingUI, SIGNAL(checkSystemWorkingState_adminUI()), this, SLOT(do_checkSystemWorkingState_adminUI()));
}

SettingUI::~SettingUI()
{
}


/************* ��������á����롢���� **************/

void SettingUI::initSettingUI()
{
	//��һ���л�����״̬����ʾ�Ͽ��ܻ�����ӳ٣�����ǰԤ��
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//���Ʋ��������뷶Χ
	QIntValidator intValidator;
	ui.lineEdit_ActualProductSize_W->setValidator(&intValidator);
	ui.lineEdit_ActualProductSize_H->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInRow->setValidator(&intValidator);
	ui.lineEdit_nBasicUnitInCol->setValidator(&intValidator);
}

//���½���
void SettingUI::refreshSettingUI()
{
	ui.lineEdit_SampleDirPath->setText(detectConfig->SampleDirPath);
	ui.lineEdit_TemplDirPath->setText(detectConfig->TemplDirPath);
	ui.lineEdit_OutputDirPath->setText(detectConfig->OutputDirPath);

	QString format = detectConfig->ImageFormat.toLower();
	if (format == ".bmp") ui.comboBox_ImageFormat->setCurrentText("    *.bmp");
	else if (format == ".jpg") ui.comboBox_ImageFormat->setCurrentText("    *.jpg");
	else if (format == ".png") ui.comboBox_ImageFormat->setCurrentText("    *.png");
	else if (format == ".tif" || format == ".tiff") ui.comboBox_ImageFormat->setCurrentText("    *.tif");

	ui.lineEdit_ActualProductSize_W->setText(QString::number(detectConfig->ActualProductSize_W));
	ui.lineEdit_ActualProductSize_H->setText(QString::number(detectConfig->ActualProductSize_H));
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(detectConfig->nBasicUnitInRow));
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(detectConfig->nBasicUnitInCol));
}

//���ù���λ��
void SettingUI::setCursorLocation(DetectConfig::ConfigIndex code)
{
	int textLen;
	switch (code)
	{
	case pcb::DetectConfig::Index_All:
		break;
	case pcb::DetectConfig::Index_None:
		ui.lineEdit_SampleDirPath->setFocus(); 
		ui.lineEdit_SampleDirPath->clearFocus();
		break;
	case pcb::DetectConfig::Index_SampleDirPath:
		textLen = ui.lineEdit_SampleDirPath->text().length();
		ui.lineEdit_SampleDirPath->setCursorPosition(textLen);
		ui.lineEdit_SampleDirPath->setFocus(); break;
	case pcb::DetectConfig::Index_TemplDirPath:
		ui.lineEdit_TemplDirPath->setFocus(); break;
	case pcb::DetectConfig::Index_OutputDirPath:
		ui.lineEdit_OutputDirPath->setFocus(); break;
	case pcb::DetectConfig::Index_ImageFormat:
		break;
	case pcb::DetectConfig::Index_ActualProductSize_W:
		ui.lineEdit_ActualProductSize_W->setFocus(); break;
	case pcb::DetectConfig::Index_ActualProductSize_H:
		ui.lineEdit_ActualProductSize_H->setFocus(); break;
	case pcb::DetectConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case pcb::DetectConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;
	}
}


/***************** ������Ӧ *****************/

//ѡ�������ļ��е�·��
void SettingUI::on_pushButton_SampleDirPath_clicked()
{
	detectConfig->SampleDirPath = pcb::selectDirPath();
	ui.lineEdit_SampleDirPath->setText(detectConfig->SampleDirPath);
}

//ѡ��ģ���ļ��е�·��
void SettingUI::on_pushButton_TemplDirPath_clicked()
{
	detectConfig->TemplDirPath = pcb::selectDirPath();
	ui.lineEdit_TemplDirPath->setText(detectConfig->TemplDirPath);
}

//ѡ������ļ��е�·��
void SettingUI::on_pushButton_OutputDirPath_clicked()
{
	detectConfig->OutputDirPath = pcb::selectDirPath();
	ui.lineEdit_OutputDirPath->setText(detectConfig->OutputDirPath);
}

//ȷ�ϼ�
void SettingUI::on_pushButton_confirm_clicked()
{
	//���������ý����ȷ�ϰ��������ذ�����Ϊ���ɵ��
	this->setPushButtonsToEnabled(false);

	//��ȡ�����ϵ�config����
	getConfigFromSettingUI();

	//��������config����Ч��
	DetectConfig::ErrorCode code = tempConfig.checkValidity(DetectConfig::Index_All);
	if (code != DetectConfig::ValidConfig) { //������Ч�򱨴�
		tempConfig.showMessageBox(this); //��������
		this->setPushButtonsToEnabled(true);//��������Ϊ�ɵ��
		DetectConfig::ConfigIndex index = DetectConfig::convertCodeToIndex(code);
		this->setCursorLocation(index);//����궨λ����Ч�������������
		return;
	}

	//���þ۽�λ��
	this->setCursorLocation(DetectConfig::Index_None);

	//�ж��Ƿ����ü��ϵͳ
	int resetCode = detectConfig->getSystemResetCode(tempConfig); 

	//����ʱ���ÿ�����detectConfig��
	tempConfig.copyTo(detectConfig);

	//����ϵͳ
	resetCode |= detectParams->updateGridSize(adminConfig, tempConfig);
	emit resetDetectSystem_settingUI(resetCode); //�ж��Ƿ����ü��ϵͳ

	//���������浽�����ļ���
	Configurator::saveConfigFile(configFileName, detectConfig);

	//�������淢����Ϣ
	emit checkSystemWorkingState_settingUI(); //���ϵͳ�Ĺ���״̬
	pcb::delay(100);
	
	//���������ϵİ�����Ϊ�ɵ��
	this->setPushButtonsToEnabled(true);
}

//���ؼ�
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
}

//ϵͳ��������
void SettingUI::on_pushButton_admin_clicked()
{
	//���ô���ʼ���ö�
	passWordUI.show();
	//���ð���
	this->setPushButtonsToEnabled(false);
}

//���ð����Ŀɵ��״̬
void SettingUI::setPushButtonsToEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);//ȷ��
	ui.pushButton_return->setEnabled(code);//����
	ui.pushButton_admin->setEnabled(code);//ϵͳ����
}

/************** ��ȡ�����ϵĲ��� *****************/

void SettingUI::on_currentIndexChanged_imgFormat()
{
	switch ((pcb::ImageFormat) ui.comboBox_ImageFormat->currentIndex())
	{
	case pcb::ImageFormat::Unknown:
		tempConfig.ImageFormat = ""; break;
	case pcb::ImageFormat::BMP:
		tempConfig.ImageFormat = ".bmp"; break;
	case pcb::ImageFormat::JPG:
		tempConfig.ImageFormat = ".jpg"; break;
	case pcb::ImageFormat::PNG:
		tempConfig.ImageFormat = ".png"; break;
	case pcb::ImageFormat::TIF:
		tempConfig.ImageFormat = ".tif"; break;
	default:
		break;
	}
}

//�����ý����ϻ�ȡ����
void SettingUI::getConfigFromSettingUI()
{
	tempConfig.resetErrorCode(); //���ô������

	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //����·��
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //ģ��·��
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//���·��

	tempConfig.ActualProductSize_W = ui.lineEdit_ActualProductSize_W->text().toInt();
	tempConfig.ActualProductSize_H = ui.lineEdit_ActualProductSize_H->text().toInt();
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//ÿһ���еĻ�����Ԫ��
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//ÿһ���еĻ�����Ԫ��
}


/****************** ϵͳ������¼���� *******************/

void SettingUI::on_closePassWordUI_pswdUI()
{
	//��ϵͳ����������Ϊ�ɵ��
	this->setPushButtonsToEnabled(true);//���ð���
}

//��ʾϵͳ�������ý���
void SettingUI::do_showAdminSettingUI_pswdUI()
{
	adminSettingUI.setAdminConfig(adminConfig);
	adminSettingUI.refreshAdminSettingUI();
	pcb::delay(10);
	adminSettingUI.showFullScreen();
	pcb::delay(10);
	this->hide(); //���ز������ý���
	this->setPushButtonsToEnabled(true);//���ð���
}


/****************** ϵͳ�������ý��� *******************/

//��ϵͳ�������ý��淵�ز������ý���
void SettingUI::do_showSettingUI_adminSettingUI()
{
	this->showFullScreen();
	pcb::delay(10);
	adminSettingUI.hide();
}

//ת����ϵͳ���ý��淢���������ź�
void SettingUI::do_resetDetectSystem_adminUI(int code)
{
	emit resetDetectSystem_settingUI(code);
}

//ת����ϵͳ���ý��淢����ϵͳ״̬����ź�
void SettingUI::do_checkSystemWorkingState_adminUI()
{
	emit checkSystemWorkingState_settingUI(code);
}
