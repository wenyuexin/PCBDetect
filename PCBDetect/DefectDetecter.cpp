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

	double factorW = 1.0 * runtimeParams->ScreenRect.width() / originalfullImgSize.width;
	double factorH = 1.0 * runtimeParams->ScreenRect.height() / originalfullImgSize.height;
	scalingFactor = qMin(factorW, factorH); //缩放因子

	scalingFactor = 1;

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
		QString cornerPointsPath = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/mask/cornerPoints.bin";
		cv::FileStorage store_new(cornerPointsPath.toStdString(), cv::FileStorage::READ);
		cv::FileNode node = store_new["cornerPoints"];
		vector<cv::Point2i> res;
		cv::read(node, res);
		if (res.size() == 0) {
			qDebug() << pcb::chinese("DefectDetecter: 加载模板的掩模区域坐标失败");
			errorCode = LoadTemplMaskRoiError; return;
		}
		maskRoi_bl = res[0];
		maskRoi_tr = res[1];
		store_new.release();
	}

	//向检测单元传入适用于整个PCB板的参数
	if (currentRow_detect == 0) {
		for (int i = 0; i < nPhotographing; i++) {
			detectUnits[i]->setMaskRoi(&maskRoi_bl, &maskRoi_tr);//设置掩模区域坐标
			detectUnits[i]->setScalingFactor(scalingFactor); //设置缩放因子
			detectUnits[i]->setScaledFullImageSize(&scaledFullImageSize); //设置缩放后的整图图像尺寸
			detectUnits[i]->setScaledSubImageSize(&scaledSubImageSize); //设置缩放后的分图图像尺寸
		}
	}

	//向检测单元传入适用于当前行的参数
	CvMatVector subImages = (*cvmatSamples)[currentRow_detect];
	for (int i = 0; i < nPhotographing; i++) {
		detectUnits[i]->setSubImage(*subImages[i]);//设置需要检测的分图
	}

	//开启若干检测线程，检测当前的一行分图
	for (int i = 0; i < nPhotographing; i++) {
		detectUnits[i]->start(); //开始转换
	}

	//等待所有检测单元运行结束
	for (int i = 0; i < nPhotographing; i++) {
		detectUnits[i]->wait();
	}

	//整合当前行的检测结果
	totalDefectNum = 0; //缺陷总数
	for (int i = 0; i < nPhotographing; i++) {
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
	}
	
	
	//for (int i = 0; i < (*cvmatSamples)[currentRow_detect].size(); i++) {
	//	int curRow = currentRow_detect;//当前行
	//	int curCol = i;//当前列
	//
	//	double t1 = clock();
	//	//读取模板掩膜
	//	QString mask_path = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/mask/" 
	//		+ QString("%1_%2_mask").arg(curRow+1).arg(curCol+1) + userConfig->ImageFormat;
	//	cv::Mat mask_roi = cv::imread(mask_path.toStdString(), 0);
	//
	//	//读取模板图片
	//	QString templPath = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/subtempl/"
	//		+ QString("%1_%2").arg(curRow + 1).arg(curCol + 1) + userConfig->ImageFormat;
	//	Mat templ_gray = cv::imread(templPath.toStdString(), 0);
	//
	//	//获取样本图片
	//	Mat samp = *((*cvmatSamples)[curRow][curCol]);
	//	Mat samp_gray;
	//	cv::cvtColor(samp, samp_gray, cv::COLOR_BGR2GRAY);
	//
	//	//保存样本图片
	//	QString sampPath = runtimeParams->currentSampleDir + "/" + QString("%1_%2").arg(curRow + 1).arg(curCol + 1) + ".bmp";
	//	cv::imwrite(sampPath.toStdString(), samp);
	//
	//	double t2 = clock();
	//	qDebug() << "==========" << pcb::chinese("模板形态学处理") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;
	//
	//		
	//	//样本与模板配准
	//	cv::Mat samp_gray_reg, h;
	//	cv::Mat imMatches;
	//	//载入特征的方法
	//	//string bin_path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString()
	//	//	+ "/bin/" + to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + ".bin";
	//	//detectFunc->load(bin_path);
	//	//detectFunc->alignImages_test_load(detectFunc->keypoints, detectFunc->descriptors, samp_gray, samp_gray_reg, h, imMatches);
	//
	//	//每次计算的方法
	//	Mat templGrayRoi, sampGrayRoi;
	//	cv::bitwise_and(mask_roi, templ_gray, templGrayRoi);
	//	detectFunc->alignImages_test(templGrayRoi, samp_gray, samp_gray_reg, h, imMatches);
	//	double t3 = clock();
	//	qDebug() << "==========" << pcb::chinese("模板匹配：") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;
	//
	//	double ratio = 0.67;
	//	Size roiSize = samp_gray.size();
	//	Rect upRect = Rect(0, 0, roiSize.width, int(ratio*roiSize.height));
	//	Rect downRect = Rect(0, int(ratio*roiSize.height), roiSize.width, roiSize.height - int(ratio*roiSize.height));
	//	//样本二值化
	//	Mat sampBw = Mat::zeros(samp_gray.size(), CV_8UC1);
	//	//自适应二值化
	//	//cv::adaptiveThreshold(samp_gray, sampBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
	//	//均值二值化
	//	//int meanSampGray = mean(samp_gray,mask_roi)[0];
	//	//cv::threshold(samp_gray, sampBw, meanSampGray, 255, cv::THRESH_BINARY_INV);
	//	//分块二值化
	//	//int meanSampGrayUp = mean(samp_gray(upRect), mask_roi(upRect))[0];
	//	//cv::threshold(samp_gray(upRect), sampBw(upRect), meanSampGrayUp, 255, cv::THRESH_BINARY_INV);
	//	//int meanSampGrayDown = mean(samp_gray(downRect), mask_roi(downRect))[0];
	//	//cv::threshold(samp_gray(downRect), sampBw(downRect), meanSampGrayDown, 255, cv::THRESH_BINARY_INV);
	//	//局部自适应二值化
	//	cv::Point bl(detectFunc->get_bl());
	//	cv::Point tr(detectFunc->get_tr());
	//
	//	sampBw = detectFunc->myThresh(curCol, curRow, samp_gray, bl, tr);
	//
	//	Mat element_a = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
	//	cv::morphologyEx(sampBw, sampBw, cv::MORPH_OPEN, element_a);
	//	cv::morphologyEx(sampBw, sampBw, cv::MORPH_CLOSE, element_a);
	//
	//
	//	//直接载入二值化模板
	//	//std::string templBwPath = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString() + "/bw/"
	//	//	+ to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_bw" + userConfig->ImageFormat.toStdString();
	//	//Mat templBw = cv::imread(templBwPath, 0);
	//
	//	//每次生成模板的二值化
	//	Mat templBw = Mat::zeros(samp_gray.size(), CV_8UC1);
	//	//自适应二值化
	//	//cv::adaptiveThreshold(templ_gray, templBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
	//	//均值二值化
	//	//int meanTemplGray = mean(templ_gray, mask_roi)[0];
	//	//cv::threshold(templ_gray, templBw, meanTemplGray, 255, cv::THRESH_BINARY_INV);
	//	//分块二值化
	//	//int meanTemplGrayUp = mean(templ_gray(upRect), mask_roi(upRect))[0];
	//	//cv::threshold(templ_gray(upRect), templBw(upRect), meanTemplGrayUp, 255, cv::THRESH_BINARY_INV);
	//	//int meanTemplGrayDown = mean(templ_gray(downRect), mask_roi(downRect))[0];
	//	//cv::threshold(templ_gray(downRect), templBw(downRect), meanTemplGrayDown, 255, cv::THRESH_BINARY_INV);
	//	//局部自适应二值化
	//	templBw = detectFunc->myThresh(curCol, curRow, templ_gray, bl, tr);
	//
	//	Mat elementTempl = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	//	cv::morphologyEx(templBw, templBw, cv::MORPH_OPEN, elementTempl);
	//	cv::morphologyEx(templBw, templBw, cv::MORPH_CLOSE, elementTempl);
	//
	//
	//	//透射变换后有一个roi
	//	Mat templ_roi = Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
	//	cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());
	//
	//	Mat templRoiReverse = 255 - templ_roi;
	//	cv::add(samp_gray_reg, templ_gray, samp_gray_reg, templRoiReverse);
	//
	//	//总的roi
	//	Mat roi;
	//	cv::bitwise_and(templ_roi, mask_roi, roi);
	//
	//	//直接对roi掩膜做投射变换
	//	cv::warpPerspective(mask_roi, roi, h, templ_roi.size());
	//	cv::bitwise_and(roi, mask_roi, roi);
	//
	//	//做差
	//	cv::warpPerspective(sampBw, sampBw, h, roi.size());//样本二值图做相应的变换，以和模板对齐
	//	//Mat diff = detectFunc->sub_process_new(templBw, sampBw, roi);
	//	Mat diff = detectFunc->sub_process_direct(templBw, sampBw, templ_gray, samp_gray_reg, roi);
	//	//调试时候的边缘处理
	//	Size szDiff = diff.size();
	//	Mat diff_roi = Mat::zeros(szDiff, diff.type());
	//	int zoom = 50;//忽略的边缘宽度
	//	diff_roi(cv::Rect(zoom, zoom, szDiff.width - 2 * zoom, szDiff.height - 2 * zoom)) = 255;
	//	bitwise_and(diff_roi, diff, diff);
	//
	//	string debug_path = "D:\\PCBData\\debugImg\\" + to_string(curRow) + "_" + to_string(curCol) + "_";
	//	cv::imwrite(debug_path + to_string(1) + ".bmp", templ_gray);
	//	cv::imwrite(debug_path + to_string(2) + ".bmp", templBw);
	//	cv::imwrite(debug_path + to_string(3) + ".bmp", samp_gray_reg);
	//	cv::imwrite(debug_path + to_string(4) + ".bmp", sampBw);
	//	cv::imwrite(debug_path + to_string(5) + ".bmp", diff);
	//
	//	//标记缺陷
	//	detectFunc->markDefect_test(diff, samp_gray_reg, templBw, templ_gray, defectNum, i);
	//	continue;
	//}


	//如果当前检测的是最后一行图像
	if (currentRow_detect == nPhotographing-1) {
		Size sz(adminConfig->ImageSize_W*nCamera, adminConfig->ImageSize_H*nCamera);
		QString fullImageDir = runtimeParams->currentOutputDir + "/" + subFolders[0] + "/";

		QString filePath = fullImageDir; //添加文件夹路径
		filePath += QString("fullImage_%1_%2_%3").arg(sz.width).arg(sz.height).arg(totalDefectNum); //添加文件名
		filePath += userConfig->ImageFormat; //添加文件后缀
		cv::imwrite(filePath.toStdString(), bigTempl); //存图

		//存储细节图
		QChar fillChar = '0'; //当字符串长度不够时使用此字符进行填充
		int defectNum = 0;//缺陷序号
		for (auto beg = allDetailImage.begin(); beg!=allDetailImage.end(); beg++) {
			defectNum++;
			cv::Point3i info = (*beg).first;
			cv::Mat imgSeg = (*beg).second;
			QString outPath = runtimeParams->currentOutputDir + "/"; //当前序号对应的输出目录
			outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(info.x, 5, 10, fillChar).arg(info.y, 5, 10, fillChar).arg(info.z);
			outPath += userConfig->ImageFormat; //添加图像格式的后缀
			cv::imwrite(outPath.toStdString(), imgSeg);//将细节图存储到本地硬盘上
		}
		
		//向检测界面发送是否合格的信息
		bool qualified = (totalDefectNum < 100);
		emit detectFinished_detectThread(qualified);

		//清空历史数据
		generateBigTempl(); //重新生成大图
		totalDefectNum = -1;//将缺陷总数置位
	}

	//检测结束
	double t2 = clock();
	qDebug() << "====================" << pcb::chinese("当前行检测结束：") << 
		(t2 - t1) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;
	
	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);
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
	runtimeParams->currentSampleDir += "/" + runtimeParams->sampleModelNum;
	QDir modelDir(runtimeParams->currentSampleDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentSampleDir);

	//判断对应的批次号文件夹是否存在
	runtimeParams->currentSampleDir += "/" + runtimeParams->sampleBatchNum;
	QDir batchDir(runtimeParams->currentSampleDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentSampleDir);

	//判断对应的样本编号文件夹是否存在
	runtimeParams->currentSampleDir += "/" + runtimeParams->sampleNum;
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
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleModelNum;
	QDir modelDir(runtimeParams->currentOutputDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的批次号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleBatchNum;
	QDir batchDir(runtimeParams->currentOutputDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的样本编号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleNum;
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
