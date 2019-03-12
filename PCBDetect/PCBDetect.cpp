#include "PCBDetect.h"

using Ui::DetectConfig;


PCBDetect::PCBDetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//多屏状态下选择在副屏全屏显示
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry(1);
	this->setGeometry(screenRect);
	this->showFullScreen(); //全屏

	//图标文件夹的路径
	IconFolder = QDir::currentPath() + "/Icons";

	//启动界面
	launcher = new LaunchUI(Q_NULLPTR, &(QPixmap(IconFolder + "/screen.png")));
	launcher->showFullScreen(); //显示launcher

	launcher->setDetectConfig(&config); //配置launcher
	launcher->runInitThread(); //运行初始化线程
	connect(launcher, SIGNAL(launchFinished_launchUI()), this, SLOT(on_launchFinished_launchUI()));

	//参数设置界面
	settingUI = new SettingUI;
	settingUI->setDetectConfig(&config);
	connect(settingUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_settingUI()));

	//模板提取界面
	templateUI = new TemplateUI;
	templateUI->setDetectConfig(&config);
	templateUI->setDetectParams(&params);
	connect(templateUI, SIGNAL(showDetectMainUI()), this, SLOT(do_showDetectMainUI_templateUI()));

	//检测界面
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


/**************** 启动界面 *****************/

void PCBDetect::on_launchFinished_launchUI()
{
	//检测界面中图像显示的初始化
	detectUI->initGraphicsView();

	//模板界面中图像显示的初始化
	templateUI->initGraphicsView();

	//更新参数设置界面的信息
	settingUI->refreshSettingUI();

	//关闭初始化界面，显示主界面
	this->showFullScreen();
	Ui::delay(10); //延时
	launcher->close();
}


/*************** 按键的控制与响应 *****************/

//设置
void PCBDetect::on_pushButton_set_clicked()
{
	showSettingUI();
}

void PCBDetect::on_pushButton_set2_clicked()
{
	showSettingUI();
}

//模板提取
void PCBDetect::on_pushButton_getTempl_clicked()
{
	showTemplateUI();
}

void PCBDetect::on_pushButton_getTempl2_clicked()
{
	showTemplateUI();
}

//检测
void PCBDetect::on_pushButton_detect_clicked()
{
	showDetectUI();
}

void PCBDetect::on_pushButton_detect2_clicked()
{
	showDetectUI();
}

//退出
void PCBDetect::on_pushButton_exit_clicked()
{
	eixtDetectSystem();
}

void PCBDetect::on_pushButton_exit2_clicked()
{
	eixtDetectSystem();
}


/****************** 设置界面 ******************/

void PCBDetect::do_showDetectMainUI_settingUI()
{
	this->showFullScreen(); //显示主界面
	Ui::delay(10); //延时
	settingUI->hide(); //隐藏设置界面
}

void PCBDetect::showSettingUI()
{
	settingUI->showFullScreen(); //显示设置界面
	Ui::delay(10); //延时
	this->hide(); //隐藏主界面
}


/***************** 模板提取界面 *****************/

void PCBDetect::do_showDetectMainUI_templateUI()
{
	this->showFullScreen(); //显示主界面
	Ui::delay(10); //延时
	templateUI->hide(); //隐藏模板提取界面
}

void PCBDetect::showTemplateUI()
{
	templateUI->showFullScreen(); //显示模板提取界面
	Ui::delay(10); //延时
	this->hide(); //隐藏主界面
}


/******************* 检测界面 ******************/

void PCBDetect::do_showDetectMainUI_detectUI()
{
	this->showFullScreen(); //显示主界面
	Ui::delay(10); //延时
	detectUI->hide(); //隐藏检测界面
}

void PCBDetect::showDetectUI()
{
	detectUI->showFullScreen(); //显示检测界面
	Ui::delay(10); //延时
	this->hide(); //隐藏主界面
}


/******************* 其他函数 ******************/

//退出检测系统
void PCBDetect::eixtDetectSystem()
{
	this->close();
	qApp->exit(0);
}
