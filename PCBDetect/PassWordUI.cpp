#include "PassWordUI.h"

PassWordUI::PassWordUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);//ȡ����󻯺���С��
	this->setWindowTitle(QString::fromLocal8Bit("ϵͳ���õ�¼"));
	ui.lineEdit_pswd->setEchoMode(QLineEdit::Password);
	connect(ui.lineEdit_pswd, SIGNAL(returnPressed()), this, SLOT(on_pushButton_confirm_clicked()));//����س���ȷ����ť

	//�����������뷨
	ui.lineEdit_pswd->setAttribute(Qt::WA_InputMethodEnabled, false);
}

PassWordUI::~PassWordUI()
{
	qDebug() << "~PassWordUI";
}


/**************** ������Ӧ *****************/

void PassWordUI::on_pushButton_confirm_clicked()
{
	QString pswd = ui.lineEdit_pswd->text();
	if (pswd == "") { //����Ϊ��
		QMessageBox::warning(this, QString::fromLocal8Bit("����"), 
			QString::fromLocal8Bit("��ǰ����Ϊ�գ����������룡 "),
			QString::fromLocal8Bit("ȷ��"));
		ui.lineEdit_pswd->setFocus();
	}
	else if (pswd != PASS_WORD) { //�������
		QMessageBox::warning(this, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("����������������룡 "),
			QString::fromLocal8Bit("ȷ��"));
		ui.lineEdit_pswd->setFocus();
	}
	else { //������ȷ����ʾϵͳ�������ý���
		ui.lineEdit_pswd->setText("");//�������
		this->hide(); //���ص�¼����
		qApp->processEvents();
		emit showAdminSettingUI_pswdUI();
	}
}

//ȡ��
void PassWordUI::on_pushButton_cancel_clicked()
{
	ui.lineEdit_pswd->setText("");//�������
	this->hide(); //���ص�¼����
	qApp->processEvents();
	emit closePassWordUI_pswdUI();
}

/**************** ���� *****************/

//�رյ�¼����
void PassWordUI::closeEvent(QCloseEvent * event)
{
	emit closePassWordUI_pswdUI();
}