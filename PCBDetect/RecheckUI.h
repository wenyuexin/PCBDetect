#pragma once

#include <QWidget>
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
#include "FlickeringArrow.h"
#include "FileSyncThread.h"


//�������
class RecheckUI : public QWidget
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
	Ui::RecheckUI ui;
	pcb::UserConfig *userConfig; //����ϵͳ������Ϣ
	pcb::RuntimeParams *runtimeParams; //���в���
	RecheckStatus recheckStatus; //����״̬
	pcb::DetectResult *detectResult; //�����

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

	//FileSyncThread fileSyncThread;//�ļ�ͬ���߳�

public:
	RecheckUI(QWidget *parent = Q_NULLPTR);
	~RecheckUI();

	void init(); //�����ʼ��
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; }

private:
	void reset(); //��������
	void refresh(); //���¸������

	void showFullImage(); //��ʾPCB��ͼ
	void initFlickeringArrow(); //���س�ʼ����˸��ͷ
	void setFlickeringArrowPos(); //���¼�ͷ��λ��
	void showLastFlawImage(); //��ʾ��һ��ȱ��ͼ
	void showNextFlawImage(); //��ʾ��һ��ȱ��ͼ

	//void getFlawImageInfo(QString dirpath);
	void showFlawImage();
	void switchFlawIndicator();

	//void showSerialNumberUI();
	//void showExitQueryUI();

	void setPushButtonsEnabled(bool enable);
	//void showMessageBox(pcb::MessageBoxType type, RecheckStatus status = Default);

	void logging(QString msg);

Q_SIGNALS:
	void recheckFinished_recheckUI();

private Q_SLOTS:
	void keyPressEvent(QKeyEvent *event); //�û������¼�
	void on_pushButton_plus2_clicked(); //����ӺŰ���
	void on_pushButton_minus2_clicked(); //������Ű���
	void on_pushButton_return_clicked(); //����˳�����
	void on_refreshArrow_arrow();
};
