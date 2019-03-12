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


//������
namespace Ui {
	class DetectUI;
}

class DetectUI : public QWidget
{
	Q_OBJECT

private:
	Ui::DetectUI ui;
	QString IconFolder; //ͼ���ļ���
	QPixmap lightOnIcon; //����ͼ�� red
	QPixmap lightOffIcon; //���ͼ�� grey
	Ui::DetectConfig *config; //��������
	Ui::DetectParams *params; //��ʱ����

	int nCamera; //�������
	int nPhotographing; //�������
	QString SampleDirPath; //sample�ļ��е�·�� 
	int currentRow_show; //��ǰ������ʾ����

	int itemSpacing; //ͼԪ���
	QSize itemSize; //ͼԪ�ߴ�
	QGraphicsScene scene; //��ͼ����
	QSize sceneSize; //�����ߴ�
	Ui::ItemGrid itemGrid; //��ͼ����
	Ui::ItemArray itemArray; //�����е�ͼԪ����

	bool isStarted; //ϵͳ�Ƿ�����������״̬
	int eventCounter; //�¼�������
	DetectThread *detectThread; //����߳�
	DetectCore *detectCore; //��������
	Ui::QImageArray samples; //��ǰ����
	Ui::DetectResult detectResult; //�����
	int detectState; //���״̬

public:
	DetectUI(QWidget *parent = Q_NULLPTR);
	~DetectUI();

	void setDetectConfig(Ui::DetectConfig *ptr);
	void setDetectParams(Ui::DetectParams *ptr);
	void initGraphicsView();

private:
	void readSampleImages();
	void showSampleImages();
	void detectSampleImages();

	void deletePointersInItemArray();
	void deletePointersInSampleImages();
	void removeItemsFromGraphicsScene();

Q_SIGNALS:
	void showDetectMainUI();
	void invalidNummberOfSampleImage();

private Q_SLOTS:
	void on_pushButton_start_clicked();
	void on_pushButton_return_clicked();
	void keyPressEvent(QKeyEvent *event);
	void nextRowOfSampleImages();
	void update_detectState_detectCore(int state);
	void on_detectFinished_detectThread(bool yes);
};
