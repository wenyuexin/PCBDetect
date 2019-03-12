#include "PCBDetect.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.processEvents();
	PCBDetect w;
	//w.show();
	return a.exec();
}
