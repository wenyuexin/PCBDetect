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


//模板提取界面
class ExtractUI : public QWidget
{
	Q_OBJECT

private:
	Ui::ExtractUI ui;
	TemplSettingUI *templSettingUI; //模板设置界面

	pcb::AdminConfig *adminConfig; //系统参数
	pcb::UserConfig *userConfig; //用户参数
	pcb::RuntimeParams *runtimeParams; //运行参数
	MotionControler *motionControler; //运动控制器
	CameraControler *cameraControler; //相机控制器
	int initCounter; //初始化次数的计数器

	const qreal ItemSpacing = 0; //图元间距（单位:pix）
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
	ExtractThread *extractThread; //模板提取线程
	TemplateExtractor *templExtractor; //模板提取器

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
	void showCurrentTime();//获取并显示时间
	void initItemGrid(pcb::ItemGrid &grid);//初始化图元网格
	void initPointersInItemArray(pcb::ItemArray &items);//初始化itemArray
	void deletePointersInItemArray(pcb::ItemArray &items);//删除itemArray中的指针
	void initPointersInCvMatArray(pcb::CvMatArray &cvmatArray);//初始化CvMatArray
	void deletePointersInCvMatArray(pcb::CvMatArray &cvmatArray);//删除CvMatArray中的指针
	void initPointersInQPixmapArray(pcb::QPixmapArray &qpixmapArray);//初始化QPixmapArray
	void deletePointersInQPixmapArray(pcb::QPixmapArray &qpixmapArray);//删除QPixmapArray的中指针
	void removeItemsFromGraphicsScene();//移除场景中的图元

	void showSampleImages();
	void readSampleImages();
	void extractTemplateImages();
	bool extractionHasNotStartedAndAllImagesHasBeenDisplayed();

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNumberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();//开始键
	void on_pushButton_return_clicked();//返回键
	void keyPressEvent(QKeyEvent *event); //键盘事件
	void mouseDoubleClickEvent(QMouseEvent *event); //鼠标双击事件
	void mousePressEvent(QMouseEvent *event); //鼠标单击事件
	void update_extractState_extractor(int state);

	void on_moveToInitialPosFinished_motion(int);//到达初始拍照位置
	void on_moveForwardFinished_motion(int);//前进结束
	void on_motionResetFinished_motion(int);//复位结束

	void on_initCamerasFinished_camera(int);//相机初始化结束
	void on_takePhotosFinished_camera(int);//拍照结束
	void on_convertFinished_convertThread();//图像转换结束

	void on_maskRoiIsSet_templSettingUI();
	void on_segThreshIsSet_templSettingUI();
	void on_modelNumIsSet_templSettingUI();
	void do_showExtractUI_templSettingUI();
	void on_settingFinished_templSettingUI();
};
