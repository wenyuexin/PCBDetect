#pragma once

#include <QObject>
#include <QDate>
#include <QDebug>


//��ƷID
class ProductID : public QObject
{
	Q_OBJECT

public:
	QString modelType; //�ͺ�
	QDate date; //����
	long long serialNum; //���
	long long damp; //ʱ���

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
