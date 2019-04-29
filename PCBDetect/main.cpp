#include "PCBDetect.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	_CrtSetBreakAlloc(161);
	_CrtSetBreakAlloc(174);

	QApplication a(argc, argv);
	a.processEvents();
	PCBDetect w;
	//w.show();
	return a.exec();
}
