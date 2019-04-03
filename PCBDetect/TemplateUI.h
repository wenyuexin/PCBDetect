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

//模板提取界面
class TemplateUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	SerialNumberUI serialNumberUI; //产品序号识别界面
	Ui::DetectConfig *detectConfig; //参数配置
	Ui::DetectParams *detectParams; //临时参数
	MotionControler *motionControler; //运动控制器
	CameraControler *cameraControler; //相机控制器

	int itemSpacing = 3; //图元间距
	QSize itemSize; //图元尺寸
	QSize gridSize; //网格尺寸
	QGraphicsScene scene; //绘图场景
	QSize sceneSize; //场景尺寸
	Ui::ItemGrid itemGrid; //绘图网点
	Ui::ItemArray itemArray; //场景中的图元阵列

	int currentRow_show; //正在显示的某行图像的行号
	int eventCounter; //事件计数器
	Ui::CvMatArray cvmatSamples; //用于检测的样本图
	Ui::QPixmapArray qpixmapSamples;//用于显示的样本图
	ImgConvertThread imgConvertThread;//图像格式转换线程
	TemplateThread *templThread; //模板提取线程
	TemplateExtractor *templExtractor; //模板提取器

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

	void initItemGrid(Ui::ItemGrid &grid);//初始化图元网格
	void initPointersInItemArray(Ui::ItemArray &items);//初始化itemArray
	void deletePointersInItemArray(Ui::ItemArray &items);//删除itemArray中的指针
	void initPointersInCvMatArray(Ui::CvMatArray &cvmats);//初始化CvMatArray
	void deletePointersInCvMatArray(Ui::CvMatArray &cvmats);//删除CvMatArray中的指针
	void initPointersInQPixmapArray(Ui::QPixmapArray &qpixmaps);//初始化QPixmapArray
	void deletePointersInQPixmapArray(Ui::QPixmapArray &qpixmaps);//删除QPixmapArray中的指针
	void removeItemsFromGraphicsScene();//移除场景中的图元

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//开始键
	void on_pushButton_return_clicked();//返回键
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
