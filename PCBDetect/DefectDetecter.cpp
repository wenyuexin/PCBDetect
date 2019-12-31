#include "DefectDetecter.h"
#include <exception>
#include <string>
#include <map>

using pcb::CvMatVector;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::DetectResult;
using pcb::QImageVector;
using pcb::CvMatVector;
using cv::Mat;
using cv::Point;
using cv::Size;
using cv::Rect;
using std::string;
using std::vector;
using std::to_string;


DefectDetecter::DefectDetecter() 
{
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	detectResult = Q_NULLPTR; //检测结果
	cvmatSamples = Q_NULLPTR; //正在检测的一行样本
	detectFunc = Q_NULLPTR; //检测辅助类
	detectState = Default; //检测状态（用于界面显示和程序调试）
	totalDefectNum = INT_MIN; //缺陷总数
}

DefectDetecter::~DefectDetecter() 
{
	qDebug() << "~DefectDetecter";
	deleteDetectUnits();
	delete detectFunc;
	detectFunc = Q_NULLPTR;
}


void DefectDetecter::init()
{
	generateBigTempl(); //生成大模板
	initDetectFunc(); //初始化检测函数类
	initDetectUnits(); //对若干检测单元进行初始化
}

//初始化templFunc
void DefectDetecter::initDetectFunc()
{
	delete detectFunc;
	detectFunc = new DetectFunc;
	detectFunc->setAdminConfig(adminConfig);
	detectFunc->setUserConfig(userConfig);
	detectFunc->setRuntimeParams(runtimeParams);
	
	//detectFunc->setDetectResult(detectResult);
}

//生成完整尺寸的缺陷检测图像
void DefectDetecter::generateBigTempl()
{
	Size originalfullImgSize = Size(adminConfig->ImageSize_W * runtimeParams->nCamera,
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); //整图的原始尺寸

	double factorW = 2.0 * runtimeParams->ScreenRect.width() / originalfullImgSize.width;
	double factorH = 2.0 * runtimeParams->ScreenRect.height() / originalfullImgSize.height;
	scalingFactor = qMin(factorW, factorH); //缩放因子

	//scalingFactor = 1; 1

	scaledSubImageSize = Size(scalingFactor * adminConfig->ImageSize_W,
		scalingFactor * adminConfig->ImageSize_H); //分图经过缩放后的尺寸

	scaledFullImageSize = Size(scaledSubImageSize.width * runtimeParams->nCamera,
		scaledSubImageSize.height * runtimeParams->nPhotographing); //整图经过缩放后的尺寸

	bigTempl = Mat(scaledFullImageSize, CV_8UC3); //生成用于记录缺陷的整图
}


/********* 检测单元的初始化和delete ***********/

void DefectDetecter::initDetectUnits()
{
	deleteDetectUnits(); //清除历史数据
	detectUnits.resize(MAX_DETECT_UNITS_NUM);
	for (int i = 0; i < MAX_DETECT_UNITS_NUM; i++) {
		detectUnits[i] = new DetectUnit();
		detectUnits[i]->setCurrentCol(i);//设置列号
		detectUnits[i]->setDetectFunc(detectFunc);//所有单元共用一个detectFunc
		detectUnits[i]->setAdminConfig(adminConfig); //设置系统参数
		detectUnits[i]->setUserConfig(userConfig); //设置用户参数
		detectUnits[i]->setRuntimeParams(runtimeParams); //设置运行参数
	}
}

void DefectDetecter::deleteDetectUnits()
{
	for (int i = 0; i < detectUnits.size(); i++) {
		delete detectUnits[i];
		detectUnits[i] = Q_NULLPTR;
	}
	detectUnits.clear();
}


/**************** 检测当前样本行 ****************/

