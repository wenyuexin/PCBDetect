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

}