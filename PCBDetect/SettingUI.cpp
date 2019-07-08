#include "SettingUI.h"

using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::Configurator;
using std::vector;


SettingUI::SettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//��Ա������ʼ��
	runtimeParams = Q_NULLPTR;//���в���
	userConfig = Q_NULLPTR; //�û�����
	adminConfig = Q_NULLPTR; //ϵͳ����
	adminSettingUI = Q_NULLPTR; //ϵͳ���ý���
	sysResetCode = 0b00000000; //ϵͳ���ô���
	NumberValidator = Q_NULLPTR;
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
	NumberValidator2 = new QRegExpValidator(QRegExp("[/.0-9]+$"));
}

void SettingUI::init()
{
	//����״̬��ѡ�����������Ǹ�������ʾ
	this->setGeometry(runtimeParams->ScreenRect);

	//���þ۽�����
	this->setFocusPolicy(Qt::ClickFocus);

	//���ù��
	this->setCursorLocation(UserConfig::Index_None);

	//��һ���л�����״̬����ʾ�Ͽ��ܻ�����ӳ٣�����ǰԤ��
	this->setPushButtonsEnabled(false);
	this->setPushButtonsEnabled(true);

	//���Ʋ��������뷶Χ
	ui.lineEdit_ActualProductSize_W->setValidator(NumberValidator);
	ui.lineEdit_ActualProductSize_H->setValidator(NumberValidator);
	ui.lineEdit_nBasicUnitInRow->setValidator(NumberValidator);
	ui.lineEdit_nBasicUnitInCol->setValidator(NumberValidator);
	ui.lineEdit_concaveRateThresh->setValidator(NumberValidator);
	ui.lineEdit_convexRateThresh->setValidator(NumberValidator);
	ui.lineEdit_exposureTime_camera->setValidator(NumberValidator);
	ui.lineEdit_inetAddressOfRecheckPC->setValidator(NumberValidator2);

	//����ع�ʱ���ȷ�Ͽ���Ϊһ��
	CheckBoxGroup_colorMode.addButton(ui.lineEdit_colorMode_rgb_camera, 0);
	CheckBoxGroup_colorMode.addButton(ui.lineEdit_colorMode_gray_camera, 1);

	//��ƥ�侫�ȵ�ȷ�Ͽ���Ϊһ��
	QPushButton button;
	checkBoxGroup_matchingAccuracy.addButton(&button);
	checkBoxGroup_matchingAccuracy.addButton(ui.checkBox_matchingAccuracy_high, 1);
	checkBoxGroup_matchingAccuracy.addButton(ui.checkBox_matchingAccuracy_low, 2);

	//����������Ĳۺ�������
	connect(ui.comboBox_ImageFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_imgFormat()));
	connect(ui.comboBox_clusterComPort, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_currentIndexChanged_comPort()));

	//ϵͳ�������õĵ�¼����
	passWordUI.setWindowFlags(passWordUI.windowFlags() | Qt::Dialog);
	passWordUI.setWindowModality(Qt::ApplicationModal);
	connect(&passWordUI, SIGNAL(showAdminSettingUI_pswdUI()), this, SLOT(do_showAdminSettingUI_pswdUI()));
	connect(&passWordUI, SIGNAL(closePassWordUI_pswdUI()), this, SLOT(on_closePassWordUI_pswdUI()));

	//ϵͳ�������ý���
	adminSettingUI = new AdminSettingUI();
	adminSettingUI->setAdminConfig(adminConfig);
	adminSettingUI->setUserConfig(userConfig);
	adminSettingUI->setRuntimeParams(runtimeParams);
	adminSettingUI->init();
	connect(adminSettingUI, SIGNAL(showSettingUI_adminUI()), this, SLOT(do_showSettingUI_adminUI()));
	connect(adminSettingUI, SIGNAL(resetDetectSystem_adminUI()), this, SLOT(do_resetDetectSystem_adminUI()));
	connect(adminSettingUI, SIGNAL(checkSystemState_adminUI()), this, SLOT(do_checkSystemState_adminUI()));
}

SettingUI::~SettingUI()
{
	qDebug() << "~SettingUI";
	delete adminSettingUI; 
	adminSettingUI = Q_NULLPTR;
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
}


/************* ��������á����롢���� **************/

