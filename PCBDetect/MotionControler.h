#pragma once
#include <QObject>
#include "Configurator.h"
#include "AMC98C.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//�˶�������
class MotionControler : public QObject
{
	Q_OBJECT

private:
	Ui::DetectConfig *config;

public:
	MotionControler(QObject *parent);
	~MotionControler();

	inline void setDetectConfig(Ui::DetectConfig *ptr = Q_NULLPTR) { config = ptr; }

	void initControler(); //��ʼ��
	void moveForward(); //ǰ��
	void returnToZero(); //����
	void resetControler(); //��λ

private:
	void on_initControler_finished();
	void on_moveForward_finished();
	void on_returnToZero_finished();
	void on_resetControler_finished();

Q_SIGNALS:
	void signal_moveForward_finished();
};

