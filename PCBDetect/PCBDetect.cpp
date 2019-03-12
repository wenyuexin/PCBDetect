#include "PCBDetect.h"

using Ui::DetectConfig;


PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//����״̬��ѡ���ڸ���ȫ����ʾ
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);
	this->showFullScreen(); //ȫ��

	//ͼ���ļ��е�·��
	IconFolder = QDir::currentPath() + "/Icons";

	//��������
	launcher = new LaunchUI(Q_NULLPTR, &(QPixmap(IconFolder + "/screen.png")));
	launcher->showFullScreen(); //��ʾlauncher

	launcher->setDetectConfig(&config); //����launcher
	launcher->runInitThread(); //���г�ʼ���߳�
	connect(launcher, SIGNAL(launchFinished_launchUI()), this, SLOT(on_launchFinished_launchUI()));

	//�������ý���
	settingUI = new SettingUI;
	settingUI->setDetectConfig(&config);
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));

	//ģ����ȡ����
	templateUI = new TemplateUI;
	templateUI->setDetectConfig(&config);
	templateUI->setDetectParams(&params);
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//������
	detectUI = new DetectUI;
	detectUI->setDetectConfig(&config);
	detectUI->setDetectParams(&params);
	connect(detectUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_detectUI()));

}

PCBDetect::~PCBDetect()
{
	delete launcher;
	delete settingUI;
	delete templateUI;
	delete detectUI;
}


/**************** �������� *****************/

void PCBDetect::on_launchFinished_launchUI()
{
	//��������ͼ����ʾ�ĳ�ʼ��
	detectUI->initGraphicsView();

	//ģ�������ͼ����ʾ�ĳ�ʼ��
	templateUI->initGraphicsView();

	//���²������ý������Ϣ
	settingUI->refreshSettingUI();

	//�رճ�ʼ�����棬��ʾ������
	this->showFullScreen();
	Ui::delay(10); //��ʱ
	launcher->close();
}


/*************** �����Ŀ�������Ӧ *****************/

//����
void PCBDetect::on_pushButton_set_clicked()
{
	showSettingUI();
}

void PCBDetect::on_pushButton_set2_clicked()
{
	showSettingUI();
}

//ģ����ȡ
void PCBDetect::on_pushButton_getTempl_clicked()
{
	showTemplateUI();
}

void PCBDetect::on_pushButton_getTempl2_clicked()
{
	showTemplateUI();
}

//���
void PCBDetect::on_pushButton_detect_clicked()
{
	showDetectUI();
}

void PCBDetect::on_pushButton_detect2_clicked()
{
	showDetectUI();
}

//�˳�
void PCBDetect::on_pushButton_exit_clicked()
{
	eixtDetectSystem();
}

void PCBDetect::on_pushButton_exit2_clicked()
{
	eixtDetectSystem();
}


/****************** ���ý��� ******************/

void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //��ʾ������
	Ui::delay(10); //��ʱ
	settingUI->hide(); //�������ý���
}

void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //��ʾ���ý���
	Ui::delay(10); //��ʱ
	this->hide(); //����������
}


/***************** ģ����ȡ���� *****************/

void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->showFullScreen(); //��ʾ������
	Ui::delay(10); //��ʱ
	templateUI->hide(); //����ģ����ȡ����
}

void PCBDetect::showTemplateUI()
{
	templateUI->showFullScreen(); //��ʾģ����ȡ����
	Ui::delay(10); //��ʱ
	this->hide(); //����������
}


/******************* ������ ******************/

void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->showFullScreen(); //��ʾ������
	Ui::delay(10); //��ʱ
	detectUI->hide(); //���ؼ�����
}

void PCBDetect::showDetectUI()
{
	detectUI->showFullScreen(); //��ʾ������
	Ui::delay(10); //��ʱ
	this->hide(); //����������
}


/******************* �������� ******************/

//�˳����ϵͳ
void PCBDetect::eixtDetectSystem()
{
	this->close();
	qApp->exit(0);
}
