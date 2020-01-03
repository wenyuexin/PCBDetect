#pragma once

#include <QGraphicsItem>
#include <QTimer>
#include <QObject>
#include <QPainter>
#include <QDebug>

//PCB��ͼ����˸�ļ�ͷ
class FlickeringArrow :
	public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

private:
	QColor brushColor;
	QTimer *timer;
	bool Flash = false;
	QSize *originalFullImageSize;

	//�����ͷ�Ĵ�С����״
	qreal sizeFactor; //��ͷ�ߴ����ӣ����Ƽ�ͷ��С
	qreal basicArrowLength; //��ͷ�Ļ�������
	qreal basicArrowWidth; //��ͷ�Ļ������
	QPointF *arrowShapeArray; //������ͷ��״�ĵ�

public:
	FlickeringArrow();
	~FlickeringArrow();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	inline void setColor(const QColor &color) { brushColor = color; }
	inline void setFullImageSize(QSize *ptr) { originalFullImageSize = ptr; }

	void startFlickering(int msec = 500);//��ʼ��˸
	void stopFlickering();//ֹͣ��˸

Q_SIGNALS:
	void refreshArrow_arrow();//֪ͨ������ˢ�¼�ͷ

private Q_SLOTS:
	void on_timeOut_timer();
};