//���½���
void SettingUI::refresh()
{
	//��������
	ui.lineEdit_SampleDirPath->setText(userConfig->SampleDirPath); //����·��
	ui.lineEdit_TemplDirPath->setText(userConfig->TemplDirPath); //ģ��·��
	ui.lineEdit_OutputDirPath->setText(userConfig->OutputDirPath); //���·��

	QString comPort = userConfig->clusterComPort.toUpper(); //�˶����ƴ��ں�
	QString headBlankChars = "    "; //��ͷ�Ŀ��ַ���
	ui.comboBox_clusterComPort->setCurrentText(headBlankChars + " " + comPort);

	QString imgFormat = userConfig->ImageFormat.toLower(); //ͼ���ʽ
	if (imgFormat == ".tiff") imgFormat = ".tif";
	ui.comboBox_ImageFormat->setCurrentText(headBlankChars + "*" + imgFormat);

	ui.lineEdit_ActualProductSize_W->setText(QString::number(userConfig->ActualProductSize_W));//��Ʒ�ߴ�
	ui.lineEdit_ActualProductSize_H->setText(QString::number(userConfig->ActualProductSize_H));//��Ʒ�ߴ�
	ui.lineEdit_nBasicUnitInRow->setText(QString::number(userConfig->nBasicUnitInRow));//������Ԫ��
	ui.lineEdit_nBasicUnitInCol->setText(QString::number(userConfig->nBasicUnitInCol));//������Ԫ��

	//�˶�����
	ui.pushButton_reset_motion->setEnabled(motionControler->isReady());

	//�����
	ui.lineEdit_exposureTime_camera->setText(QString::number(userConfig->exposureTime));//�ع�ʱ��

	if (userConfig->colorMode == 0) //ɫ��ģʽ
		ui.lineEdit_colorMode_rgb_camera->setChecked(true); //RGB��ɫ
	else if (userConfig->matchingAccuracyLevel == 1) {
		ui.lineEdit_colorMode_gray_camera->setChecked(true); //�ڰ�
	}

	//����㷨
	vector<bool> vec = userConfig->defectTypeToBeProcessed; //������ȱ������
	ui.checkBox_defectType_short->setChecked(true);
	ui.checkBox_defectType_short->setDisabled(true);
	ui.checkBox_defectType_break->setChecked(true);
	ui.checkBox_defectType_break->setDisabled(true);
	ui.checkBox_defectType_convex->setChecked(vec[2]);
	ui.checkBox_defectType_concave->setChecked(vec[3]);

	if (userConfig->matchingAccuracyLevel == 1) //ƥ�侫�ȵȼ�
		ui.checkBox_matchingAccuracy_high->setChecked(true);
	else if (userConfig->matchingAccuracyLevel == 2) {
		ui.checkBox_matchingAccuracy_low->setChecked(true);
	}

	ui.lineEdit_concaveRateThresh->setText(QString::number(userConfig->concaveRateThresh)); //ȱʧ����ֵ
	ui.lineEdit_convexRateThresh->setText(QString::number(userConfig->convexRateThresh)); //͹������ֵ

	//�ļ�����
	ui.lineEdit_inetAddressOfRecheckPC->setText(userConfig->inetAddressOfRecheckPC); //�����豸��IP
}

