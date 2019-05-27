#pragma once

#include "AMC98C.h"
#include <QWidget>
#include "ui_DetectUI.h"
#include "Configurator.h"
#include "RuntimeLib.h"
#include "DetectThread.h"
#include "ImgConvertThread.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include "SerialNumberUI.h"
#include <vector>
#include <QRect>
#include <QList>
#include <QGraphicsItem>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QDebug>


//������
class DetectUI : public QWidget
{
	Q_OBJECT

private:
	Ui::DetectUI ui;
	QString IconFolder; //ͼ���ļ���
	QPixmap lightOnIcon; //����ͼ�� red
	QPixmap lightOffIcon; //���ͼ�� grey
	QPixmap defaultIcon; //Ĭ��ͼ��
	SerialNumberUI *serialNumberUI; //��Ʒ���ʶ�����

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������

	const qreal ItemSpacing = 0; //ͼԪ���
	QSizeF itemSize; //ͼԪ�ߴ�
	QSizeF gridSize; //����ߴ�
	QGraphicsScene scene; //��ͼ����
	QSizeF sceneSize; //�����ߴ�
	pcb::ItemGrid itemGrid; //��ͼ����
	pcb::ItemArray itemArray; //�����е�ͼԪ����

	int currentRow_show; //������ʾ��ĳ��ͼ����к�
	int eventCounter; //�¼�������
	pcb::CvMatArray cvmatSamples; //���ڼ�������ͼ
	pcb::QPixmapArray qpixmapSamples;//������ʾ������ͼ
	ImgConvertThread imgConvertThread;//ͼ���ʽת���߳�

	DetectThread *detectThread; //����߳�
	DefectDetecter *defectDetecter; //��������
	pcb::DetectResult detectResult; //�����
	int detectState; //���״̬

public:
	DetectUI(QWidget *parent = Q_NULLPTR);
	~DetectUI();
	void init();
	void reset();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setMotionControler(MotionControler *ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler *ptr) { cameraControler = ptr; }

	void initGraphicsView();
	void setPushButtonsEnabled(bool enable);
	void refreshCameraControler();

private:
	void initItemGrid(pcb::ItemGrid &grid);//��ʼ��ͼԪ����
	void initPointersInItemArray(pcb::ItemArray &items);//��ʼ��itemArray
	void deletePointersInItemArray(pcb::ItemArray &items);//ɾ��itemArray�е�ָ��
	void initPointersInCvMatArray(pcb::CvMatArray &cvmats);//��ʼ��CvMatArray
	void deletePointersInCvMatArray(pcb::CvMatArray &cvmats);//ɾ��CvMatArray�е�ָ��
	void initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//��ʼ��QPixmapArray
	void deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//ɾ��QPixmapArray�е�ָ��
	void removeItemsFromGraphicsScene();//�Ƴ������е�ͼԪ

	void showSampleImages();
	void readSampleImages();
	void detectSampleImages(); //���

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNumberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();
	void on_pushButton_return_clicked();

	void keyPressEvent(QKeyEvent *event);
	void do_updateDetectState_detecter(int state);
	void on_detectFinished_detectThread(bool qualified);

	void on_resetControlerFinished_motion(int);
	void on_moveToInitialPosFinished_motion();
	void on_moveForwardFinished_motion();

	void on_initCamerasFinished_camera(int);
	void on_takePhotosFinished_camera(int);
	void on_convertFinished_convertThread();

	void mouseDoubleClickEvent(QMouseEvent *event);
	void on_recognizeFinished_serialNumUI();
	void do_showPreviousUI_serialNumUI();
};
