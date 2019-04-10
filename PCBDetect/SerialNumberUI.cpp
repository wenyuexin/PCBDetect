#include "SerialNumberUI.h"

SerialNumberUI::SerialNumberUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setMouseTracking(true);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);

	//��ȡ��ͼ�ؼ�QGraphicsView��λ��
	QPoint graphicsViewPos = ui.graphicsView->pos();
	QSize graphicsViewSize = ui.graphicsView->size();
	graphicsViewRect = QRect(graphicsViewPos, graphicsViewSize);
	ui.graphicsView->setScene(&graphicsScene);

	//���ü�����ľ۽�����
	//this->setFocusPolicy(Qt::ClickFocus);

	//������
	this->setMouseTracking(true);

	//���ù�����
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

SerialNumberUI::~SerialNumberUI()
{
}


/******************* ������Ӧ *******************/

//��ȡ������Ʒ��ŵ�ROI����
void SerialNumberUI::on_pushButton_getROI_clicked()
{

}

//�л���ͼ
void SerialNumberUI::on_pushButton_switchImage_clicked()
{
	emit switchImage_serialNumUI();
}

//ʶ���Ʒ���
void SerialNumberUI::on_pushButton_recognize_clicked()
{
	//�˴�����OCRģ��
}

void SerialNumberUI::on_pushButton_return_clicked()
{
	//emit recognizeFinished_serialNumberUI();
}


/******************* ��ʾ��ͼ������ѡ *******************/

//��graphicView����ʾ��ͼ
void SerialNumberUI::showSampleImage(int gridRowIdx, int gridColIdx)
{
	QSize graphicsViewSize = graphicsViewRect.size();
	QPixmap scaledImg = ((*qpixmapSamples)[gridRowIdx][gridColIdx])->scaled(graphicsViewSize, Qt::KeepAspectRatio);
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledImg); //����ͼԪ
	//item->setPos(itemGrid[currentRow_show][iCamera]); //ͼԪ����ʾλ��
	graphicsScene.addItem(item);
	
	ui.graphicsView->show();
}

//��갴���¼�
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

//����ƶ��¼�
void SerialNumberUI::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton && mousePress) {
		if (!this->mouseRelease) {
			this->mouseReleasePos = event->pos();//��������ͷŵ�λ��
			//paint();

			qDebug() << mouseReleasePos.x() << mouseReleasePos.y();
		}
	}
}

//����ͷ��¼�
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

// ����beginPoint , endPoint ��ȡ��ǰѡ�еľ���
QRect SerialNumberUI::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
	int x, y, width, height;
	width = qAbs(beginPoint.x() - endPoint.x());
	height = qAbs(beginPoint.y() - endPoint.y());
	x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
	y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

	// �������Ϊ��ʱ������ͼ����;
	// ���Կ�QQ��ͼ����ѡȡ��ͼ����Ϊ��ʱĬ��Ϊ2;
	QRect selectedRect = QRect(x, y, width, height);
	if (selectedRect.width() == 0) selectedRect.setWidth(1);
	if (selectedRect.height() == 0) selectedRect.setHeight(1);

	return selectedRect;
}