//���ù���λ��
void SettingUI::setCursorLocation(UserConfig::ConfigIndex code)
{
	int textLen;
	switch (code)
	{
	case pcb::UserConfig::Index_All:
	case pcb::UserConfig::Index_None:
		ui.lineEdit_SampleDirPath->setFocus(); 
		ui.lineEdit_SampleDirPath->clearFocus(); break;

	//��������
	case pcb::UserConfig::Index_SampleDirPath:
		textLen = ui.lineEdit_SampleDirPath->text().length();
		ui.lineEdit_SampleDirPath->setCursorPosition(textLen);
		ui.lineEdit_SampleDirPath->setFocus(); break;
	case pcb::UserConfig::Index_TemplDirPath:
		ui.lineEdit_TemplDirPath->setFocus(); break;
	case pcb::UserConfig::Index_OutputDirPath:
		ui.lineEdit_OutputDirPath->setFocus(); break;
	case pcb::UserConfig::Index_ImageFormat:
		ui.comboBox_ImageFormat->setFocus(); break;
	
	case pcb::UserConfig::Index_ActualProductSize_W:
		ui.lineEdit_ActualProductSize_W->setFocus(); break;
	case pcb::UserConfig::Index_ActualProductSize_H:
		ui.lineEdit_ActualProductSize_H->setFocus(); break;
	case pcb::UserConfig::Index_nBasicUnitInRow:
		ui.lineEdit_nBasicUnitInRow->setFocus(); break;
	case pcb::UserConfig::Index_nBasicUnitInCol:
		ui.lineEdit_nBasicUnitInCol->setFocus(); break;

	//�˶��ṹ
	case pcb::UserConfig::Index_clusterComPort:
		ui.comboBox_clusterComPort->setFocus(); break;

	//�����
	case pcb::UserConfig::Index_exposureTime:
		ui.lineEdit_exposureTime_camera->setFocus(); break;
	case pcb::UserConfig::Index_colorMode:
		ui.lineEdit_colorMode_rgb_camera->setFocus(); break;

	//����㷨
	case pcb::UserConfig::Index_defectTypeToBeProcessed:
		ui.checkBox_defectType_short->setFocus(); break;
	case pcb::UserConfig::Index_matchingAccuracyLevel:
		ui.checkBox_matchingAccuracy_high->setFocus(); break;
	case pcb::UserConfig::Index_concaveRateThresh:
		ui.lineEdit_concaveRateThresh->setFocus(); break;
	case pcb::UserConfig::Index_convexRateThresh:
		ui.lineEdit_convexRateThresh->setFocus(); break;

	//�ļ�����
	case pcb::UserConfig::Index_inetAddressOfRecheckPC:
		ui.lineEdit_inetAddressOfRecheckPC->setFocus(); break;
	}
}

//���ð����Ŀɵ��״̬
void SettingUI::setPushButtonsEnabled(bool code)
{
	ui.pushButton_SampleDirPath->setEnabled(code);
	ui.pushButton_TemplDirPath->setEnabled(code);
	ui.pushButton_OutputDirPath->setEnabled(code);

	ui.pushButton_initAndReturnToZero_motion->setEnabled(code);
	ui.pushButton_reset_motion->setEnabled(code && motionControler->isReady());

	ui.pushButton_confirm->setEnabled(code);//ȷ��
	ui.pushButton_return->setEnabled(code);//����
	ui.pushButton_admin->setEnabled(code);//ϵͳ����
}


/***************** ������������ *****************/

//ѡ�������ļ��е�·��
void SettingUI::on_pushButton_SampleDirPath_clicked()
{
	userConfig->SampleDirPath = pcb::selectDirPath(this);
	ui.lineEdit_SampleDirPath->setText(userConfig->SampleDirPath);
}

//ѡ��ģ���ļ��е�·��
void SettingUI::on_pushButton_TemplDirPath_clicked()
{
	userConfig->TemplDirPath = pcb::selectDirPath(this);
	ui.lineEdit_TemplDirPath->setText(userConfig->TemplDirPath);
}

//ѡ������ļ��е�·��
void SettingUI::on_pushButton_OutputDirPath_clicked()
{
	userConfig->OutputDirPath = pcb::selectDirPath(this);
	ui.lineEdit_OutputDirPath->setText(userConfig->OutputDirPath);
}


/***************** �˶����� *****************/

//�����˶��ṹ
void SettingUI::on_pushButton_initAndReturnToZero_motion_clicked()
{
	//���ý����ϵİ����������
	this->setPushButtonsEnabled(false);

	//�����˶�������
	motionControler->setOperation(MotionControler::ResetControler);
	motionControler->start();
	while (motionControler->isRunning()) pcb::delay(50);

	//���ý����ϵİ����������
	this->setPushButtonsEnabled(true);
}

//�˶��ṹ��λ
void SettingUI::on_pushButton_reset_motion_clicked()
{
	//���ý����ϵİ����������
	this->setPushButtonsEnabled(false);

	//�˶���λ
	motionControler->setOperation(MotionControler::MotionReset);
	motionControler->start();
	while (motionControler->isRunning()) pcb::delay(50);

	//���ý����ϵİ����������
	this->setPushButtonsEnabled(true);
}


/***************** ȷ�ϼ������ؼ� *****************/

