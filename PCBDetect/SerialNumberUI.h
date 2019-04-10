#pragma once

#include "ui_SerialNumberUI.h"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QGraphicsScene>


//产品序号识别界面
class SerialNumberUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SerialNumberUI ui;
	pcb::DetectParams *params; //运行时的临时参数
	pcb::CvMatArray *cvmatSamples; //用于检测的样本图
	pcb::QPixmapArray *qpixmapSamples; //用于显示的样本图
	int gridRowIdx; //点击的分图在第几列
	int gridColIdx; //点击的分图在第几行
	
	QRect graphicsViewRect;
	QGraphicsScene graphicsScene;
	QGraphicsPixmapItem* item;

	QPoint mousePressPos; //鼠标的起始点击位置
	QPoint mouseReleasePos; //鼠标的释放位置
	bool mousePress;
	bool mouseRelease;

	enum CaptureStatus { InitCapture, BeginCapture, BeginMove, CaptureFinished };
	int captureStatus = InitCapture;

public:
	SerialNumberUI(QWidget *parent = Q_NULLPTR);
	~SerialNumberUI();

	inline void setDetectParams(pcb::DetectParams *ptr = Q_NULLPTR) { params = ptr; }
	inline void setCvMatArray(pcb::CvMatArray *ptr = Q_NULLPTR) { cvmatSamples = ptr; }
	inline void setQPixmapArray(pcb::QPixmapArray *ptr = Q_NULLPTR) { qpixmapSamples = ptr; }
	inline void setGridIndex(int row, int col) { gridRowIdx = row; gridColIdx = col; }

	void showSampleImage(int gridRowIdx, int gridColIdx);

private:
	bool isPressPosInGraphicViewRect(QPoint mousePressPos);
	QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);


Q_SIGNALS:
	void recognizeFinished_serialNumUI();
	void switchImage_serialNumUI();

private Q_SLOTS:
	void on_pushButton_getROI_clicked();
	void on_pushButton_switchImage_clicked();
	void on_pushButton_recognize_clicked();
	void on_pushButton_return_clicked();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
