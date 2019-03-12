#pragma once

#include <QWidget>
#include "ui_SettingUI.h"
#include "Configurator.h"
#include <QDesktopWidget>
#include <QRegExpValidator>


namespace Ui {
	class SettingUI;
}

//�������ý���
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	Ui::DetectConfig *config;
	QDoubleValidator doubleValidator;
	QIntValidator intValidator;

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();
	void setDetectConfig(Ui::DetectConfig *);
	void refreshSettingUI();

private:
	void initSettingUI();
	void selectDirPath(QString &path);
	void writeConfigFile(QString &fileName);

Q_SIGNALS:
	void showDetectMainUI();

private Q_SLOTS:
	void on_pushButton_SampleDirPath_clicked();
	void on_pushButton_TemplDirPath_clicked();
	void on_pushButton_OutputDirPath_clicked();
	void on_currentIndexChanged_imageFormat();

	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();
};
