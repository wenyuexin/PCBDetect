#pragma once

#include <QWidget>
#include "ui_DetectUI.h"
#include "Configurator.h"
#include "DetectThread.h"
#include <vector>
#include <QRect>
#include <QList>
#include <QGraphicsItem>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QDebug>


//检测界面
namespace Ui {
	class DetectUI;
}

class DetectUI : public QWidget
{
	Q_OBJECT

private:
	Ui::DetectUI ui;
	QString IconFolder; //图标文件夹
	QPixmap lightOnIcon; //亮灯图标 red
	QPixmap lightOffIcon; //灭灯图标 grey
	pcb::DetectConfig *config; //参数配置
	pcb::DetectParams *params; //临时参数
	int currentRow_show; //当前正在显示的行

	int itemSpacing = 3; //图元间距
	QSize itemSize; //图元尺寸
	QGraphicsScene scene; //绘图场景
	QSize sceneSize; //场景尺寸
	pcb::ItemGrid itemGrid; //绘图网点
	pcb::ItemArray itemArray; //场景中的图元阵列

	bool isStarted; //系统是否处于已启动的状态
	int eventCounter; //事件计数器
	DetectThread *detectThread; //检测线程
	DetectCore *detectCore; //检测核心类
	pcb::QImageArray samples; //当前样本
	pcb::DetectResult detectResult; //检测结果
	int detectState; //检测状态

public:
	DetectUI(QWidget *parent = Q_NULLPTR);
	~DetectUI();

	inline void setDetectConfig(pcb::DetectConfig* ptr = Q_NULLPTR) { config = ptr; }
	inline void setDetectParams(pcb::DetectParams* ptr = Q_NULLPTR) { params = ptr; }

	void initGraphicsView();
	void resetDetectUI();

private:
	void readSampleImages();
	void showSampleImages();
	void detectSampleImages();

	void initItemGrid();//初始化图元网格
	void initPointersInItemArray();//初始化itemArray
	void deletePointersInItemArray();//删除itemArray中的指针
	void initPointersInSampleImages();//初始化sampleImages
	void deletePointersInSampleImages();//删除sampleImages中的指针
	void removeItemsFromGraphicsScene();//移除场景中的图元

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();
	void on_pushButton_clear_clicked();
	void on_pushButton_return_clicked();

	void keyPressEvent(QKeyEvent *event);
	void nextRowOfSampleImages();
	void update_detectState_detectCore(int state);
	void on_detectFinished_detectThread(bool yes);
};
