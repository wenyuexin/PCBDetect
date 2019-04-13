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


//模板提取界面
class TemplateUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	SerialNumberUI serialNumberUI; //产品序号识别界面
	pcb::AdminConfig *adminConfig; //系统参数
	pcb::DetectConfig *detectConfig; //用户参数
	pcb::DetectParams *detectParams; //运行参数
	MotionControler *motionControler; //运动控制器
	CameraControler *cameraControler; //相机控制器

	const int itemSpacing = 3; //图元间距
	QSize itemSize; //图元尺寸
	QSize gridSize; //网格尺寸
	QGraphicsScene scene; //绘图场景
	QSize sceneSize; //场景尺寸
	pcb::ItemGrid itemGrid; //绘图网点
	pcb::ItemArray itemArray; //场景中的图元阵列

	int currentRow_show; //正在显示的某行图像的行号
	int eventCounter; //事件计数器
	pcb::CvMatArray cvmatSamples; //用于检测的样本图
	pcb::QPixmapArray qpixmapSamples;//用于显示的样本图
	ImgConvertThread imgConvertThread;//图像格式转换线程
	TemplateThread *templThread; //模板提取线程
	TemplateExtractor *templExtractor; //模板提取器

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
	void initItemGrid(pcb::ItemGrid &grid);//初始化图元网格
	void initPointersInItemArray(pcb::ItemArray &items);//初始化itemArray
	void deletePointersInItemArray(pcb::ItemArray &items);//删除itemArray中的指针
	void initPointersInCvMatArray(pcb::CvMatArray &cvmats);//初始化CvMatArray
	void deletePointersInCvMatArray(pcb::CvMatArray &cvmats);//删除CvMatArray中的指针
	void initPointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//初始化QPixmapArray
	void deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmaps);//删除QPixmapArray中的指针
	void removeItemsFromGraphicsScene();//移除场景中的图元

	void showSampleImages();
	void nextRowOfSampleImages();
	void readSampleImages2();
	void nextRowOfSampleImages2();
	void extractTemplateImages();

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//开始键
	void on_pushButton_return_clicked();//返回键
	void keyPressEvent(QKeyEvent *event);
	void update_extractState_extractor(int state);

	void on_moveForwardFinished_motion();//前进结束
	void on_resetControlerFinished_motion(int);//复位结束
	void on_initCamerasFinished_camera(int);//相机初始化结束
	void on_takePhotosFinished_camera(int);//拍照结束
	void on_convertFinished_convertThread();//图像转换结束

	void mouseDoubleClickEvent(QMouseEvent *event);
	void on_switchImage_serialNumUI();
	void on_recognizeFinished_serialNumUI();
	void do_showPreviousUI_serialNumUI();
};
