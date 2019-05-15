#pragma once

#include "ui_SerialNumberUI.h"
#include "Configurator.h"
#include "RuntimeLib.h"
#include <allheaders.h>
#include <capi.h>
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QButtonGroup>
#include <QRegExpValidator>


//产品序号识别界面
class SerialNumberUI : public QWidget
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
	Ui::SerialNumberUI ui;
	ErrorCode errorCode;

	pcb::AdminConfig *adminConfig; //系统参数
	pcb::RuntimeParams *runtimeParams; //运行时的临时参数
	pcb::CvMatArray *cvmatSamples; //用于检测的样本图
	pcb::QPixmapArray *qpixmapSamples; //用于显示的样本图
	int gridRowIdx; //点击的分图在第几列
	int gridColIdx; //点击的分图在第几行
	
	QRect graphicsViewRect;
	QGraphicsScene graphicsScene;//绘图场景
	QPointF graphicsScenePos;//场景左上角点在屏幕中的位置
	QGraphicsPixmapItem *imageItem;//图元

	QButtonGroup checkBoxGroup;
	QPointF roiRect_tl;
	QPointF roiRect_br;
	double imageScalingRatio;

	QPoint mousePressPos; //鼠标的起始点击位置
	QPoint mouseReleasePos; //鼠标的释放位置
	bool mousePress;
	bool mouseRelease;

	enum CaptureStatus { InitCapture, BeginCapture, BeginMove, CaptureFinished };
	int captureStatus = InitCapture;

	TessBaseAPI *ocrHandle;
	QString roiFilePath;

public:
	SerialNumberUI(QWidget *parent = Q_NULLPTR);
	~SerialNumberUI();
	void init();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setRuntimeParams(pcb::RuntimeParams *ptr) { runtimeParams = ptr; }
	inline void setCvMatArray(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setQPixmapArray(pcb::QPixmapArray *ptr) { qpixmapSamples = ptr; }
	inline void setGridIndex(int row, int col) { gridRowIdx = row; gridColIdx = col; }

	void showSampleImage(int row, int col);
	void reset();

private:
	bool isPressPosInGraphicViewRect(QPoint mousePressPos);
	QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);

	void initCheckBoxGroup();
	void setSerialNumberUIEnabled(bool);
	void setPushButtonsEnabled(bool);

	void deleteImageItem();
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //弹窗警告

Q_SIGNALS:
	void switchImage_serialNumUI();
	void recognizeFinished_serialNumUI();
	void showPreviousUI_serialNumUI();

private Q_SLOTS:
	void on_pushButton_getROI_clicked();
	void on_pushButton_recognize_clicked();
	void on_pushButton_confirm_clicked();
	void on_pushButton_return_clicked();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
