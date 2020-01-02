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
	modelType = ""; //型号
	date = QDate::currentDate(); //日期
	serialNum = 0; //序号
}

//型号
void ProductID::setModelType(QString str)
{
	modelType = str.remove(QRegExp("\\s")); //删除空白字符
	date = QDate::currentDate(); //日期
}

void ProductID::resetModelType()
{
	setModelType("");
}

//序号
void ProductID::update()
{
	date = QDate::currentDate(); //日期
	//serialNum += 1; //序号递增
	serialNum = QDateTime::currentMSecsSinceEpoch();
}

void ProductID::clearSerialNum()
{
	date = QDate::currentDate(); //日期
	serialNum = 0; //序号
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

//将产品ID为表示目录层次的字符串
QString ProductID::getDirHierarchy()
{
	return modelType + "/" + date.toString("MM.dd") + "/" + QString::number(serialNum);
}