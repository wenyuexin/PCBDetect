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
	ui.lineEdit_ImageResolutionRatio->setValidator(&intValidator);
	ui.lineEdit_ImageOverlappingRate->setValidator(&doubleValidator);
}

//���½���
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	//ui.lineEdit_MaxPhotographingNum->setText(QString::number(adminConfig->MaxPhotographingNum));
	ui.lineEdit_ImageResolutionRatio->setText(QString::number(adminConfig->ImageResolutionRatio));
	ui.lineEdit_ImageOverlappingRate->setText(QString::number(adminConfig->ImageOverlappingRate));
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
	int resetCode = detectConfig->getSystemResetCode(tempConfig);

	//����ʱ���ÿ�����config��
	tempConfig.copyTo(adminConfig);

	//����ϵͳ
	emit resetDetectSystem_adminUI(resetCode); //�ж��Ƿ����ü��ϵͳ

	//���������浽config�ļ���
	Configurator::saveConfigFile(configFileName, adminConfig);

	//�������淢����Ϣ
	emit checkSystemWorkingState_adminUI(); //���ϵͳ�Ĺ���״̬

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

void AdminSettingUI::getConfigFromAdminSettingUI()
{
	tempConfig.MaxMotionStroke = ui.lineEdit_MaxMotionStroke->text().toInt();
	tempConfig.MaxCameraNum = ui.lineEdit_MaxCameraNum->text().toInt();
	tempConfig.ImageResolutionRatio = ui.lineEdit_ImageResolutionRatio->text().toInt();
	tempConfig.ImageOverlappingRate = ui.lineEdit_ImageOverlappingRate->text().toDouble();
}

void setCursorLocation(pcb::AdminConfig::ConfigIndex code);