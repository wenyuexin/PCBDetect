#include "ProductID.h"

ProductID::ProductID(QObject *parent)
	: QObject(parent)
{
}

ProductID::~ProductID()
{
	qDebug() << "~ProductID";
}


void ProductID::init()
{
	reset();
}

void ProductID::reset()
{
	modelType = ""; //�ͺ�
	date = QDate::currentDate(); //����
	serialNum = 0; //���
}

//�ͺ�
void ProductID::setModelType(QString str)
{
	modelType = str.remove(QRegExp("\\s")); //ɾ���հ��ַ�
	date = QDate::currentDate(); //����
}

void ProductID::resetModelType()
{
	setModelType("");
}

//���
void ProductID::update()
{
	date = QDate::currentDate(); //����
	//serialNum += 1; //��ŵ���
	serialNum = QDateTime::currentMSecsSinceEpoch();
}

void ProductID::clearSerialNum()
{
	date = QDate::currentDate(); //����
	serialNum = 0; //���
}


void ProductID::copyTo(ProductID *dst)
{
	dst->modelType = this->modelType;
	dst->date = QDate(this->date);
	dst->serialNum = this->serialNum;
}


bool ProductID::isValid()
{
	return isModelTypeValid() && serialNum > 0;
}

bool ProductID::isModelTypeValid()
{
	return modelType != "";
}


QString ProductID::getDateString()
{
	return date.toString("MM.dd");
}

//����ƷIDΪ��ʾĿ¼��ε��ַ���
QString ProductID::getDirHierarchy()
{
	return modelType + "/" + date.toString("MM.dd") + "/" + QString::number(serialNum);
}