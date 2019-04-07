#pragma once

#include <QWidget>
#include "ui_LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "CameraControler.h"
#include <QDesktopWidget>
#include <QPixmap>
#include <QDir>
#include <QTime>

namespace pcb {
	class LaunchUI;
}

//��������
class LaunchUI : public QWidget
{
	Q_OBJECT

private:
	Ui::LaunchUI ui;
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���
	CameraControler *cameraControler; //���������
	SysInitThread *initThread; //ϵͳ��ʼ���߳�

public:
	LaunchUI(QWidget *parent = Q_NULLPTR);
	~LaunchUI();

	inline void setDetectConfig(pcb::DetectConfig* ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setAdminConfig(pcb::AdminConfig* ptr = Q_NULLPTR) { adminConfig = ptr; }
	inline void setCameraControler(CameraControler* ptr = Q_NULLPTR) { cameraControler = ptr; }
	void runInitThread(); //����

Q_SIGNALS:
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString status);
	void on_detectConfigError_initThread();
	void on_adminConfigError_initThread();
	void on_cameraError_initThread();
	void on_sysInitFinished_initThread();
};
