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
	pcb::DetectConfig *config; //��������
	pcb::DetectParams *params; //��ʱ����
	int currentRow_show; //��ǰ������ʾ����

	int itemSpacing = 3; //ͼԪ���
	QSize itemSize; //ͼԪ�ߴ�
	QGraphicsScene scene; //��ͼ����
	QSize sceneSize; //�����ߴ�
	pcb::ItemGrid itemGrid; //��ͼ����
	pcb::ItemArray itemArray; //�����е�ͼԪ����

	bool isStarted; //ϵͳ�Ƿ�����������״̬
	int eventCounter; //�¼�������
	DetectThread *detectThread; //����߳�
	DetectCore *detectCore; //��������
	pcb::QImageArray samples; //��ǰ����
	pcb::DetectResult detectResult; //�����
	int detectState; //���״̬

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

	void initItemGrid();//��ʼ��ͼԪ����
	void initPointersInItemArray();//��ʼ��itemArray
	void deletePointersInItemArray();//ɾ��itemArray�е�ָ��
	void initPointersInSampleImages();//��ʼ��sampleImages
	void deletePointersInSampleImages();//ɾ��sampleImages�е�ָ��
	void removeItemsFromGraphicsScene();//�Ƴ������е�ͼԪ

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
