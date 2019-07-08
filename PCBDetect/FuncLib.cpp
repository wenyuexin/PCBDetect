#include "FuncLib.h"

using std::vector;


//非阻塞延迟
void pcb::delay(unsigned long msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//交互式文件夹路径选择
QString pcb::selectDirPath(QWidget *parent, QString windowTitle)
{
	if (windowTitle == "") windowTitle = chinese("请选择路径");

	QFileDialog *fileDialog = new QFileDialog(parent);
	fileDialog->setWindowTitle(windowTitle); //设置文件保存对话框的标题
	fileDialog->setFileMode(QFileDialog::Directory); //设置文件对话框弹出的时候显示文件夹
	fileDialog->setViewMode(QFileDialog::Detail); //文件以详细的形式显示，显示文件名，大小，创建日期等信息

	QString path = "";
	if (fileDialog->exec() == QDialog::DialogCode::Accepted) //选择路径
		path = fileDialog->selectedFiles()[0];
	delete fileDialog;
	return path;
}

//删除字符串首尾的非数字字符
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
 * clearFiles():仅清空文件夹内的文件(不包括子文件夹内的文件)
 * folderFullPath:文件夹全路径
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
 * clearFolder() - 删除非空文件夹
 * folderFullPath - 文件夹全路径
 * include - 是否要删除输入路径对应的文件夹
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

	/* 首先获取目标文件夹内所有文件及文件夹信息 */
	fileList = dir.entryInfoList(QDir::Dirs | QDir::Files
		| QDir::Readable | QDir::Writable
		| QDir::Hidden | QDir::NoDotAndDotDot
		, QDir::Name);

	while (fileList.size() > 0) {
		infoNum = fileList.size();
		for (i = infoNum - 1; i >= 0; i--) {
			curFile = fileList[i];
			if (curFile.isFile()) { /* 如果是文件，删除文件 */
				QFile fileTemp(curFile.filePath());
				fileTemp.remove();
				fileList.removeAt(i);
			}

			if (curFile.isDir()) { /* 如果是文件夹 */
				QDir dirTemp(curFile.filePath());
				fileListTemp = dirTemp.entryInfoList(QDir::Dirs | QDir::Files
					| QDir::Readable | QDir::Writable
					| QDir::Hidden | QDir::NoDotAndDotDot
					, QDir::Name);

				if (fileListTemp.size() == 0) { /* 下层没有文件或文件夹 则直接删除*/
					dirTemp.rmdir(".");
					fileList.removeAt(i);
				}
				else { /* 下层有文件夹或文件 则将信息添加到列表*/
					for (j = 0; j < fileListTemp.size(); j++) {
						if (!(fileList.contains(fileListTemp[j])))
							fileList.append(fileListTemp[j]);
					}
				}
			}
		}
	}

	//删除目标文件夹
	//如果只是清空文件夹folderFullPath的内容而不删除folderFullPath本身,则删掉本行即可
	//dir.rmdir(".");
}


//将bool向量转换为数字字符串
QString pcb::boolVectorToString(const vector<bool> &vec)
{
	QString str = "";
	for (int i = 0; i < vec.size(); i++) {
		str += (vec[i]) ? '1' : '0';
	}
	return str;
}

//将数字字符串转换为bool向量
//如果str的大小不等于n，则返回空向量
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


//基于正则表达式判断IP地址的合法性
bool pcb::isInetAddress(QString ip)
{
	QRegExp rx("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])(\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])){2}\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
	return  rx.exactMatch(ip.trimmed()); //去除空字符后进行匹配
}