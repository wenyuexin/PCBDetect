#pragma once
#include <QObject>
#include "Configurator.h"
//#include "AMC98C.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


namespace pcb {
	class ImageConverter;
}

//�˶�������
class MotionControler : public QObject
{
	Q_OBJECT

private:
	pcb::DetectConfig *config;
	int callerOfResetControler; //��λ�ĵ��ú����ı�ʶ

public:
	MotionControler(QObject *parent = Q_NULLPTR);
	~MotionControler();

	//inline void setDetectConfig(pcb::DetectConfig *ptr = Q_NULLPTR) { config = ptr; } 

	void initControler(); //��ʼ��
	void moveForward(); //ǰ��
	void returnToZero(); //����
	void resetControler(int caller); //��λ

private:
	void on_initControler_finished();
	void on_moveForward_finished();
	void on_returnToZero_finished();
	void on_resetControler_finished();

Q_SIGNALS:
	void initControlerFinished_motion();
	void moveForwardFinished_motion();
	void returnToZeroFinished_motion();
	void resetControlerFinished_motion(int);
};

