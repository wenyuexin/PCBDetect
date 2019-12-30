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


//复查界面
class RecheckUI : public QWidget
{
	Q_OBJECT

public:
	enum RecheckStatus {
		NoError,
		Unchecked,
		CurrentBatchRechecked, //当前批次已复查完成
		InvalidFullImageName, //整图的文件名无效
		FullImageNotFound, //没有找到整图
		LoadFullImageFailed, //整图加载失败
		FlawImageNotFound, //没有找到缺陷图
		LoadFlawImageFailed, //缺陷图加载失败
		OpenFlawImageFolderFailed, //无法打开缺陷图所在的文件夹
		Default
	};

private:
	Ui::RecheckUI ui;
	pcb::UserConfig *userConfig; //检修系统配置信息
	pcb::RuntimeParams *runtimeParams; //运行参数
	RecheckStatus recheckStatus; //复查状态
	pcb::DetectResult *detectResult; //检测结果

	QPixmap fullImage; //PCB大图
	const QString fullImageNamePrefix = "fullImage"; //文件名前缀
	QSize originalFullImageSize; //PCB大图的原始尺寸
	QSize fullImageItemSize; //PCB大图的显示尺寸
	qreal scaledFactor; //PCB大图的尺寸变换因子
	QGraphicsScene fullImageScene; //显示PCB大图的场景
	FlickeringArrow flickeringArrow; //PCB大图上闪烁的箭头

	QString IconFolder; //图标文件夹
	QPixmap lightOnIcon; //亮灯图标 red
	QPixmap lightOffIcon; //灭灯图标 grey

	int defectNum; //缺陷总数
	int defectIndex; //当前正在显示的缺陷图
	//QVector<pcb::FlawImageInfo> flawImageInfoVec; //缺陷图的信息
	uint16_t flawIndicatorStatus; //指示灯亮灭状态

	//pcb::FolderHierarchy OutFolderHierarchy; //输出目录下的文件夹层次

	//FileSyncThread fileSyncThread;//文件同步线程

public:
	RecheckUI(QWidget *parent = Q_NULLPTR);
	~RecheckUI();

	void init(); //界面初始化
	inline void setUserConfig(pcb::UserConfig *ptr) { userConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setDetectResult(pcb::DetectResult *ptr) { detectResult = ptr; }

private:
	void reset(); //界面重置
	void refresh(); //更新复查界面

	void showFullImage(); //显示PCB整图
	void initFlickeringArrow(); //加载初始的闪烁箭头
	void setFlickeringArrowPos(); //更新箭头的位置
	void showLastFlawImage(); //显示上一张缺陷图
	void showNextFlawImage(); //显示下一张缺陷图

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
	void keyPressEvent(QKeyEvent *event); //敲击键盘事件
	void on_pushButton_plus2_clicked(); //点击加号按键
	void on_pushButton_minus2_clicked(); //点击减号按键
	void on_pushButton_return_clicked(); //点击退出按键
	void on_refreshArrow_arrow();
};
