#include "AdminSettingUI.h"

using pcb::Configurator;
using pcb::AdminConfig;
using pcb::DetectConfig;
using pcb::DetectParams;


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//������ʼ��
	adminConfig = Q_NULLPTR; //ϵͳ����
	detectConfig = Q_NULLPTR; //�û�����
	detectParams = Q_NULLPTR; //���в���

	//һЩ��ʼ������
	this->initAdminSettingUI();
}

AdminSettingUI::~AdminSettingUI()
{
}

void AdminSettingUI::initAdminSettingUI()
{
	//���ù��
	this->setCursorLocation(AdminConfig::Index_None);

	//��һ���л�����״̬����ʾ�Ͽ��ܻ�����ӳ٣�����ǰԤ��
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//���Ʋ��������뷶Χ
	QIntValidator intValidator;
	QDoubleValidator doubleValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(&intValidator);
	ui.lineEdit_MaxCameraNum->setValidator(&intValidator);
	ui.lineEdit_PixelsNumPerUnitLength->setValidator(&intValidator);
	ui.lineEdit_ImageOverlappingRate->setValidator(&doubleValidator);
	ui.lineEdit_ImageSize_W->setValidator(&intValidator);
	ui.lineEdit_ImageSize_H->setValidator(&intValidator);
}

//���½���
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_PixelsNumPerUnitLength->setText(QString::number(adminConfig->PixelsNumPerUnitLength));
	ui.lineEdit_ImageOverlappingRate->setText(QString::number(adminConfig->ImageOverlappingRate));
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
		this->setPushButtonsToEnabled(true);//��������Ϊ�ɵ��
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
		sysResetCode |= detectParams->calcItemGridSize(adminConfig, detectConfig);
		if (!detectParams->isValid()) detectParams->showMessageBox(this);
		//���ͼ��ϵͳ�����ź�
		emit resetDetectSystem_adminUI(sysResetCode);
	}
	if (adminConfig->getErrorCode() != tempConfig.getErrorCode()) {
		//����ʱ���ÿ�����ϵͳ����������
		tempConfig.copyTo(adminConfig);
	}
	
	//���������ϵİ�����Ϊ�ɵ��
	this->setPushButtonsToEnabled(true);
}

//����
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminUI();
	//��������ϵ�ϵͳ������Ч����adminConfig��Ч������ʾadminConfig
	//ע����ʼ�����£�û�а�ȷ��������tempConfigΪ�գ�tempConfig��Ч
	if (!tempConfig.isValid() && adminConfig->isValid()) {
		this->refreshAdminSettingUI();
	}
}

//���ð����Ŀɵ��״̬
void AdminSettingUI::setPushButtonsToEnabled(bool code)
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
	tempConfig.MaxCameraNum = ui.lineEdit_MaxCameraNum->text().toInt();
	tempConfig.PixelsNumPerUnitLength = ui.lineEdit_PixelsNumPerUnitLength->text().toInt();
	tempConfig.ImageOverlappingRate = ui.lineEdit_ImageOverlappingRate->text().toDouble();
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
	case pcb::DetectConfig::Index_All:
	case pcb::DetectConfig::Index_None:
		ui.lineEdit_MaxMotionStroke->setFocus();
		ui.lineEdit_MaxMotionStroke->clearFocus(); break;
	case pcb::AdminConfig::Index_MaxMotionStroke:
		ui.lineEdit_MaxMotionStroke->setFocus(); break;
	case pcb::AdminConfig::Index_MaxCameraNum:
		ui.lineEdit_MaxCameraNum->setFocus(); break;
	case pcb::AdminConfig::Index_PixelsNumPerUnitLength:
		ui.lineEdit_PixelsNumPerUnitLength->setFocus(); break;
	case pcb::AdminConfig::Index_ImageOverlappingRate:
		ui.lineEdit_ImageOverlappingRate->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_W:
		ui.lineEdit_ImageSize_W->setFocus(); break;
	case pcb::AdminConfig::Index_ImageSize_H:
		ui.lineEdit_ImageSize_H->setFocus(); break;
	}
}