#pragma once

#include <QWidget>
#include "ui_SettingUI.h"
#include "Configurator.h"
#include <QDesktopWidget>
#include <QRegExpValidator>


namespace Ui {
	class SettingUI;
}

//参数设置界面
class SettingUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SettingUI ui;
	Ui::DetectConfig *config;
	Ui::DetectConfig tempConfig;
	QDoubleValidator doubleValidator;
	QIntValidator intValidator;

public:
	SettingUI(QWidget *parent = Q_NULLPTR);
	~SettingUI();

	inline void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }
	void refreshSettingUI();
	void setPushButtonsToEnabled(bool code);

private:
	void initSettingUI();
	void selectDirPath(QString &path);
	void getConfigFromSettingUI();
	void setCursorLocation(Ui::DetectConfig::ConfigIndex code);

Q_SIGNALS:
	void showDetectMainUI();
	void resetDetectSystem_settingUI(int);
	void enableButtonsOnDetectMainUI_settingUI();
	void checkSystemWorkingState_settingUI();

private Q_SLOTS:
	void on_pushButton_SampleDirPath_clicked();
	void on_pushButton_TemplDirPath_clicked();
	void on_pushButton_OutputDirPath_clicked();
	void on_currentIndexChanged_imageFormat();

	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();
};
