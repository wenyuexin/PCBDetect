#include "PCBDetect.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QString path = "D:\\Workspace\\Qt\\PCBData\\output\\99\\1\\1";
	std::vector<std::string> list;
	pcb::getFilePathList(path, list);

	//QApplication a(argc, argv);
	//a.processEvents();
	//PCBDetect w;
	//return a.exec();

	return 0;
}
