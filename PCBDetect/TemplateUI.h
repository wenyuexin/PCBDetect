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


namespace Ui {
	class TemplateUI;
}

//ģ����ȡ����
class TemplateUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	SerialNumberUI serialNumberUI; //��Ʒ���ʶ�����
	Ui::DetectConfig *detectConfig; //��������
	Ui::DetectParams *detectParams; //��ʱ����
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������

	int itemSpacing = 3; //ͼԪ���
	QSize itemSize; //ͼԪ�ߴ�
	QSize gridSize; //����ߴ�
	QGraphicsScene scene; //��ͼ����
	QSize sceneSize; //�����ߴ�
	Ui::ItemGrid itemGrid; //��ͼ����
	Ui::ItemArray itemArray; //�����е�ͼԪ����

	int currentRow_show; //������ʾ��ĳ��ͼ����к�
	int eventCounter; //�¼�������
	Ui::CvMatArray cvmatSamples; //���ڼ�������ͼ
	Ui::QPixmapArray qpixmapSamples;//������ʾ������ͼ
	ImgConvertThread imgConvertThread;//ͼ���ʽת���߳�
	TemplateThread *templThread; //ģ����ȡ�߳�
	TemplateExtractor *templExtractor; //ģ����ȡ��

public:
	TemplateUI(QWidget *parent = Q_NULLPTR);
	~TemplateUI();
	
	inline void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { detectConfig = ptr; }
	inline void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR) { detectParams = ptr; }
	inline void setMotionControler(MotionControler *ptr = Q_NULLPTR) { motionControler = ptr; }
	inline void setCameraControler(CameraControler *ptr = Q_NULLPTR) { cameraControler = ptr; }

	void doConnect();
	void initGraphicsView();
	void resetTemplateUI();

private:
	void readSampleImages();
	void showSampleImages();
	void nextRowOfSampleImages();

	void readSampleImages2();
	void nextRowOfSampleImages2();

	void extractTemplateImages();

	void initItemGrid(Ui::ItemGrid &grid);//��ʼ��ͼԪ����
	void initPointersInItemArray(Ui::ItemArray &items);//��ʼ��itemArray
	void deletePointersInItemArray(Ui::ItemArray &items);//ɾ��itemArray�е�ָ��
	void initPointersInCvMatArray(Ui::CvMatArray &cvmats);//��ʼ��CvMatArray
	void deletePointersInCvMatArray(Ui::CvMatArray &cvmats);//ɾ��CvMatArray�е�ָ��
	void initPointersInQPixmapArray(Ui::QPixmapArray &qpixmaps);//��ʼ��QPixmapArray
	void deletePointersInQPixmapArray(Ui::QPixmapArray &qpixmaps);//ɾ��QPixmapArray�е�ָ��
	void removeItemsFromGraphicsScene();//�Ƴ������е�ͼԪ

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//��ʼ��
	void on_pushButton_return_clicked();//���ؼ�
	void keyPressEvent(QKeyEvent *event);
	void update_extractState_extractor(int state);

	void on_moveForwardFinished_motion();
	void on_resetControlerFinished_motion(int);
	void on_initCamerasFinished_camera(int);
	void on_takePhotosFinished_camera(int);
	void on_convertFinished_convertThread();

	void mouseDoubleClickEvent(QMouseEvent *event);
	void on_switchImage_serialNumberUI();
	void on_recognizeFinished_serialNumberUI();
};
