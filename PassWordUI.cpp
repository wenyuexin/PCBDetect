#include "PassWordUI.h"

PassWordUI::PassWordUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setFixedSize(350, 220);
	setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);//取消最大化和最小化
	this->setWindowTitle(QString::fromLocal8Bit("登录"));
	ui.lineEdit_pswd->setEchoMode(QLineEdit::Password);
	connect(ui.lineEdit_pswd, SIGNAL(returnPressed()), this, SLOT(on_pushButton_confirm_clicked()));//点击回车绑定确定按钮
}

PassWordUI::~PassWordUI()
{
}

/***************** 按键响应 *****************/

//确认
void PassWordUI::on_pushButton_confirm_clicked()
{
	QString pswd = ui.lineEdit_pswd->text();
	if (pswd == "") {
		QMessageBox::information(this, QString::fromLocal8Bit("警告"), 
			QString::fromLocal8Bit("请输入密码！"),
			QString::fromLocal8Bit("确定"));
		ui.lineEdit_pswd->setFocus();
	}

	else if (pswd != PASS_WORD) {
		QMessageBox::information(this, QString::fromLocal8Bit("警告"), 
			QString::fromLocal8Bit("密码错误，请重新输入！"),
			QString::fromLocal8Bit("确定"));
		ui.lineEdit_pswd->setFocus();
	}
	else {
		emit entry();
		this->close();
	}
}

//取消
void PassWordUI::on_pushButton_cancel_clicked()
{
	this->close();
}

void PassWordUI::closeEvent(QCloseEvent * event)
{
	emit UIClose();
}