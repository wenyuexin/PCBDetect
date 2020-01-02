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


//��Ʒ���ʶ�����
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
	QRegExpValidator *NumberValidator; //�޶������ֵ�������ʽ
	QRegExpValidator *SegThreshValidator; //0-255
	ErrorCode errorCode; //�������

	pcb::AdminConfig *adminConfig; //ϵͳ����
	pcb::RuntimeParams *runtimeParams; //����ʱ����ʱ����
	pcb::CvMatArray *cvmatSamples; //���ڼ�������ͼ
	pcb::QPixmapArray *qpixmapSamples; //������ʾ������ͼ
	int gridRowIdx; //����ķ�ͼ�ڵڼ���
	int gridColIdx; //����ķ�ͼ�ڵڼ���
	
	QRect graphicsViewRect;
	QGraphicsScene graphicsScene;//��ͼ����
	QPointF graphicsScenePos;//�������Ͻǵ�����Ļ�е�λ��
	double imageScalingRatio; //ͼ�����ű���
	QGraphicsPixmapItem *imageItem;//ͼԪ
	ImageConverter *imageConverter; //ͼ��ת����
	QTime lastImageRefreshTime; //��һ���ֶ�ˢ��ͼ���ʱ��
	cv::Mat currentCvMatImage; //��ǰ������ʾ��ͼ

	bool maskRoiFlag; //ģ�����������Ƿ��Ѿ�����
	bool segThreshFlag; //ͼ��ָ���ֵ�Ƿ��Ѿ�����
	bool productIdFlag; //��Ʒ����Ƿ��Ѿ�����

	QButtonGroup checkBoxGroup;
	QPointF maskRoi_tl; //��Ĥ���Ͻǵ�����
	QPointF maskRoi_br; //��Ĥ���½ǵ�����

	QPoint mousePressPos; //������ʼ���λ��
	QPoint mouseReleasePos; //�����ͷ�λ��
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

	void initGraphicsView(); //��ʼ��
	void initMaskRoiWidgets();
	void initSegThreshWidgets();

	void resetMaskRoiWidgets(); //����
	void resetSegThreshWidgets();

	void setTemplSettingUIEnabled(bool);
	//void setPushButtonsEnabled(bool);

	double intervalCensored(double num, double minVal, double maxVal);
	void showImageDividedByThresh(int thresh); //��ʾ�ָ��Ľ��
	void showSampleImage(const QPixmap &img);
	void deleteImageItem(); //ɾ��ͼԪ

	bool showMessageBox(QWidget *parent, ErrorCode code = Default); //��������

Q_SIGNALS:
	void segThreshIsSet_templSettingUI();
	void modelTypeIsSet_templSettingUI();
	void switchImage_templSettingUI();
	void settingFinished_templSettingUI(); //ȷ��
	void showExtractUI_templSettingUI(); //����

private Q_SLOTS:
	void on_pushButton_maskRoi_clicked(); //ȷ��ģ������

	void on_checkBox_segThresh_clicked(); //��ֵȷ�Ͽ����仯
	void on_horizontalSlider_segThresh_changed(int); //��ֵ���������仯
	void on_pushButton_segThresh_clicked(); //ȷ����ֵ

	void on_pushButton_modelType_clicked(); //ȷ���ͺ�

	//void on_pushButton_confirm_clicked(); //ȷ�ϲ�����
	void on_pushButton_return_clicked(); //����

	void mousePressEvent(QMouseEvent *event); //����¼�
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
