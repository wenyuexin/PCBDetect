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
	ui.lineEdit_SampleDirPath->setText(detectConfig->SampleDirPath);
	ui.lineEdit_TemplDirPath->setText(detectConfig->TemplDirPath);
	ui.lineEdit_OutputDirPath->setText(detectConfig->OutputDirPath);

	QString format = detectConfig->ImageFormat.toLower();
	if (format == ".bmp") ui.comboBox_ImageFormat->setCurrentText("    *.bmp");
	else if (format == ".jpg") ui.comboBox_ImageFormat->setCurrentText("    *.jpg");
	else if (format == ".png") ui.comboBox_ImageFormat->setCurrentText("    *.png");
	else if (format == ".tif" || format == ".tiff") ui.comboBox_ImageFormat->setCurrentText("    *.tif");

	ui.lineEdit_nCamera->setText(QString::number(detectConfig->nCamera));
	ui.lineEdit_nPhotographing->setText(QString::number(detectConfig->nPhotographing));
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(detectConfig->nBasicUnitInRow));
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(detectConfig->nBasicUnitInCol));

	ui.lineEdit_ImageAspectRatio_W->setText(QString::number(detectConfig->ImageAspectRatio_W));
	ui.lineEdit_ImageAspectRatio_H->setText(QString::number(detectConfig->ImageAspectRatio_H));
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
	case pcb::DetectConfig::Index_nCamera:
		ui.lineEdit_nCamera->setFocus(); break;
	case pcb::DetectConfig::Index_nPhotographing:
		ui.lineEdit_nPhotographing->setFocus(); break;
	case pcb::DetectConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case pcb::DetectConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;
	case pcb::DetectConfig::Index_ImageAspectRatio_W:
		ui.lineEdit_ImageAspectRatio_W->setFocus(); break;
	case pcb::DetectConfig::Index_ImageAspectRatio_H:
		ui.lineEdit_ImageAspectRatio_H->setFocus(); break;
	case pcb::DetectConfig::Index_ImageAspectRatio:
		break;
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
	setPushButtonsToEnabled(false);

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
	emit resetDetectSystem_adminUI(resetCode); //�ж��Ƿ����ü��ϵͳ

	//���������浽�����ļ���
	Configurator::saveConfigFile(configFileName, detectConfig);

	//�������淢����Ϣ
	emit checkSystemWorkingState_adminUI(); //���ϵͳ�Ĺ���״̬

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

//�����ý����ϻ�ȡ����
void SettingUI::getConfigFromSettingUI()
{
	tempConfig.resetErrorCode(); //���ô������

	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //����·��
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //ģ��·��
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//���·��
	tempConfig.nCamera = ui.lineEdit_nCamera->text().toInt();//�������
	tempConfig.nPhotographing = ui.lineEdit_nPhotographing->text().toInt();//���մ���
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//ÿһ���еĻ�����Ԫ��
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//ÿһ���еĻ�����Ԫ��
	tempConfig.ImageAspectRatio_W = ui.lineEdit_ImageAspectRatio_W->text().toInt();//����ͼ��Ŀ�߱�
	tempConfig.ImageAspectRatio_H = ui.lineEdit_ImageAspectRatio_H->text().toInt();//����ͼ��Ŀ�߱�
	
	DetectConfig::ErrorCode code = tempConfig.calcImageAspectRatio();
	if (code != DetectConfig::ValidConfig) 
		detectConfig->showMessageBox(this, code); 
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
