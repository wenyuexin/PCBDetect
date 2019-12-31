#include "FlickeringArrow.h"


FlickeringArrow::FlickeringArrow()
{
	brushColor = Qt::red;
	Flash = true;

	//�����ͷ�Ĵ�С����״ ��
	sizeFactor = 1.7; //��ͷ�ߴ����ӣ����Ƽ�ͷ��С
	basicArrowLength = 20; //��ͷ�Ļ�������
	basicArrowWidth = 12; //��ͷ�Ļ������
	arrowShapeArray =  new QPointF[7] {
		sizeFactor * QPointF(1.0, 0.0),
		sizeFactor * QPointF(15.0, basicArrowWidth / 2),
		sizeFactor * QPointF(13.0, 2.0),
		sizeFactor * QPointF(basicArrowLength, 2.0), //�Գ�
		sizeFactor * QPointF(basicArrowLength, -2.0),
		sizeFactor * QPointF(13.0, -2.0),
		sizeFactor * QPointF(15.0, -basicArrowWidth / 2)
	};

	timer = new QTimer; //����ˢ����˸�ļ�ͷ
	connect(timer, SIGNAL(timeout()), this, SLOT(on_timeOut_timer()));
}


FlickeringArrow::~FlickeringArrow()
{
	qDebug() << "~FlickeringArrow";
	delete[] arrowShapeArray;
	arrowShapeArray = Q_NULLPTR;
	delete timer;
	timer = Q_NULLPTR;
}


/**************** ���Ƽ�ͷ *******************/

QRectF FlickeringArrow::boundingRect() const
{
	qreal adjust = 0.5;
	qreal itemSize = basicArrowLength * sizeFactor;
	return QRectF(-itemSize-adjust, -itemSize-adjust, 2*itemSize+adjust, 2*itemSize+adjust);
}


void FlickeringArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	if (Flash) {
		painter->setPen(QPen(QColor(0, 0, 0))); //�ڱ�
		painter->setBrush(QBrush(Qt::white, Qt::SolidPattern)); //�׵�
	}
	else {
		painter->setPen(QPen(QColor(255, 255, 255))); //�ױ�
		painter->setBrush(QBrush(Qt::red, Qt::SolidPattern)); //���
	}

	//����4�ֲ�ָͬ��ļ�ͷ
	static const QPointF *arrow_toLeft = arrowShapeArray;
	
	static const QPointF arrow_toRight[7] = {
		QPointF(-arrowShapeArray[0].x(), arrowShapeArray[0].y()),
		QPointF(-arrowShapeArray[1].x(), arrowShapeArray[1].y()),
		QPointF(-arrowShapeArray[2].x(), arrowShapeArray[2].y()),
		QPointF(-arrowShapeArray[3].x(), arrowShapeArray[3].y()),
		QPointF(-arrowShapeArray[4].x(), arrowShapeArray[4].y()),
		QPointF(-arrowShapeArray[5].x(), arrowShapeArray[5].y()),
		QPointF(-arrowShapeArray[6].x(), arrowShapeArray[6].y())
	};

	static const QPointF arrow_toUp[7] = {
		QPointF(-arrowShapeArray[0].y(), arrowShapeArray[0].x()),
		QPointF(-arrowShapeArray[1].y(), arrowShapeArray[1].x()),
		QPointF(-arrowShapeArray[2].y(), arrowShapeArray[2].x()),
		QPointF(-arrowShapeArray[3].y(), arrowShapeArray[3].x()),
		QPointF(-arrowShapeArray[4].y(), arrowShapeArray[4].x()),
		QPointF(-arrowShapeArray[5].y(), arrowShapeArray[5].x()),
		QPointF(-arrowShapeArray[6].y(), arrowShapeArray[6].x())
	};

	static const QPointF arrow_toDown[7] = {
		QPointF(arrowShapeArray[0].y(), -arrowShapeArray[0].x()),
		QPointF(arrowShapeArray[1].y(), -arrowShapeArray[1].x()),
		QPointF(arrowShapeArray[2].y(), -arrowShapeArray[2].x()),
		QPointF(arrowShapeArray[3].y(), -arrowShapeArray[3].x()),
		QPointF(arrowShapeArray[4].y(), -arrowShapeArray[4].x()),
		QPointF(arrowShapeArray[5].y(), -arrowShapeArray[5].x()),
		QPointF(arrowShapeArray[6].y(), -arrowShapeArray[6].x())
	};

	//���Ƽ�ͷ
	int marginWidth = sizeFactor * basicArrowLength * 1.3; //��Ե���
	if (pos().y() < marginWidth) {
		painter->drawConvexPolygon(arrow_toUp, 7);
	}
	else if (pos().y() > fullImageSize->height() - marginWidth) {
		painter->drawConvexPolygon(arrow_toDown, 7);
	} 
	else if(pos().x() > fullImageSize->width() - marginWidth) {
		painter->drawConvexPolygon(arrow_toRight, 7);
	}
	else {
		painter->drawConvexPolygon(arrow_toLeft, 7);
	}
	Flash = !Flash;
}


/**************** ��ʱ���� *****************/

//����ʱ����ʱ����
void FlickeringArrow::on_timeOut_timer()
{
	//֪ͨ������ˢ�¼�ͷ
	emit refreshArrow_arrow();
}

//��ʼ��˸
void FlickeringArrow::startFlickering(int msec)
{
	timer->start(msec);
}

//ֹͣ��˸
void FlickeringArrow::stopFlickering()
{
	timer->stop();
}