#pragma once

#include <QWidget>
#include "ui_TemplateUI.h"
#include "Configurator.h"
#include "ExtractThread.h"
#include "ImgConvertThread.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include "SerialNumberUI.h"
#include <QDesktopWidget>
#include <QKeyEvent>


//ģ����ȡ����
class ExtractUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	SerialNumberUI *serialNumberUI; //��Ʒ���ʶ�����
	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������

	const qreal itemSpacing = 0; //ͼԪ��ࣨ��λ:pix��
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
	ExtractThread *extractThread; //ģ����ȡ�߳�
	TemplateExtractor *templExtractor; //ģ����ȡ��

public:
	ExtractUI(QWidget *parent = Q_NULLPTR);
	~ExtractUI();
	void init();
	
	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setMotionControler(MotionControler *ptr) { motionControler = ptr; }
	inline void setCameraControler(CameraControler *ptr) { cameraControler = ptr; }

	void initGraphicsView();
	void resetTemplateUI();
	void refreshCameraControler();
	void setPushButtonsEnabled(bool enable);

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
	void extractTemplateImages();

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNumberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//��ʼ��
	void on_pushButton_return_clicked();//���ؼ�
	void keyPressEvent(QKeyEvent *event);
	void update_extractState_extractor(int state);

	void on_moveForwardFinished_motion();//ǰ������
	void on_resetControlerFinished_motion();//��λ����
	void on_moveToInitialPosFinished_motion();//�����ʼ����λ��

	void on_initCamerasFinished_camera(int);//�����ʼ������
	void on_takePhotosFinished_camera(int);//���ս���
	void on_convertFinished_convertThread();//ͼ��ת������

	void mouseDoubleClickEvent(QMouseEvent *event);
	void on_recognizeFinished_serialNumUI();
	void do_showPreviousUI_serialNumUI();
	void on_getMaskRoiFinished_serialNumUI();
};
