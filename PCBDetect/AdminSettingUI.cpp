#include "AdminSettingUI.h"

using pcb::Configurator;
using pcb::AdminConfig;
using pcb::UserConfig;
using pcb::RuntimeParams;


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//������ʼ��
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	NumberValidator = new QRegExpValidator(QRegExp("[0-9]+$"));
}

void AdminSettingUI::init()
{
	//����״̬��ѡ�����������Ǹ�������ʾ
	this->setGeometry(runtimeParams->ScreenRect);

	//���ù��
	this->setCursorLocation(AdminConfig::Index_None);

	//��һ���л�����״̬����ʾ�Ͽ��ܻ�����ӳ٣�����ǰԤ��
	this->setPushButtonsEnabled(false);
	this->setPushButtonsEnabled(true);

	//���Ʋ��������뷶Χ
	QIntValidator intValidator;
	QDoubleValidator doubleValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(NumberValidator);
	ui.lineEdit_PulseNumInUnitTime->setValidator(NumberValidator);
	ui.lineEdit_MaxCameraNum->setValidator(NumberValidator);
	ui.lineEdit_PixelsNumPerUnitLength->setValidator(NumberValidator);
	ui.lineEdit_ImageOverlappingRate_W->setValidator(&doubleValidator);
	ui.lineEdit_ImageOverlappingRate_H->setValidator(&doubleValidator);
	ui.lineEdit_ImageSize_W->setValidator(NumberValidator);
	ui.lineEdit_ImageSize_H->setValidator(NumberValidator);
}

AdminSettingUI::~AdminSettingUI()
{
	qDebug() << "~AdminSettingUI";
	delete NumberValidator;
	NumberValidator = Q_NULLPTR;
}


/***************** ���½��� ****************/

//���½���
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_PulseNumInUnitTime->setText(QString::number(adminConfig->PulseNumInUnitTime));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_PixelsNumPerUnitLength->setText(QString::number(adminConfig->PixelsNumPerUnitLength, 'f', 6));
	ui.lineEdit_ImageOverlappingRate_W->setText(QString::number(adminConfig->ImageOverlappingRate_W, 'f', 6));
	ui.lineEdit_ImageOverlappingRate_H->setText(QString::number(adminConfig->ImageOverlappingRate_H, 'f', 6));
	ui.lineEdit_ImageSize_W->setText(QString::number(adminConfig->ImageSize_W));
	ui.lineEdit_ImageSize_H->setText(QString::number(adminConfig->ImageSize_H));
}


/********************** ������Ӧ **********************/

//ȷ��
void AdminSettingUI::on_pushButton_confirm_clicked()
{
	//��ȡ�����ϵ�config����
	this->getConfigFromAdminSettingUI();

	//��������config����Ч��
	AdminConfig::ErrorCode code = tempConfig.checkValidity(AdminConfig::Index_All);
	if (code != AdminConfig::ValidConfig) { //������Ч�򱨴�
		tempConfig.showMessageBox(this);
		this->setPushButtonsEnabled(true);//��������Ϊ�ɵ��
		AdminConfig::ConfigIndex index = AdminConfig::convertCodeToIndex(code);
		this->setCursorLocation(index); //����궨λ����Ч�������������
		return;
	}

	//���þ۽�λ��
	this->setCursorLocation(AdminConfig::Index_None);

	//�жϲ����Ƿ���
	if (adminConfig->unequals(tempConfig) != AdminConfig::Index_None) {
		//�ж��Ƿ����ü��ϵͳ
		sysResetCode |= adminConfig->getSystemResetCode(tempConfig);
		//����ʱ���ÿ�����config��
		tempConfig.copyTo(adminConfig);
		//���������浽config�ļ���
		Configurator::saveConfigFile(configFileName, adminConfig);
		//�������в���


		if (userConfig->isValid(adminConfig, false)) {
			runtimeParams->copyTo(&tempParams);

			RuntimeParams::ErrorCode code;
			//���㵥��ǰ������
			code = tempParams.calcSingleMotionStroke(adminConfig);
			if (code != RuntimeParams::ValidValue) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			//����������������մ���
			code = tempParams.calcItemGridSize(adminConfig, userConfig);
			if (code != RuntimeParams::ValidValues) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			//�����ʼ����λ��
			code = tempParams.calcInitialPhotoPos(adminConfig);
			if (code != RuntimeParams::ValidValue) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}

			if (!tempParams.isValid(RuntimeParams::Index_All_SysInit, true, adminConfig)) {
				tempParams.showMessageBox(this); 
				this->setPushButtonsEnabled(true); return;
			}
			sysResetCode |= runtimeParams->getSystemResetCode(tempParams);
			tempParams.copyTo(runtimeParams);
		}

		//�ж��Ƿ����ü��ϵͳ
		if (sysResetCode != 0 && adminConfig->isValid(true) && userConfig->isValid(adminConfig)
			&& runtimeParams->isValid(RuntimeParams::Index_All_SysInit, true, adminConfig))
		{
			QMessageBox::warning(this, pcb::chinese("��ʾ"),
				pcb::chinese("�����޸Ĺؼ����������ģ�齫���������� \n"),
				pcb::chinese("ȷ��"));
			pcb::delay(10);
			emit resetDetectSystem_adminUI(sysResetCode);
		}
	}
	else if (adminConfig->getErrorCode() != tempConfig.getErrorCode()) {
		//����ʱ���ÿ�����ϵͳ����������
		tempConfig.copyTo(adminConfig);
	}
	
	//���������ϵİ�����Ϊ�ɵ��
	this->setPushButtonsEnabled(true);
}

