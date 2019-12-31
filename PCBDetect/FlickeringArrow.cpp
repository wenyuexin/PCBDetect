#include "FlickeringArrow.h"


FlickeringArrow::FlickeringArrow()
{
	brushColor = Qt::red;
	Flash = true;

	//定义箭头的大小与形状 ←
	sizeFactor = 1.7; //箭头尺寸因子，控制箭头大小
	basicArrowLength = 20; //箭头的基础长度
	basicArrowWidth = 12; //箭头的基础宽度
	arrowShapeArray =  new QPointF[7] {
		sizeFactor * QPointF(1.0, 0.0),
		sizeFactor * QPointF(15.0, basicArrowWidth / 2),
		sizeFactor * QPointF(13.0, 2.0),
		sizeFactor * QPointF(basicArrowLength, 2.0), //对称
		sizeFactor * QPointF(basicArrowLength, -2.0),
		sizeFactor * QPointF(13.0, -2.0),
		sizeFactor * QPointF(15.0, -basicArrowWidth / 2)
	};

	timer = new QTimer; //用于刷新闪烁的箭头
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


/**************** 绘制箭头 *******************/

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
		painter->setPen(QPen(QColor(0, 0, 0))); //黑边
		painter->setBrush(QBrush(Qt::white, Qt::SolidPattern)); //白底
	}
	else {
		painter->setPen(QPen(QColor(255, 255, 255))); //白边
		painter->setBrush(QBrush(Qt::red, Qt::SolidPattern)); //红底
	}

	//定义4种不同指向的箭头
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

	//绘制箭头
	int marginWidth = sizeFactor * basicArrowLength * 1.3; //边缘宽度
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


/**************** 定时更新 *****************/

//当计时器计时结束
void FlickeringArrow::on_timeOut_timer()
{
	//通知主界面刷新箭头
	emit refreshArrow_arrow();
}

//开始闪烁
void FlickeringArrow::startFlickering(int msec)
{
	timer->start(msec);
}

//停止闪烁
void FlickeringArrow::stopFlickering()
{
	timer->stop();
}