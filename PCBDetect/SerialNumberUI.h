#pragma once

#include "ui_SerialNumberUI.h"
#include "Configurator.h"
#include "RuntimeLibrary.h"
#include <allheaders.h>
#include <capi.h>
#include <QWidget>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QButtonGroup>
#include <QRegExpValidator>


//��Ʒ���ʶ�����
class SerialNumberUI : public QWidget
{
	Q_OBJECT

public:
	enum ErrorCode {
		NoError = 0x000,
		Uncheck = 0x700,
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

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::DetectParams *detectParams; //����ʱ����ʱ����
	pcb::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	pcb::QPixmapArray *qpixmapSamples; //������ʾ������ͼ
	int gridRowIdx; //����ķ�ͼ�ڵڼ���
	int gridColIdx; //����ķ�ͼ�ڵڼ���
	
	QRect graphicsViewRect;
	QPointF graphicsScenePos;
	QGraphicsScene graphicsScene;
	QGraphicsPixmapItem* imageItem;

	QButtonGroup checkBoxGroup;
	QPointF roiRect_tl;
	QPointF roiRect_br;
	double imageScalingRatio;

	QPoint mousePressPos; //������ʼ���λ��
	QPoint mouseReleasePos; //�����ͷ�λ��
	bool mousePress;
	bool mouseRelease;

	enum CaptureStatus { InitCapture, BeginCapture, BeginMove, CaptureFinished };
	int captureStatus = InitCapture;

	TessBaseAPI *handle;
	QString roiFilePath;
	QString serialNum; //��Ʒ���

public:
	SerialNumberUI(QWidget *parent = Q_NULLPTR);
	~SerialNumberUI();

	inline void setAdminConfig(pcb::AdminConfig *ptr) { adminConfig = ptr; }
	inline void setDetectParams(pcb::DetectParams *ptr) { detectParams = ptr; }
	inline void setCvMatArray(pcb::CvMatArray *ptr) { cvmatSamples = ptr; }
	inline void setQPixmapArray(pcb::QPixmapArray *ptr) { qpixmapSamples = ptr; }
	inline void setGridIndex(int row, int col) { gridRowIdx = row; gridColIdx = col; }

	void showSampleImage(int gridRowIdx, int gridColIdx);

private:
	bool isPressPosInGraphicViewRect(QPoint mousePressPos);
	QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);

	void initSerialNumberUI();
	void initCheckBoxGroup();
	void setSerialNumberUIEnabled(bool);

	void deleteImageItem();
	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

Q_SIGNALS:
	void switchImage_serialNumUI();
	void recognizeFinished_serialNumUI();
	void showPreviousUI_serialNumUI();

private Q_SLOTS:
	void on_pushButton_getROI_clicked();
	void on_pushButton_confirm_clicked();
	void on_pushButton_recognize_clicked();
	void on_pushButton_return_clicked();

	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
