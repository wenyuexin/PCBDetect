#include "PassWordUI.h"

PassWordUI::PassWordUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);//取消最大化和最小化
	this->setWindowTitle(QString::fromLocal8Bit("系统设置登录"));
	ui.lineEdit_pswd->setEchoMode(QLineEdit::Password);
	connect(ui.lineEdit_pswd, SIGNAL(returnPressed()), this, SLOT(on_pushButton_confirm_clicked()));//点击回车绑定确定按钮

	//禁用中文输入法
	ui.lineEdit_pswd->setAttribute(Qt::WA_InputMethodEnabled, false);
}

PassWordUI::~PassWordUI()
{
	qDebug() << "~PassWordUI";
}


/**************** 按键响应 *****************/

void PassWordUI::on_pushButton_confirm_clicked()
{
	QString pswd = ui.lineEdit_pswd->text();
	if (pswd == "") { //密码为空
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), 
			QString::fromLocal8Bit("当前密码为空，请输入密码！ "),
			QString::fromLocal8Bit("确定"));
		ui.lineEdit_pswd->setFocus();
	}
	else if (pswd != PASS_WORD) { //密码错误
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
			QString::fromLocal8Bit("密码错误，请重新输入！ "),
			QString::fromLocal8Bit("确定"));
		ui.lineEdit_pswd->setFocus();
	}
	else { //密码正确，显示系统参数设置界面
		ui.lineEdit_pswd->setText("");//清空密码
		this->hide(); //隐藏登录界面
		qApp->processEvents();
		emit showAdminSettingUI_pswdUI();
	}
}

//取消
void PassWordUI::on_pushButton_cancel_clicked()
{
	ui.lineEdit_pswd->setText("");//清空密码
	this->hide(); //隐藏登录界面
	qApp->processEvents();
	emit closePassWordUI_pswdUI();
}

/**************** 其他 *****************/

//关闭登录界面
void PassWordUI::closeEvent(QCloseEvent * event)
{
	emit closePassWordUI_pswdUI();
}