#pragma once

#include <QWidget>
#include "ui_LaunchUI.h"
#include "SysInitThread.h"
#include "Configurator.h"
#include "RuntimeLib.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include <QDesktopWidget>
#include <QPixmap>
#include <QDir>
#include <QTime>


//��������
class LaunchUI : public QWidget
{
	Q_OBJECT

private:
	Ui::LaunchUI ui;
	SysInitThread *initThread; //ϵͳ��ʼ���߳�
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������

public:
	LaunchUI(QWidget *parent = Q_NULLPTR, QRect &screenRect = QRect());
	~LaunchUI();

	inline void setAdminConfig(pcb::AdminConfig* ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig* ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams* ptr) { runtimeParams = ptr; }
	inline void setMotionControler(MotionControler* ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler* ptr) { cameraControler = ptr; }
	void runInitThread(); //����

Q_SIGNALS:
	void initGraphicsView_launchUI(int LaunchCode);
	void launchFinished_launchUI(int LaunchCode);

private Q_SLOTS:
	void update_sysInitStatus_initThread(QString);
	void on_adminConfigError_initThread();
	void on_userConfigError_initThread();
	void on_runtimeParamsError_initThread();
	void on_initGraphicsView_initThread(int);

	void on_motionError_initThread(int code = MotionControler::Default);
	void on_cameraError_initThread();
	void on_sysInitFinished_initThread();
};
