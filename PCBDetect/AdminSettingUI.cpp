#include "AdminSettingUI.h"


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//һЩ��ʼ������
	this->initSettingUI();
}

AdminSettingUI::~AdminSettingUI()
{
}

void AdminSettingUI::initSettingUI()
{
	//��һ���л�����״̬����ʾ�Ͽ��ܻ�����ӳ٣�����ǰԤ��
	this->setPushButtonsToEnabled(false);
	this->setPushButtonsToEnabled(true);

	//���Ʋ��������뷶Χ
	QIntValidator intValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(&intValidator);
	ui.lineEdit_MaxCameraNum->setValidator(&intValidator);
	ui.lineEdit_MaxPhotographingNum->setValidator(&intValidator);
}

//���½���
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_MaxPhotographingNum->setText(QString::number(adminConfig->MaxPhotographingNum));
}


/********************** ������Ӧ **********************/

//ȷ��
void AdminSettingUI::on_pushButton_confirm_clicked()
{
	//��ȡ�����ϵ�config����
	getConfigFromAdminSettingUI();

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
	emit resetDetectSystem_settingUI(resetCode); //�ж��Ƿ����ü��ϵͳ

	//���������浽config�ļ���
	pcb::Configurator::saveConfigFile(configFileName, adminConfig);

	//�������淢����Ϣ
	emit checkSystemWorkingState_settingUI(); //���ϵͳ�Ĺ���״̬

	//���������ϵİ�����Ϊ�ɵ��
	this->setPushButtonsToEnabled(true);

	/*******/
	bool k1, k2, k3;
	double MaxS =( ui.lineEdit_MaxMotionStroke->text()).toDouble(&k1);
	int CamerN = (ui.lineEdit_MaxCameraNum->text()).toInt(&k2);
	int PhotoN =( ui.lineEdit_MaxPhotographingNum->text()).toInt(&k3);

	if(MaxS <= 0 || CamerN <= 0 || PhotoN <= 0)
		QMessageBox::information(this, QString::fromLocal8Bit("����"), 
			QString::fromLocal8Bit("���ݱ���Ϊ���������������룡"),
			QString::fromLocal8Bit("ȷ��"));
	else {
		adminConfig->MaxCameraNum = CamerN;
		adminConfig->MaxMotionStroke = MaxS;
		adminConfig->MaxPhotographingNum = PhotoN;

		//Configurator::saveConfigFile_A(fileName, config_A);
	}
}

//����
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminSettingUI();
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
}