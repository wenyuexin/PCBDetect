#include "FileSyncThread.h"

FileSyncThread::FileSyncThread(QObject *parent)
	: QThread(parent)
{
}

FileSyncThread::~FileSyncThread()
{
	qDebug() << "~FileSyncThread";
}


void FileSyncThread::run()
{
	QString folder = runtimeParams->currentOutputDir; //需要同步的文件夹
	std::vector<std::string> fileList; //存有文件路径的字符串列表
	pcb::getFilePathList(folder, fileList); //遍历文件夹及其子文件夹中的文件
	if (fileList.size() == 0) return;

	QString ip = userConfig->inetAddressOfRecheckPC; //复查设备的IP地址
	FileSender FileSender(ip.toStdString()); //文件发送类
	FileSender.sendFiles(fileList); //将列表中的文件发送到复查设备中
}