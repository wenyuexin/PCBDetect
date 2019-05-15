#include "PCBDetect.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QString out_path = QString("123") + "/";
	out_path += QString("%1_%2_%3_%4").arg(50, 4, 10, QChar('0')).arg(3, 5).arg(101, 5).arg(1);



	QApplication a(argc, argv);
	a.processEvents();
	PCBDetect w;
	return a.exec();
}
