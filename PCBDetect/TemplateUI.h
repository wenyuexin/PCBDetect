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

//ģ���ȡ����
class TemplateUI : public QWidget
{
	Q_OBJECT

private:
	Ui::TemplateUI ui;
	Ui::DetectConfig *config; //��������
	Ui::DetectParams *params; //��ʱ����

	int itemSpacing; //ͼԪ���
	QSize itemSize; //ͼԪ�ߴ�
	QGraphicsScene scene; //��ͼ����
	QSize sceneSize; //�����ߴ�
	Ui::ItemGrid itemGrid; //��ͼ����
	Ui::ItemArray itemArray; //�����е�ͼԪ����

	int currentRow_show; //������ʾ��ĳ��ͼ����к�
	int eventCounter; //�¼�������
	Ui::QImageArray samples; //��ǰPCB����ͼ
	TemplateThread *templThread; //ģ����ȡ�߳�
	TemplateExtractor *templExtractor; //ģ����ȡ��

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

	void initPointersInItemArray();//��ʼ��itemArray
	void deletePointersInItemArray();//ɾ��itemArray�е�ָ��
	void initPointersInSampleImages();//��ʼ��sampleImages
	void deletePointersInSampleImages();//ɾ��sampleImages�е�ָ��
	void removeItemsFromGraphicsScene();//�Ƴ������е�ͼԪ

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
