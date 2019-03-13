#pragma once

#include <QWidget>
#include "ui_TemplateUI.h"
#include "Configurator.h"
#include "TemplateThread.h"
#include <QDesktopWidget>
#include <QKeyEvent>


namespace Ui {
	class TemplateUI;
}

//模板获取界面
class TemplateUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	Ui::DetectConfig *config; //参数配置
	Ui::DetectParams *params; //临时参数

	int itemSpacing; //图元间距
	QSize itemSize; //图元尺寸
	QGraphicsScene scene; //绘图场景
	QSize sceneSize; //场景尺寸
	Ui::ItemGrid itemGrid; //绘图网点
	Ui::ItemArray itemArray; //场景中的图元阵列

	int currentRow_show; //正在显示的某行图像的行号
	int eventCounter; //事件计数器
	Ui::QImageArray samples; //当前PCB样本图
	TemplateThread *templThread; //模板提取线程
	TemplateExtractor *templExtractor; //模板提取器

public:
	TemplateUI(QWidget *parent = Q_NULLPTR);
	~TemplateUI();
	
	inline void setDetectConfig(Ui::DetectConfig* ptr = Q_NULLPTR) { config = ptr; }
	inline void setDetectParams(Ui::DetectParams* ptr = Q_NULLPTR) { params = ptr; }
	void initGraphicsView();

private:
	void readSampleImages();
	void showSampleImages();
	void extractTemplateImages();

	void initPointersInItemArray();//初始化itemArray
	void deletePointersInItemArray();//删除itemArray中的指针
	void initPointersInSampleImages();//初始化sampleImages
	void deletePointersInSampleImages();//删除sampleImages中的指针
	void removeItemsFromGraphicsScene();//移除场景中的图元

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_getTempl_clicked();
	void on_pushButton_clear_clicked();
	void on_pushButton_return_clicked();
	void keyPressEvent(QKeyEvent *event);
	void nextRowOfSampleImages();
	void update_extractState_extractor(int state);
};
