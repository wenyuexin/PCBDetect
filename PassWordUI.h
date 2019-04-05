#pragma once

#include <QWidget>
#include "ui_PassWordUI.h"
#include <QMessageBox>


namespace Ui {
	class PassWordUI;
}

//����ϵͳ�����������¼����
class PassWordUI : public QWidget
{
	Q_OBJECT

public:
	PassWordUI(QWidget *parent = Q_NULLPTR);
	~PassWordUI();

private:
	Ui::PassWordUI ui;
	const QString PASS_WORD = "1234";

Q_SIGNALS:
	void entry();//������ȷ�����͸��ź�
	void UIClose();

private Q_SLOTS:
	void on_pushButton_confirm_clicked();
	void on_pushButton_cancel_clicked();
	void closeEvent(QCloseEvent * event);
};
