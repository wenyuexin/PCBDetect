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
	QString folder = runtimeParams->currentOutputDir; //��Ҫͬ�����ļ���
	std::vector<std::string> fileList; //�����ļ�·�����ַ����б�
	pcb::getFilePathList(folder, fileList); //�����ļ��м������ļ����е��ļ�
	if (fileList.size() == 0) return;

	QString ip = userConfig->inetAddressOfRecheckPC; //�����豸��IP��ַ
	FileSender FileSender(ip.toStdString()); //�ļ�������
	FileSender.sendFiles(fileList); //���б��е��ļ����͵������豸��
}