void DefectDetecter::detect()
{
	detectState = DetectState::Start;
	emit updateDetectState_detecter(detectState);
	double t1 = clock();
	qDebug() << "====================" << pcb::chinese("开始检测") <<
		"( currentRow_detect =" << runtimeParams->currentRow_detect << ")" << endl;

	int currentRow_detect = runtimeParams->currentRow_detect;
	int nCamera = runtimeParams->nCamera;
	int nPhotographing = runtimeParams->nPhotographing;

	//检测对应的样本目录是否存在，不存在则创建
	if (currentRow_detect == 0) makeCurrentSampleDir();

	//检测对应的输出目录是否存在，不存在则创建
	vector<QString> subFolders { "fullImage" };
	if (currentRow_detect == 0) makeCurrentOutputDir(subFolders);

	//读取掩膜区域坐标值
	if (currentRow_detect == 0) {
		QString cornerPointsPath = userConfig->TemplDirPath + "/" + runtimeParams->productID.modelType + "/mask/cornerPoints.bin";
		cv::FileStorage store_new(cornerPointsPath.toStdString(), cv::FileStorage::READ);
		cv::FileNode node = store_new["cornerPoints"];
		vector<cv::Point2i> res;
		cv::read(node, res);
		if (res.size() == 0) {
			qDebug() << pcb::chinese("DefectDetecter: 加载模板的掩模区域坐标和阈值失败");
			errorCode = LoadTemplMaskRoiError; return;
		}
		maskRoi_bl = res[0];
		maskRoi_tr = res[1];
		segThresh = res[2].x;//全局阈值
		UsingDefaultSegThresh = res[2].y;//分割标志

		store_new.release();
	}

	static int templateFlag = -999;
	if (templateFlag != ((runtimeParams->productID.modelType).toInt()) || ((detectFunc->templateVec).empty() || (detectFunc->maskVec).empty())) {
		(detectFunc->templateVec).clear();
		(detectFunc->maskVec).clear();
		detectFunc->templateVec = std::vector<std::vector<cv::Mat>>(runtimeParams->nPhotographing, std::vector<cv::Mat>{});
		detectFunc->maskVec = std::vector<std::vector<cv::Mat>>(runtimeParams->nPhotographing, std::vector<cv::Mat>{});
		for (int i = 0; i < runtimeParams->nPhotographing; i++)
			for (int j = 0; j < runtimeParams->nCamera; j++)
			{
				QString templPath = userConfig->TemplDirPath + "/" + runtimeParams->productID.modelType + "/subtempl/"
					+ QString("%1_%2").arg(i + 1).arg(j + 1) + ".bmp";
				Mat templGray = cv::imread(templPath.toStdString(), 0);
				detectFunc->templateVec[i].push_back(templGray);

				QString mask_path = userConfig->TemplDirPath + "/" + runtimeParams->productID.modelType + "/mask/"
					+ QString("%1_%2_mask").arg(i + 1).arg(j + 1) + ".bmp";
				Mat mask_roi = cv::imread(mask_path.toStdString(), 0);
				detectFunc->maskVec[i].push_back(mask_roi);
			}
		templateFlag = (runtimeParams->productID.modelType).toInt();
	}

	//向检测单元传入适用于整个PCB板的参数
	if (currentRow_detect == 0) {
		totalDefectNum = 0; //缺陷总数
		for (int i = 0; i < nCamera; i++) {
			detectUnits[i]->setMaskRoi(&maskRoi_bl, &maskRoi_tr);//设置掩模区域坐标
			detectUnits[i]->setSegThresh(segThresh);//设置全局阈值
			detectUnits[i]->setThreshFlag(UsingDefaultSegThresh);//设置自动分割标志
			detectUnits[i]->setScalingFactor(scalingFactor); //设置缩放因子
			detectUnits[i]->setScaledFullImageSize(&scaledFullImageSize); //设置缩放后的整图图像尺寸
			detectUnits[i]->setScaledSubImageSize(&scaledSubImageSize); //设置缩放后的分图图像尺寸
		}
	}

	//nPhotographing
	//向检测单元传入适用于当前行的参数
	CvMatVector subImages = (*cvmatSamples)[currentRow_detect];
	for (int i = 0; i < nCamera; i++) {
		detectUnits[i]->setSubImage(*subImages[i]);//设置需要检测的分图
	}

	//开启若干检测线程，检测当前的一行分图
	for (int i = 0; i < nCamera; i++) {
		detectUnits[i]->start(); //开始转换
	}

	//等待所有检测单元运行结束
	for (int i = 0; i < nCamera; i++) {
		detectUnits[i]->wait();
	}

	//检测结束，整合当前行的检测结果
	
	for (int i = 0; i < nCamera; i++) {
		//统计缺陷总数
		int defectNum = detectUnits[i]->getDefectNum();
		totalDefectNum += defectNum;

		//将标记了缺陷的分图复制到大图
		Mat markedSubImage = detectUnits[i]->getMarkedSubImage();
		int curCol = detectUnits[i]->getcurCol();
		int curRow = detectUnits[i]->getcurRow();
		Rect rect(Point(curCol*markedSubImage.cols, curRow*markedSubImage.rows), markedSubImage.size());
		markedSubImage.copyTo(bigTempl(rect));

		//将缺陷细节图及其相关信息合并在一起
		std::map<cv::Point3i, cv::Mat, cmp_point3i> detailImage = detectUnits[i]->getDetailImage();
		allDetailImage.insert(detailImage.begin(), detailImage.end());
		detectUnits[i]->clearDetailImage();
	}

	//检测结束
	double t2 = clock();
	qDebug() << "====================" << pcb::chinese("当前行检测结束：") <<
		(t2 - t1) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;

	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);

	//如果当前检测的是最后一行图像 
	if (currentRow_detect == nPhotographing-1) {
		//构造存储结果信息的对象
		vector<pcb::FlawInfo> flawInfos(allDetailImage.size());
		pcb::DetectResult *detectResult = new DetectResult;
		
		Size sz(adminConfig->ImageSize_W*nCamera, adminConfig->ImageSize_H*nCamera);
		QString fullImageDir = runtimeParams->currentOutputDir + "/" + subFolders[0] + "/";

		//存储整图
		QString filePath = fullImageDir; //添加文件夹路径
		filePath += QString("fullImage_%1_%2_%3").arg(sz.width).arg(sz.height).arg(totalDefectNum); //添加文件名
		filePath += userConfig->ImageFormat; //添加文件后缀
		cv::imwrite(filePath.toStdString(), bigTempl); //存大图

		//存储细节图
		QChar fillChar = '0'; //当字符串长度不够时使用此字符进行填充
 		int defectNum = 0;// 缺陷序号
		for (auto beg = allDetailImage.begin(); beg!=allDetailImage.end(); beg++) {

			defectNum++;
			cv::Point3i info = (*beg).first;
			cv::Mat imgSeg = (*beg).second;
			QString outPath = runtimeParams->currentOutputDir + "/"; //当前序号对应的输出目录
			outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(info.x, 5, 10, fillChar).arg(info.y, 5, 10, fillChar).arg(info.z);
			outPath += userConfig->ImageFormat; //添加图像格式的后缀
			cv::imwrite(outPath.toStdString(), imgSeg);//将细节图存储到本地硬盘上

			//将分图缺陷信息保存进FlawInfo对象
			pcb::FlawInfo temp;
			temp.flawIndex = defectNum;
			temp.flawType = info.z;
			temp.xPos = info.x;
			temp.yPos = info.y;
			flawInfos[defectNum-1] = temp;
		}
		
		//向检测界面发送是否合格的信息
		bool qualified = (totalDefectNum < 1);
		emit detectFinished_detectThread(qualified);

		//将结果信息存入结果对象
		detectResult->flawInfo = flawInfos;
		detectResult->fullImage = bigTempl.clone();
		detectResult->SampleIsQualified = qualified;

		//清空历史数据
		allDetailImage.clear();
		generateBigTempl(); //重新生成大图
		totalDefectNum = -1;//将缺陷总数置位

		double t3 = clock();
		qDebug() << "====================" << pcb::chinese("存储检测结果：") <<
			(t3 - t2) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;
	}
}


