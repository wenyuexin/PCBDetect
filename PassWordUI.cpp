#include "PassWordUI.h"

PassWordUI::PassWordUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setFixedSize(350, 220);
	setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);//ȡ����󻯺���С��
	this->setWindowTitle(QString::fromLocal8Bit("��¼"));
	ui.lineEdit_pswd->setEchoMode(QLineEdit::Password);
	connect(ui.lineEdit_pswd, SIGNAL(returnPressed()), this, SLOT(on_pushButton_confirm_clicked()));//����س���ȷ����ť
}

PassWordUI::~PassWordUI()
{
}

/***************** ������Ӧ *****************/

//ȷ��
void PassWordUI::on_pushButton_confirm_clicked()
{
	QString pswd = ui.lineEdit_pswd->text();
	if (pswd == "") {
		QMessageBox::information(this, QString::fromLocal8Bit("����"), 
			QString::fromLocal8Bit("���������룡"),
			QString::fromLocal8Bit("ȷ��"));
		ui.lineEdit_pswd->setFocus();
	}

	else if (pswd != PASS_WORD) {
		QMessageBox::information(this, QString::fromLocal8Bit("����"), 
			QString::fromLocal8Bit("����������������룡"),
			QString::fromLocal8Bit("ȷ��"));
		ui.lineEdit_pswd->setFocus();
	}
	else {
		emit entry();
		this->close();
	}
}

//ȡ��
void PassWordUI::on_pushButton_cancel_clicked()
{
	this->close();
}

void PassWordUI::closeEvent(QCloseEvent * event)
{
	emit UIClose();
}