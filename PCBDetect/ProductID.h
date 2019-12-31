#pragma once

#include <QObject>
#include <QDate>
#include <QDebug>


//产品ID
class ProductID : public QObject
{
	Q_OBJECT

public:
	QString modelType; //型号
	QDate date; //日期
	long long serialNum; //序号
	long long damp; //时间戳

	enum ErrorCode {
		Default
	};
	
	ProductID(QObject *parent = Q_NULLPTR);
	~ProductID();

	void init();
	void reset();
	void setModelType(QString num);
	void resetModelType();
	void clearSerialNum();
	void update();
	void copyTo(ProductID *dst);

	bool isValid();
	bool isModelTypeValid();
	
	QString getDateString();
	QString getDirHierarchy();
	//ErrorCode getErrorCode();
};
