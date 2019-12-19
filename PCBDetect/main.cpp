
#ifndef WIN32
#define WIN32
#endif
#include "PCBDetect.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.processEvents();
	PCBDetect w;
	return a.exec();
}
