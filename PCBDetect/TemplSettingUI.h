#pragma once

#include "ui_TemplSettingUI.h"
#include "Configurator.h"
#include "RuntimeParams.h"
#include "pcbFuncLib.h"
#include "ImageConverter.h"
#include <allheaders.h>
#include <capi.h>
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QButtonGroup>
#include <QRegExpValidator>


//产品序号识别界面
class TemplSettingUI : public QWidget
{
	Q_OBJECT

public:
	enum ErrorCode {
		NoError = 0x000,
		Unchecked = 0x700,
		InitFailed = 0x701,
		Invalid_RoiRect = 0x702,
		Invalid_RoiData = 0x703,
		RecognizeFailed = 0x704,
		Invalid_SerialNum = 0x705,
		Default = 0x7FF
	};

private:
	Ui::TemplSettingUI ui;
	QRegExpValidator *NumberValidator; //限定于数字的正则表达式
	QRegExpValidator *SegThreshValidator; //0-255
	ErrorCode errorCode; //错误代码

	pcb::AdminConfig *adminConfig; //系统参数
	pcb::RuntimeParams *runtimeParams; //运行时的临时参数
	pcb::CvMatArray *cvmatSamples; //用于检测的样本图
	pcb::QPixmapArray *qpixmapSamples; //用于显示的样本图
	int gridRowIdx; //点击的分图在第几列
	int gridColIdx; //点击的分图在第几行
	
	QRect graphicsViewRect;
	QGraphicsScene graphicsScene;//绘图场景
	QPointF graphicsScenePos;//场景左上角点在屏幕中的位置
	double imageScalingRatio; //图像缩放比例
	QGraphicsPixmapItem *imageItem;//图元
	ImageConverter *imageConverter; //图像转换器
	QTime lastImageRefreshTime; //上一次手动刷新图像的时间
	cv::Mat currentCvMatImage; //当前正在显示的图

	bool maskRoiFlag; //模板区域坐标是否已经设置
	bool segThreshFlag; //图像分割阈值是否已经设置
	bool productIdFlag; //产品序号是否已经设置

	QButtonGroup checkBoxGroup;
	QPointF maskRoi_tl; //掩膜左上角的坐标
	QPointF maskRoi_br; //掩膜右下角的坐标

	QPoint mousePressPos; //鼠标的起始点击位置
	QPoint mouseReleasePos; //鼠标的释放位置
	bool mousePress;
	bool mouseRelease;
	enum CaptureStatus { InitCapture, BeginCapture, BeginMove, CaptureFinished };
	int captureStatus = InitCapture;

public:
	TemplSettingUI(QWidget *parent = Q_NULLPTR);
	~TemplSettingUI();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setCvMatArray(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setQPixmapArray(pcb::QPixmapArray *ptr) { qpixmapSamples = ptr; }
	inline void setGridIndex(int row, int col) { gridRowIdx = row; gridColIdx = col; }
	inline bool isReadyForExtract() { return maskRoiFlag && segThreshFlag && productIdFlag; }

	void showSampleImage(int row, int col);
	void reset();

private:
	bool isPressPosInGraphicViewRect(QPoint mousePressPos);
	QRect getRect(const QPoint &point1, const QPoint &point2);

	void initGraphicsView(); //初始化
	void initMaskRoiWidgets();
	void initSegThreshWidgets();

	void resetMaskRoiWidgets(); //重置
	void resetSegThreshWidgets();

	void setTemplSettingUIEnabled(bool);
	//void setPushButtonsEnabled(bool);

	double intervalCensored(double num, double minVal, double maxVal);
	void showImageDividedByThresh(int thresh); //显示分割后的结果
	void showSampleImage(const QPixmap &img);
	void deleteImageItem(); //删除图元

	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

Q_SIGNALS:
	void segThreshIsSet_templSettingUI();
	void modelTypeIsSet_templSettingUI();
	void switchImage_templSettingUI();
	void settingFinished_templSettingUI(); //确认
	void showExtractUI_templSettingUI(); //返回

private Q_SLOTS:
	void on_pushButton_maskRoi_clicked(); //确认模板区域

	void on_checkBox_segThresh_clicked(); //阈值确认框发生变化
	void on_horizontalSlider_segThresh_changed(int); //阈值滑条发生变化
	void on_pushButton_segThresh_clicked(); //确认阈值

	void on_pushButton_modelType_clicked(); //确认型号

	//void on_pushButton_confirm_clicked(); //确认并返回
	void on_pushButton_return_clicked(); //返回

	void mousePressEvent(QMouseEvent *event); //鼠标事件
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
