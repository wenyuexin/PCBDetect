#pragma once

#include <QWidget>
#include "ui_ExtractUI.h"
#include "Configurator.h"
#include "ExtractThread.h"
#include "ImgConvertThread.h"
#include "MotionControler.h"
#include "CameraControler.h"
#include "TemplSettingUI.h"
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QTimer>


//ģ����ȡ����
class ExtractUI : public QWidget
{
	Q_OBJECT

private:
	Ui::ExtractUI ui;
	TemplSettingUI *templSettingUI; //ģ�����ý���

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::UserConfig *userConfig; //�û�����
	pcb::RuntimeParams *runtimeParams; //���в���
	MotionControler *motionControler; //�˶�������
	CameraControler *cameraControler; //���������
	int initCounter; //��ʼ�������ļ�����

	const qreal ItemSpacing = 0; //ͼԪ��ࣨ��λ:pix��
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
	void reset();
	void refreshCameraControler();
	void setPushButtonsEnabled(bool enable);

private:
	void showCurrentTime();//��ȡ����ʾʱ��
	void initItemGrid(pcb::ItemGrid &grid);//��ʼ��ͼԪ����
	void initPointersInItemArray(pcb::ItemArray &items);//��ʼ��itemArray
	void deletePointersInItemArray(pcb::ItemArray &items);//ɾ��itemArray�е�ָ��
	void initPointersInCvMatArray(pcb::CvMatArray &cvmats);//��ʼ��CvMatArray
	void deletePointersInCvMatArray(pcb::CvMatArray &cvmats);//ɾ��CvMatArray�е�ָ��
	void initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//��ʼ��QPixmapArray
	void deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//ɾ��QPixmapArray����ָ��
	void removeItemsFromGraphicsScene();//�Ƴ������е�ͼԪ

	void showSampleImages();
	void readSampleImages();
	void extractTemplateImages();
	bool extractionHasNotStartedAndAllImagesHasBeenDisplayed();

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNumberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//��ʼ��
	void on_pushButton_return_clicked();//���ؼ�
	void keyPressEvent(QKeyEvent *event); //�����¼�
	void mouseDoubleClickEvent(QMouseEvent *event); //���˫���¼�
	void mousePressEvent(QMouseEvent *event); //��굥���¼�
	void update_extractState_extractor(int state);

	void on_moveToInitialPosFinished_motion(int);//�����ʼ����λ��
	void on_moveForwardFinished_motion(int);//ǰ������
	void on_motionResetFinished_motion(int);//��λ����

	void on_initCamerasFinished_camera(int);//�����ʼ������
	void on_takePhotosFinished_camera(int);//���ս���
	void on_convertFinished_convertThread();//ͼ��ת������

	void on_maskRoiIsSet_templSettingUI();
	void on_segThreshIsSet_templSettingUI();
	void on_modelNumIsSet_templSettingUI();
	void do_showExtractUI_templSettingUI();
	void on_settingFinished_templSettingUI();
};
