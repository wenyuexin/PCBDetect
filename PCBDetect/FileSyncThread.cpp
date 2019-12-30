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
	QString hierarchy = runtimeParams->productID.getDirHierarchy(); //��ȡĿ¼����ַ���
	FileSender fileSender(ip.toStdString()); //�ļ�������
	fileSender.setHierarchy(hierarchy.toStdString()); //����Ŀ¼���
	fileSender.sendFiles(fileList); //���б��е��ļ����͵������豸��
}