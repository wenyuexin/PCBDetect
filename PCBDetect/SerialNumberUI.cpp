#include "SerialNumberUI.h"

SerialNumberUI::SerialNumberUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setMouseTracking(true);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//获取绘图控件QGraphicsView的位置
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//设置检测界面的聚焦策略
	//this->setFocusPolicy(Qt::ClickFocus);

	//鼠标跟踪
	this->setMouseTracking(true);

	//禁用滚动条
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

SerialNumberUI::~SerialNumberUI()
{
}


/******************* 按键响应 *******************/

//获取包含产品序号的ROI区域
void SerialNumberUI::on_pushButton_getROI_clicked()
{

}

//切换分图
void SerialNumberUI::on_pushButton_switchImage_clicked()
{
	emit switchImage_serialNumUI();
}

//识别产品序号
void SerialNumberUI::on_pushButton_recognize_clicked()
{
	//此处调用OCR模块
}

void SerialNumberUI::on_pushButton_return_clicked()
{
	//emit recognizeFinished_serialNumberUI();
}


/******************* 显示分图与鼠标框选 *******************/

//在graphicView中显示分图
void SerialNumberUI::showSampleImage(int gridRowIdx, int gridColIdx)
{
	QSize graphicsViewSize = graphicsViewRect.size();
	QPixmap scaledImg = ((*qpixmapSamples)[gridRowIdx][gridColIdx])->scaled(graphicsViewSize, Qt::KeepAspectRatio);
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //定义图元
	//item->setPos(itemGrid[currentRow_show][iCamera]); //图元的显示位置
	graphicsScene.addItem(item);
	
	ui.graphicsView->show();
}

//鼠标按下事件
void SerialNumberUI::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && captureStatus == InitCapture) {
		captureStatus = BeginCapture;
		mousePressPos = event->pos();

		qDebug() << mousePressPos.x() << mousePressPos.y();
	} 
	else if (event->button() == Qt::LeftButton && graphicsViewRect.contains(event->pos())) {
		captureStatus = BeginMove;
		setCursor(Qt::SizeAllCursor);
		mouseReleasePos = event->pos();
	}
}

//鼠标移动事件
void SerialNumberUI::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton && mousePress) {
		if (!this->mouseRelease) {
			this->mouseReleasePos = event->pos();//更新鼠标释放的位置
			//paint();

			qDebug() << mouseReleasePos.x() << mouseReleasePos.y();
		}
	}
}

//鼠标释放事件
void SerialNumberUI::mouseReleaseEvent(QMouseEvent *event)
{
	Qt::MouseButtons b = event->buttons();
	if (event->buttons() == Qt::LeftButton) {
		if (this->mouseRelease) {
			this->mouseReleasePos = event->pos();
			this->mousePress = false;
			this->mouseRelease = true;

			qDebug() << mouseReleasePos.x() << mouseReleasePos.y();
			//paint();
		}
	}
}

bool SerialNumberUI::isPressPosInGraphicViewRect(QPoint mousePressPos)
{
	QRect selectRect = getRect(mousePressPos, mouseReleasePos);
	return selectRect.contains(mousePressPos);
}

// 根据beginPoint , endPoint 获取当前选中的矩形
QRect SerialNumberUI::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
	int x, y, width, height;
	width = qAbs(beginPoint.x() - endPoint.x());
	height = qAbs(beginPoint.y() - endPoint.y());
	x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
	y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

	// 避免宽或高为零时拷贝截图有误;
	// 可以看QQ截图，当选取截图宽或高为零时默认为2;
	QRect selectedRect = QRect(x, y, width, height);
	if (selectedRect.width() == 0) selectedRect.setWidth(1);
	if (selectedRect.height() == 0) selectedRect.setHeight(1);

	return selectedRect;
}
