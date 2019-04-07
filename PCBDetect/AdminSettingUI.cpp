#include "AdminSettingUI.h"

using pcb::Configurator;


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//һЩ��ʼ������
	this->initAdminSettingUI();
}

AdminSettingUI::~AdminSettingUI()
{
}

void AdminSettingUI::initAdminSettingUI()
{
	//��һ���л�����״̬����ʾ�Ͽ��ܻ�����ӳ٣�����ǰԤ��
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//���Ʋ��������뷶Χ
	QIntValidator intValidator;
	QDoubleValidator doubleValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(&intValidator);
	ui.lineEdit_MaxCameraNum->setValidator(&intValidator);
	//ui.lineEdit_MaxPhotographingNum->setValidator(&intValidator);
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
		this->setCursorLocation(index);//����궨λ����Ч�������������
		return;
	}

	//���þ۽�λ��
	this->setCursorLocation(AdminConfig::Index_None);

	//�ж��Ƿ����ü��ϵͳ
	int resetCode = adminConfig->getSystemResetCode(tempConfig);

	//����ʱ���ÿ�����config��
	tempConfig.copyTo(adminConfig);

	//����ϵͳ
	resetCode |= detectParams->updateGridSize(adminConfig, tempConfig);
	emit resetDetectSystem_adminUI(resetCode); //�ж��Ƿ����ü��ϵͳ

	//���������浽config�ļ���
	Configurator::saveConfigFile(configFileName, adminConfig);

	//�������淢����Ϣ
	emit checkSystemWorkingState_adminUI(); //���ϵͳ�Ĺ���״̬
	pcb::delay(100);

	//���������ϵİ�����Ϊ�ɵ��
	this->setPushButtonsToEnabled(true);
}

//����
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminUI();
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
void setCursorLocation(pcb::AdminConfig::ConfigIndex code)
{
	int MaxMotionStroke; //��е�ṹ������˶��г�
		int MaxCameraNum; //�������������
		int PixelsNumPerUnitLength; //ͼ��ֱ��� pix/mm
		double ImageOverlappingRate; //��ͼ�ص���
		int ImageSize_W; //ͼ����
		int ImageSize_H; //ͼ��߶�
		double ImageAspectRatio; //ͼ���߱�

	switch (code)
	{
	case pcb::DetectConfig::MaxMotionStroke:
		ui.lineEdit_MaxMotionStroke->setFocus(); break;
	case pcb::DetectConfig::MaxCameraNum:
		ui.lineEdit_MaxCameraNum->setFocus(); break;
	case pcb::DetectConfig::PixelsNumPerUnitLength:
		ui.lineEdit_PixelsNumPerUnitLength->setFocus(); break;
	case pcb::DetectConfig::ImageOverlappingRate:
		ui.lineEdit_ImageOverlappingRate->setFocus(); break;
	case pcb::DetectConfig::Index_ImageSize_W:
		ui.lineEdit_ImageSize_W->setFocus(); break;
	case pcb::DetectConfig::Index_ImageSize_H:
		ui.lineEdit_ImageSize_H->setFocus(); break;
	}
}