//ȷ�ϼ�
void SettingUI::on_pushButton_confirm_clicked()
{
	//���������ý����ȷ�ϰ��������ذ�����Ϊ���ɵ��
	this->setPushButtonsEnabled(false);

	//��ȡ�����ϵ�config����
	getConfigFromSettingUI();

	//��������config����Ч��
	UserConfig::ErrorCode code;
	code = tempConfig.checkValidity(UserConfig::Index_All, adminConfig);
	if (code != UserConfig::ValidConfig) { //������Ч�򱨴�
		tempConfig.showMessageBox(this); //��������
		this->setPushButtonsEnabled(true); //��������Ϊ�ɵ��
		UserConfig::ConfigIndex index = UserConfig::convertCodeToIndex(code);
		this->setCursorLocation(index); //����궨λ����Ч�������������
		return;
	}

	//���þ۽�λ��
	this->setCursorLocation(UserConfig::Index_None);

	//�жϲ����Ƿ��Ѿ��޸�
	sysResetCode = 0b00000000; //ϵͳ���ô���
	if (userConfig->unequals(tempConfig) != UserConfig::Index_None) {
		//�ж��Ƿ����ü��ϵͳ
		sysResetCode |= userConfig->getSystemResetCode(tempConfig);
		//����ʱ���ÿ�����userConfig��
		tempConfig.copyTo(userConfig);
		//���������浽�����ļ���
		Configurator::saveConfigFile(configFileName, userConfig);

		//�������в���
		if (adminConfig->isValid(false)) {
			runtimeParams->copyTo(&tempParams); //���Ƶ���ʱ������
			if (!tempParams.update(adminConfig, userConfig)) { //������ʱ�����еĲ���
				tempParams.showMessageBox(this);
				this->setPushButtonsEnabled(true); return;
			}
			sysResetCode |= runtimeParams->getSystemResetCode(tempParams); //��ȡ���ô���
			tempParams.copyTo(runtimeParams); //����ʱ�����Ƶ����в�����
		}

		//�ж��Ƿ����ü��ϵͳ
		if (sysResetCode != 0 && adminConfig->isValid(true) && userConfig->isValid(adminConfig)
			&& runtimeParams->isValid(RuntimeParams::Index_All_SysInit, true, adminConfig))
		{
			QMessageBox::warning(this, pcb::chinese("��ʾ"),
				pcb::chinese("�����޸Ĺؼ����������ģ�齫���������� \n"),
				pcb::chinese("ȷ��"));
			pcb::delay(10);
			emit resetDetectSystem_settingUI(sysResetCode);
		}
	}
	else if (userConfig->getErrorCode() != tempConfig.getErrorCode()) {
		//����ʱ���ÿ������û�����������
		tempConfig.copyTo(userConfig);
		//���������浽�����ļ���
		Configurator::saveConfigFile(configFileName, userConfig);
	}
		
	//�������淢����Ϣ
	emit checkSystemState_settingUI(); //���ϵͳ�Ĺ���״̬
	pcb::delay(10);
	
	//���������ϵİ�����Ϊ�ɵ��
	this->setPushButtonsEnabled(true);
}

//���ؼ�
void SettingUI::on_pushButton_return_clicked()
{
	emit showDetectMainUI();
	//��������ϵĲ�����Ч����userConfig��Ч������ʾuserConfig
	if (!tempConfig.isValid(adminConfig) && userConfig->isValid(adminConfig)) {
		this->refresh();
	}
}


/************** ��ȡ�����ϵĲ��� *****************/

//���ں�
void SettingUI::on_currentIndexChanged_comPort()
{
	switch (ui.comboBox_clusterComPort->currentIndex())
	{
	case 0:
		tempConfig.clusterComPort = ""; break;
	case 1:
		tempConfig.clusterComPort = "COM1"; break;
	case 2:
		tempConfig.clusterComPort = "COM2"; break;
	case 3:
		tempConfig.clusterComPort = "COM3"; break;
	case 4:
		tempConfig.clusterComPort = "COM4"; break;
	case 5:
		tempConfig.clusterComPort = "COM5"; break;
	case 6:
		tempConfig.clusterComPort = "COM6"; break;
	case 7:
		tempConfig.clusterComPort = "COM7"; break;
	case 8:
		tempConfig.clusterComPort = "COM8"; break;
	case 9:
		tempConfig.clusterComPort = "COM9"; break;
	default:
		break;
	}
}

