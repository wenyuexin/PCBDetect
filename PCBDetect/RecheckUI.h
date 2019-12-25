#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RecheckUI.h"
#include <QKeyEvent>
#include <QDebug>
#include <QDir>
#include <QMap>
#include <QGraphicsScene>
#include <QPixmap>
#include <QRect>
#include <QDesktopWidget>
#include <QTimer>
#include <QMessageBox>
#include <time.h> 
#include "pcbFuncLib.h"
#include "RuntimeParams.h"
#include "Configurator.h"
#include "SysInitThread.h"
#include "SerialNumberUI.h"
//#include "ExitQueryUI.h"
#include "FlickeringArrow.h"
#include "FileSyncThread.h"


//����������
class PCBRecheck : public QMainWindow
{
	Q_OBJECT

public:
	enum RecheckStatus {
		NoError,
		Unchecked,
		CurrentBatchRechecked, //��ǰ�����Ѹ������
		InvalidFullImageName, //��ͼ���ļ�����Ч
		FullImageNotFound, //û���ҵ���ͼ
		LoadFullImageFailed, //��ͼ����ʧ��
		FlawImageNotFound, //û���ҵ�ȱ��ͼ
		LoadFlawImageFailed, //ȱ��ͼ����ʧ��
		OpenFlawImageFolderFailed, //�޷���ȱ��ͼ���ڵ��ļ���
		Default
	};

private:
	Ui::PCBRecheckClass ui;
	SysInitThread *sysInitThread; //ϵͳ��ʼ���߳�
	SerialNumberUI *serialNumberUI; //pcb������ý���
	//ExitQueryUI *exitQueryUI; //�˳�ѯ�ʽ���
	pcb::UserConfig userConfig; //����ϵͳ������Ϣ
	pcb::RuntimeParams runtimeParams; //���в���
	RecheckStatus recheckStatus; //����״̬

	QPixmap fullImage; //PCB��ͼ
	const QString fullImageNamePrefix = "fullImage"; //�ļ���ǰ׺
	QSize originalFullImageSize; //PCB��ͼ��ԭʼ�ߴ�
	QSize fullImageItemSize; //PCB��ͼ����ʾ�ߴ�
	qreal scaledFactor; //PCB��ͼ�ĳߴ�任����
	QGraphicsScene fullImageScene; //��ʾPCB��ͼ�ĳ���
	FlickeringArrow flickeringArrow; //PCB��ͼ����˸�ļ�ͷ

	QString IconFolder; //ͼ���ļ���
	QPixmap lightOnIcon; //����ͼ�� red
	QPixmap lightOffIcon; //���ͼ�� grey

	int defectNum; //ȱ������
	int defectIndex; //��ǰ������ʾ��ȱ��ͼ
	//QVector<pcb::FlawImageInfo> flawImageInfoVec; //ȱ��ͼ����Ϣ
	uint16_t flawIndicatorStatus; //ָʾ������״̬

	//pcb::FolderHierarchy OutFolderHierarchy; //���Ŀ¼�µ��ļ��в��

	FileSyncThread fileSyncThread;//�ļ�ͬ���߳�

public:
	PCBRecheck(QWidget *parent = Q_NULLPTR);
	~PCBRecheck();

private:
	void init();
	void showLastFlawImage(); //��ʾ��һ��ȱ��ͼ
	void showNextFlawImage(); //��ʾ��һ��ȱ��ͼ
	
	void refreshRecheckUI(); //���¸������
	bool loadFullImage(); //����PCB��ͼ
	void showFullImage(); //��ʾPCB��ͼ
	void initFlickeringArrow(); //���س�ʼ����˸��ͷ
	void setFlickeringArrowPos(); //���¼�ͷ��λ��

	void getFlawImageInfo(QString dirpath);
	void showFlawImage();
	void switchFlawIndicator();

	//void showSerialNumberUI();
	//void showExitQueryUI();

	void setPushButtonsEnabled(bool enable);
	void exitRecheckSystem();
	//void showMessageBox(pcb::MessageBoxType type, RecheckStatus status = Default);

	void logging(QString msg);

private Q_SLOTS:
	//void on_sysInitFinished_initThread(); //��ʼ���̷߳���������
	//void on_userConfigError_initThread(); //��ʼ������
	//void on_outFolderHierarchyError_initThread(); //��ʼ������

	void keyPressEvent(QKeyEvent *event); //�û������¼�
	void on_pushButton_plus2_clicked(); //����ӺŰ���
	void on_pushButton_minus2_clicked(); //������Ű���
	void on_pushButton_exit_clicked(); //����˳�����

	//void do_showRecheckUI_numUI(); //pcb������ý��淵��������
	//void do_exitRecheckSystem_numUI(); //�˳�ϵͳ

	//void do_showSerialNumberUI_exitUI();
	//void do_showRecheckUI_exitUI();
	void on_refreshArrow_arrow();
};