/*********** 生成相应的样本文件夹、输出文件夹 ************/

//判断与产品序号对应的样本文件夹是否存在，若不存在则创建
void DefectDetecter::makeCurrentSampleDir(std::vector<QString> &subFolders)
{
	//判断顶层的sample文件夹是否存在
	runtimeParams->currentSampleDir = userConfig->SampleDirPath;
	QDir outputDir(runtimeParams->currentSampleDir);
	if (!outputDir.exists()) outputDir.mkdir(runtimeParams->currentSampleDir);

	//判断对应的型号文件夹是否存在
	runtimeParams->currentSampleDir += "/" + runtimeParams->productID.modelType;
	QDir modelDir(runtimeParams->currentSampleDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentSampleDir);

	//判断对应的批次号文件夹是否存在
	runtimeParams->currentSampleDir += "/" + runtimeParams->productID.getDateString();
	QDir batchDir(runtimeParams->currentSampleDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentSampleDir);

	//判断对应的样本编号文件夹是否存在
	runtimeParams->currentSampleDir += "/" + QString::number(runtimeParams->productID.serialNum);
	QDir resultDir(runtimeParams->currentSampleDir);
	if (!resultDir.exists()) {
		resultDir.mkdir(runtimeParams->currentSampleDir);//创建文件夹
	}
	else if (!runtimeParams->DeveloperMode) { //文件夹存在且开发者模式未开启
		pcb::clearFolder(runtimeParams->currentSampleDir, false);//清空文件夹
	}

	//添加子文件夹
	for (int i = 0; i < subFolders.size(); i++) {
		resultDir.mkdir(subFolders[i]);
	}
}

//判断与产品序号对应的输出文件夹是否存在，若不存在则创建
void DefectDetecter::makeCurrentOutputDir(vector<QString> &subFolders)
{
	//判断顶层的output文件夹是否存在
	runtimeParams->currentOutputDir = userConfig->OutputDirPath; 
	QDir outputDir(runtimeParams->currentOutputDir);
	if (!outputDir.exists()) outputDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的型号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + runtimeParams->productID.modelType;
	QDir modelDir(runtimeParams->currentOutputDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的批次号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + runtimeParams->productID.getDateString();
	QDir batchDir(runtimeParams->currentOutputDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的样本编号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + QString::number(runtimeParams->productID.serialNum);
	QDir resultDir(runtimeParams->currentOutputDir);
	if (!resultDir.exists()) {
		resultDir.mkdir(runtimeParams->currentOutputDir);//创建文件夹
	}
	else {
		pcb::clearFolder(runtimeParams->currentOutputDir, false);//清空文件夹
	}

	//添加子文件夹
	for (int i = 0; i < subFolders.size(); i++) {
		resultDir.mkdir(subFolders[i]);
	}
}
