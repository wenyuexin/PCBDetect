#pragma once

#include <QWidget>
#include "ui_TemplateUI.h"
#include "Configurator.h"
#include "TemplateThread.h"
#include "ImgConvertThread.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include "SerialNumberUI.h"
#include <QDesktopWidget>
#include <QKeyEvent>


//ģ����ȡ����
class TemplateUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	SerialNumberUI serialNumberUI; //��Ʒ���ʶ�����
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectConfig *detectConfig; //�û�����
	pcb::DetectParams *detectParams; //���в���
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������

	const int itemSpacing = 3; //ͼԪ���
	QSize itemSize; //ͼԪ�ߴ�
	QSize gridSize; //����ߴ�
	QGraphicsScene scene; //��ͼ����
	QSize sceneSize; //�����ߴ�
	pcb::ItemGrid itemGrid; //��ͼ����
	pcb::ItemArray itemArray; //�����е�ͼԪ����

	int currentRow_show; //������ʾ��ĳ��ͼ����к�
	int eventCounter; //�¼�������
	pcb::CvMatArray cvmatSamples; //���ڼ�������ͼ
	pcb::QPixmapArray qpixmapSamples;//������ʾ������ͼ
	ImgConvertThread imgConvertThread;//ͼ���ʽת���߳�
	TemplateThread *templThread; //ģ����ȡ�߳�
	TemplateExtractor *templExtractor; //ģ����ȡ��

public:
	TemplateUI(QWidget *parent = Q_NULLPTR);
	~TemplateUI();
	
	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectConfig(pcb::DetectConfig *ptr) { detectConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }
	inline void setMotionControler(MotionControler *ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler *ptr) { cameraControler = ptr; }

	void doConnect();
	void initGraphicsView();
	void resetTemplateUI();

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
	void nextRowOfSampleImages();
	void readSampleImages2();
	void nextRowOfSampleImages2();
	void extractTemplateImages();

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//��ʼ��
	void on_pushButton_return_clicked();//���ؼ�
	void keyPressEvent(QKeyEvent *event);
	void update_extractState_extractor(int state);

	void on_moveForwardFinished_motion();//ǰ������
	void on_resetControlerFinished_motion(int);//��λ����
	void on_initCamerasFinished_camera(int);//�����ʼ������
	void on_takePhotosFinished_camera(int);//���ս���
	void on_convertFinished_convertThread();//ͼ��ת������

	void mouseDoubleClickEvent(QMouseEvent *event);
	void on_switchImage_serialNumUI();
	void on_recognizeFinished_serialNumUI();
	void do_showPreviousUI_serialNumUI();
};
