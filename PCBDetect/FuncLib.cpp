#include "FuncLib.h"

using std::vector;


//�������ӳ�
void pcb::delay(unsigned long msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//����ʽ�ļ���·��ѡ��
QString pcb::selectDirPath(QWidget *parent, QString windowTitle)
{
	if (windowTitle == "") windowTitle = chinese("��ѡ��·��");

	QFileDialog *fileDialog = new QFileDialog(parent);
	fileDialog->setWindowTitle(windowTitle); //�����ļ�����Ի���ı���
	fileDialog->setFileMode(QFileDialog::Directory); //�����ļ��Ի��򵯳���ʱ����ʾ�ļ���
	fileDialog->setViewMode(QFileDialog::Detail); //�ļ�����ϸ����ʽ��ʾ����ʾ�ļ�������С���������ڵ���Ϣ

	QString path = "";
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) //ѡ��·��
		path = fileDialog->selectedFiles()[0];
	delete fileDialog;
	return path;
}

//ɾ���ַ�����β�ķ������ַ�
QString pcb::eraseNonDigitalCharInHeadAndTail(QString s)
{
	if (s == "") return "";
	int begin = 0;
	for (; begin < s.size() && !s.at(begin).isDigit(); begin++) {}
	int end = s.size() - 1;
	for (; end > begin && !s.at(end).isDigit(); end--) {}
	return s.mid(begin, end - begin + 1);
}


/*
 * clearFiles():������ļ����ڵ��ļ�(���������ļ����ڵ��ļ�)
 * folderFullPath:�ļ���ȫ·��
 */
void pcb::clearFiles(const QString &folderFullPath)
{
	QDir dir(folderFullPath);
	dir.setFilter(QDir::Files);
	int fileCount = dir.count();
	for (int i = 0; i < fileCount; i++)
		dir.remove(dir[i]);
}

/*
 * clearFolder() - ɾ���ǿ��ļ���
 * folderFullPath - �ļ���ȫ·��
 * include - �Ƿ�Ҫɾ������·����Ӧ���ļ���
 */
void pcb::clearFolder(const QString &folderFullPath, bool included)
{
	QDir             dir(folderFullPath);
	QFileInfoList    fileList;
	QFileInfo        curFile;
	QFileInfoList    fileListTemp;
	int32_t          infoNum;
	int32_t          i;
	int32_t          j;

	/* ���Ȼ�ȡĿ���ļ����������ļ����ļ�����Ϣ */
	fileList = dir.entryInfoList(QDir::Dirs | QDir::Files
		| QDir::Readable | QDir::Writable
		| QDir::Hidden | QDir::NoDotAndDotDot
		, QDir::Name);

	while (fileList.size() > 0) {
		infoNum = fileList.size();
		for (i = infoNum - 1; i >= 0; i--) {
			curFile = fileList[i];
			if (curFile.isFile()) { /* ������ļ���ɾ���ļ� */
				QFile fileTemp(curFile.filePath());
				fileTemp.remove();
				fileList.removeAt(i);
			}

			if (curFile.isDir()) { /* ������ļ��� */
				QDir dirTemp(curFile.filePath());
				fileListTemp = dirTemp.entryInfoList(QDir::Dirs | QDir::Files
					| QDir::Readable | QDir::Writable
					| QDir::Hidden | QDir::NoDotAndDotDot
					, QDir::Name);

				if (fileListTemp.size() == 0) { /* �²�û���ļ����ļ��� ��ֱ��ɾ��*/
					dirTemp.rmdir(".");
					fileList.removeAt(i);
				}
				else { /* �²����ļ��л��ļ� ����Ϣ��ӵ��б�*/
					for (j = 0; j < fileListTemp.size(); j++) {
						if (!(fileList.contains(fileListTemp[j])))
							fileList.append(fileListTemp[j]);
					}
				}
			}
		}
	}

	//ɾ��Ŀ���ļ���
	//���ֻ������ļ���folderFullPath�����ݶ���ɾ��folderFullPath����,��ɾ�����м���
	//dir.rmdir(".");
}


//��bool����ת��Ϊ�����ַ���
QString pcb::boolVectorToString(const vector<bool> &vec)
{
	QString str = "";
	for (int i = 0; i < vec.size(); i++) {
		str += (vec[i]) ? '1' : '0';
	}
	return str;
}

//�������ַ���ת��Ϊbool����
//���str�Ĵ�С������n���򷵻ؿ�����
vector<bool> pcb::stringToBoolVector(const QString &str, int n)
{
	vector<bool> vec;
	if (n > 0 && str.size() != n) return vec;

	vec.resize(str.size());
	for (int i = 0; i < str.size(); i++) {
		vec[i] = bool(str.mid(i, 1).toInt());
	}
	return vec;
}


//����������ʽ�ж�IP��ַ�ĺϷ���
bool pcb::isInetAddress(QString ip)
{
	QRegExp rx("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])(\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])){2}\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
	return  rx.exactMatch(ip.trimmed()); //ȥ�����ַ������ƥ��
}