//ͼ���ʽ
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
	
	//��������
	tempConfig.SampleDirPath = ui.lineEdit_SampleDirPath->text(); //����·��
	tempConfig.TemplDirPath = ui.lineEdit_TemplDirPath->text(); //ģ��·��
	tempConfig.OutputDirPath = ui.lineEdit_OutputDirPath->text();//���·��

	tempConfig.ActualProductSize_W = ui.lineEdit_ActualProductSize_W->text().toInt();
	tempConfig.ActualProductSize_H = ui.lineEdit_ActualProductSize_H->text().toInt();
	tempConfig.nBasicUnitInRow = ui.lineEdit_nBasicUnitInRow->text().toInt();//ÿһ���еĻ�����Ԫ��
	tempConfig.nBasicUnitInCol = ui.lineEdit_nBasicUnitInCol->text().toInt();//ÿһ���еĻ�����Ԫ��

	QString defectTypeToBeProcessed = "";
	if (ui.checkBox_defectType_short->isChecked()) defectTypeToBeProcessed;

	//�����
	tempConfig.exposureTime = ui.lineEdit_ActualProductSize_W->text().toInt();//�ع�ʱ��
	if (ui.lineEdit_colorMode_rgb_camera->isChecked()) tempConfig.colorMode = 0;//ɫ��ģʽ
	if (ui.lineEdit_colorMode_gray_camera->isChecked()) tempConfig.colorMode = 1;

	//����㷨
	tempConfig.defectTypeToBeProcessed[0] = ui.checkBox_defectType_short->isChecked();//������ȱ������
	tempConfig.defectTypeToBeProcessed[1] = ui.checkBox_defectType_break->isChecked();
	tempConfig.defectTypeToBeProcessed[2] = ui.checkBox_defectType_convex->isChecked();
	tempConfig.defectTypeToBeProcessed[3] = ui.checkBox_defectType_concave->isChecked();
	
	//����㷨
	int accuracyLevel = 0; //ƥ��ģʽ
	if (ui.checkBox_matchingAccuracy_high->isChecked()) accuracyLevel = 1;
	if (ui.checkBox_matchingAccuracy_low->isChecked()) accuracyLevel = 2;
	tempConfig.matchingAccuracyLevel = accuracyLevel; 

	tempConfig.concaveRateThresh = ui.lineEdit_concaveRateThresh->text().toInt();//ȱʧ����ֵ
	tempConfig.convexRateThresh = ui.lineEdit_convexRateThresh->text().toInt();//͹������ֵ

	//�ļ�����
	tempConfig.inetAddressOfRecheckPC = ui.lineEdit_inetAddressOfRecheckPC->text(); //�����豸IP
}


/****************** ϵͳ������¼���� *******************/

//��ϵͳ������½����
void SettingUI::on_pushButton_admin_clicked()
{
	//���ô���ʼ���ö�
	passWordUI.show();
	//���ð���
	this->setPushButtonsEnabled(false);
}

//�ر�ϵͳ������½����
void SettingUI::on_closePassWordUI_pswdUI()
{
	//��ϵͳ����������Ϊ�ɵ��
	this->setPushButtonsEnabled(true);//���ð���
}

//��ʾϵͳ�������ý���
void SettingUI::do_showAdminSettingUI_pswdUI()
{
	adminSettingUI->setAdminConfig(adminConfig);
	adminSettingUI->refreshAdminSettingUI();
	pcb::delay(10);
	adminSettingUI->showFullScreen();
	pcb::delay(10);
	this->hide(); //���ز������ý���
	this->setPushButtonsEnabled(true);//���ð���
}


/****************** ϵͳ�������ý��� *******************/

//��ϵͳ�������ý��淵�ز������ý���
void SettingUI::do_showSettingUI_adminUI()
{
	this->setCursorLocation(UserConfig::Index_None);
	this->showFullScreen();
	pcb::delay(10);
	adminSettingUI->hide();
}

//ת����ϵͳ���ý��淢���������ź�
void SettingUI::do_resetDetectSystem_adminUI(int code)
{
	emit resetDetectSystem_settingUI(code);
}

//ת����ϵͳ���ý��淢����ϵͳ״̬����ź�
void SettingUI::do_checkSystemWorkingState_adminUI()
{
	if (adminConfig->isValid(true) && userConfig->isValid(adminConfig)
		&& runtimeParams->isValid(RuntimeParams::Index_All_SysInit, true, adminConfig))
	{
		emit checkSystemState_settingUI();
		pcb::delay(10);
	}
	else {
	}
}
