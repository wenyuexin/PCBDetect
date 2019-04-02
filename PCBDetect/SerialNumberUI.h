#pragma once

#include "ui_SerialNumberUI.h"
#include "Configurator.h"
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QGraphicsScene>

class SerialNumberUI : public QWidget
{
	Q_OBJECT

private:
	Ui::SerialNumberUI ui;
	Ui::DetectParams *params; //����ʱ����ʱ����
	Ui::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	Ui::QPixmapArray *qpixmapSamples; //������ʾ������ͼ
	int gridRowIdx; //����ķ�ͼ�ڵڼ���
	int gridColIdx; //����ķ�ͼ�ڵڼ���
	
	QRect graphicsViewRect;
	QGraphicsScene graphicsScene;
	QGraphicsPixmapItem* item;

	QPoint mousePressPos; //������ʼ���λ��
	QPoint mouseReleasePos; //�����ͷ�λ��
	bool mousePress;
	bool mouseRelease;

	enum CaptureStatus { InitCapture, BeginCapture, BeginMove, CaptureFinished };
	int captureStatus = InitCapture;

public:
	SerialNumberUI(QWidget *parent = Q_NULLPTR);
	~SerialNumberUI();

	inline void setDetectParams(Ui::DetectParams *ptr = Q_NULLPTR) { params = ptr; }
	inline void setCvMatArray(Ui::CvMatArray *ptr = Q_NULLPTR) { cvmatSamples = ptr; }
	inline void setQPixmapArray(Ui::QPixmapArray *ptr = Q_NULLPTR) { qpixmapSamples = ptr; }
	inline void setGridIndex(int row, int col) { gridRowIdx = row; gridColIdx = col; }

	void showSampleImage(int gridRowIdx, int gridColIdx);

private:
	bool isPressPosInGraphicViewRect(QPoint mousePressPos);
	QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);


Q_SIGNALS:
	void recognizeFinished_serialNumberUI();
	void switchImage_serialNumberUI();

private Q_SLOTS:
	void on_pushButton_getROI_clicked();
	void on_pushButton_switchImage_clicked();
	void on_pushButton_recognize_clicked();
	void on_pushButton_return_clicked();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};