//����
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminUI();
	//��������ϵ�ϵͳ������Ч����adminConfig��Ч������ʾadminConfig
	if (!tempConfig.isValid(true) && adminConfig->isValid(true)) {
		this->refreshAdminSettingUI();
	}
}

//���ð����Ŀɵ��״̬
void AdminSettingUI::setPushButtonsEnabled(bool code)
{
	ui.pushButton_confirm->setEnabled(code);//ȷ��
	ui.pushButton_return->setEnabled(code);//����
}


/**************** ��ȡ�����ϵĲ��� ****************/

//�ӽ����ȡconfig����
void AdminSettingUI::getConfigFromAdminSettingUI()
{
	tempConfig.resetErrorCode();

	tempConfig.MaxMotionStroke = ui.lineEdit_MaxMotionStroke->text().toInt();
	tempConfig.PulseNumInUnitTime = ui.lineEdit_PulseNumInUnitTime->text().toInt();
	tempConfig.MaxCameraNum = ui.lineEdit_MaxCameraNum->text().toInt();
	tempConfig.PixelsNumPerUnitLength = ui.lineEdit_PixelsNumPerUnitLength->text().toDouble();
	tempConfig.ImageOverlappingRate_W = ui.lineEdit_ImageOverlappingRate_W->text().toDouble();
	tempConfig.ImageOverlappingRate_H = ui.lineEdit_ImageOverlappingRate_H->text().toDouble();
	tempConfig.ImageSize_W = ui.lineEdit_ImageSize_W->text().toInt();
	tempConfig.ImageSize_H = ui.lineEdit_ImageSize_H->text().toInt();

	AdminConfig::ErrorCode code = tempConfig.calcImageAspectRatio();
	if (code != AdminConfig::ValidConfig) 
		adminConfig->showMessageBox(this, code); 
}

//���ý����Ϲ���λ��
void AdminSettingUI::setCursorLocation(AdminConfig::ConfigIndex code)
{
	switch (code)
	{
	case pcb::UserConfig::Index_All:
	case pcb::UserConfig::Index_None:
		ui.lineEdit_MaxMotionStroke->setFocus();
		ui.lineEdit_MaxMotionStroke->clearFocus(); break;
	case pcb::AdminConfig::Index_MaxMotionStroke:
		ui.lineEdit_MaxMotionStroke->setFocus(); break;
	case pcb::AdminConfig::Index_PulseNumInUnitTime:
		ui.lineEdit_PulseNumInUnitTime->setFocus(); break;
	case pcb::AdminConfig::Index_MaxCameraNum:
		ui.lineEdit_MaxCameraNum->setFocus(); break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength:
		ui.lineEdit_PixelsNumPerUnitLength->setFocus(); break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_W:
		ui.lineEdit_ImageOverlappingRate_W->setFocus(); break;
	case pcb::AdminConfig::Index_ImageOverlappingRate_H:
		ui.lineEdit_ImageOverlappingRate_H->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_W:
		ui.lineEdit_ImageSize_W->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_H:
		ui.lineEdit_ImageSize_H->setFocus(); break;
	}
}
