#include "AdminSettingUI.h"


AdminSettingUI::AdminSettingUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//一些初始化操作
	this->initSettingUI();
}

AdminSettingUI::~AdminSettingUI()
{
}

void AdminSettingUI::initSettingUI()
{
	//限制参数的输入范围
	QIntValidator intValidator;
	ui.lineEdit_MaxMotionStroke->setValidator(&intValidator);
	ui.lineEdit_MaxCameraNum->setValidator(&intValidator);
	ui.lineEdit_MaxPhotographingNum->setValidator(&intValidator);
}

//更新界面
void AdminSettingUI::refreshAdminSettingUI()
{
	ui.lineEdit_MaxMotionStroke->setText(QString::number(adminConfig->MaxMotionStroke));
	ui.lineEdit_MaxCameraNum->setText(QString::number(adminConfig->MaxCameraNum));
	ui.lineEdit_MaxPhotographingNum->setText(QString::number(adminConfig->MaxPhotographingNum));
}


/********************** 按键响应 **********************/

//确认
void AdminSettingUI::on_pushButton_confirm_clicked()
{
	bool k1, k2, k3;
	double MaxS =( ui.lineEdit_MaxMotionStroke->text()).toDouble(&k1);
	int CamerN = (ui.lineEdit_MaxCameraNum->text()).toInt(&k2);
	int PhotoN =( ui.lineEdit_MaxPhotographingNum->text()).toInt(&k3);

	if(MaxS <= 0 || CamerN <= 0 || PhotoN <= 0)
		QMessageBox::information(this, QString::fromLocal8Bit("警告"), 
			QString::fromLocal8Bit("数据必须为正数，请重新输入！"),
			QString::fromLocal8Bit("确定"));
	else {
		adminConfig->MaxCameraNum = CamerN;
		adminConfig->MaxMotionStroke = MaxS;
		adminConfig->MaxPhotographingNum = PhotoN;

		//Configurator::saveConfigFile_A(fileName, config_A);
	}
}

//返回
void AdminSettingUI::on_pushButton_return_clicked()
{
	emit showSettingUI_adminSettingUI();
}
