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


//检测界面
class DetectUI : public QWidget
{
	Q_OBJECT

private:
	Ui::DetectUI ui;
	QString IconFolder; //图标文件夹
	QPixmap lightOnIcon; //亮灯图标 red
	QPixmap lightOffIcon; //灭灯图标 grey
	QPixmap defaultIcon; //默认图标
	SerialNumberUI *serialNumberUI; //产品序号识别界面

	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	MotionControler *motionControler; //运动控制器
	CameraControler *cameraControler; //相机控制器

	const qreal ItemSpacing = 0; //图元间距
	QSizeF itemSize; //图元尺寸
	QSizeF gridSize; //网格尺寸
	QGraphicsScene scene; //绘图场景
	QSizeF sceneSize; //场景尺寸
	pcb::ItemGrid itemGrid; //绘图网点
	pcb::ItemArray itemArray; //场景中的图元阵列

	int currentRow_show; //正在显示的某行图像的行号
	int eventCounter; //事件计数器
	pcb::CvMatArray cvmatSamples; //用于检测的样本图
	pcb::QPixmapArray qpixmapSamples;//用于显示的样本图
	ImgConvertThread imgConvertThread;//图像格式转换线程

	DetectThread *detectThread; //检测线程
	DefectDetecter *defectDetecter; //检测核心类
	pcb::DetectResult detectResult; //检测结果
	int detectState; //检测状态

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
	void initItemGrid(pcb::ItemGrid &grid);//初始化图元网格
	void initPointersInItemArray(pcb::ItemArray &items);//初始化itemArray
	void deletePointersInItemArray(pcb::ItemArray &items);//删除itemArray中的指针
	void initPointersInCvMatArray(pcb::CvMatArray &cvmats);//初始化CvMatArray
	void deletePointersInCvMatArray(pcb::CvMatArray &cvmats);//删除CvMatArray中的指针
	void initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//初始化QPixmapArray
	void deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//删除QPixmapArray中的指针
	void removeItemsFromGraphicsScene();//移除场景中的图元

	void showSampleImages();
	void readSampleImages();
	void detectSampleImages(); //检测

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
