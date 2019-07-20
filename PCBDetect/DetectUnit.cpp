#include "DetectUnit.h"
#include <iostream>

using cv::Mat;
using cv::Size;
using cv::Rect;
using cv::Point;
using std::string;


DetectUnit::DetectUnit(QObject *parent)
	: QThread(parent)
{
	adminConfig = Q_NULLPTR; //系统参数
	userConfig = Q_NULLPTR; //用户参数
	runtimeParams = Q_NULLPTR; //运行参数
	detectFunc = Q_NULLPTR; //检测函数类

	maskRoi_bl = Q_NULLPTR; //掩模区域左下角坐标，只读
	maskRoi_tr = Q_NULLPTR; //掩模区域右上角坐标，只读

	defectNum = INT_MIN; //分图中的缺陷数
}

DetectUnit::~DetectUnit()
{
	qDebug() << "~DetectUnit";
}


//开启线程，检测单个分图
void DetectUnit::run()
{
	double t1 = clock();
	defectNum = 0; //分图的缺陷数清零
	curRow = runtimeParams->currentRow_detect; //当前正在检测的行

	//读取模板掩膜
	QString mask_path = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/mask/"
		+ QString("%1_%2_mask").arg(curRow + 1).arg(curCol + 1) + userConfig->ImageFormat;
	Mat mask_roi = cv::imread(mask_path.toStdString(), 0);

	//读取模板图片
	QString templPath = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/subtempl/"
		+ QString("%1_%2").arg(curRow + 1).arg(curCol + 1) + userConfig->ImageFormat;
	Mat templGray = cv::imread(templPath.toStdString(), 0);

	double t2 = clock();
	qDebug() << "==========" << pcb::chinese("读取模板文件") << (t2 - t1) / CLOCKS_PER_SEC << "s" 
		<< " ( curCol = " << curCol << ")" << endl;

	//样本图片转为灰度图
	Mat sampGray;
	cv::cvtColor(samp, sampGray, cv::COLOR_BGR2GRAY);

	double t3 = clock();
	qDebug() << "==========" << pcb::chinese("模板形态学处理") << (t3 - t2) / CLOCKS_PER_SEC << "s" 
		<< " ( curCol = " << curCol << ")" << endl;


	//样本与模板配准
	Mat sampGrayReg, h;
	Mat imMatches;
	//载入特征的方法
	//string bin_path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString()
	//	+ "/bin/" + to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + ".bin";
	//detectFunc->load(bin_path);
	//detectFunc->alignImages_test_load(detectFunc->keypoints, detectFunc->descriptors, samp_gray, samp_gray_reg, h, imMatches);

	//每次计算的方法
	Mat templGrayRoi, sampGrayRoi;
	cv::bitwise_and(mask_roi, templGray, templGrayRoi);
	detectFunc->alignImages_test(templGrayRoi, sampGray, sampGrayReg, h, imMatches);
	double t4 = clock();
	qDebug() << "==========" << pcb::chinese("模板匹配：") << (t4 - t3) / CLOCKS_PER_SEC << "s" 
		<< " ( curCol = " << curCol << ")"  << endl;


	
	//样本二值化
	Mat sampBw = Mat::zeros(sampGray.size(), CV_8UC1);
	//自适应二值化
	//cv::adaptiveThreshold(samp_gray, sampBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
	//均值二值化
	//int meanSampGray = mean(samp_gray,mask_roi)[0];
	//cv::threshold(samp_gray, sampBw, meanSampGray, 255, cv::THRESH_BINARY_INV);
	//分块二值化
	//double ratio = 0.67;
	//Size roiSize = sampGray.size();
	//Rect upRect = Rect(0, 0, roiSize.width, int(ratio*roiSize.height));
	//Rect downRect = Rect(0, int(ratio*roiSize.height), roiSize.width, roiSize.height - int(ratio*roiSize.height));
	//int meanSampGrayUp = mean(samp_gray(upRect), mask_roi(upRect))[0];
	//cv::threshold(samp_gray(upRect), sampBw(upRect), meanSampGrayUp, 255, cv::THRESH_BINARY_INV);
	//int meanSampGrayDown = mean(samp_gray(downRect), mask_roi(downRect))[0];
	//cv::threshold(samp_gray(downRect), sampBw(downRect), meanSampGrayDown, 255, cv::THRESH_BINARY_INV);
	//局部自适应二值化
	//sampBw = detectFunc->myThresh(curCol, curRow, sampGray, *maskRoi_bl, *maskRoi_tr);

	//直接二值化
	sampBw = detectFunc->myThresh(curCol, curRow, sampGray, *maskRoi_bl, *maskRoi_tr);

	Mat sampBw_direct = sampBw.clone();

	//Mat element_a = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
	//cv::morphologyEx(sampBw, sampBw, cv::MORPH_OPEN, element_a);
	//cv::morphologyEx(sampBw, sampBw, cv::MORPH_CLOSE, element_a);

	//直接载入二值化模板
	//std::string templBwPath = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString() + "/bw/"
	//	+ to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_bw" + userConfig->ImageFormat.toStdString();
	//Mat templBw = cv::imread(templBwPath, 0);

	//每次生成模板的二值化
	Mat templBw = Mat::zeros(sampGray.size(), CV_8UC1);
	//自适应二值化
	//cv::adaptiveThreshold(templ_gray, templBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
	//均值二值化
	//int meanTemplGray = mean(templ_gray, mask_roi)[0];
	//cv::threshold(templ_gray, templBw, meanTemplGray, 255, cv::THRESH_BINARY_INV);
	//分块二值化
	//int meanTemplGrayUp = mean(templ_gray(upRect), mask_roi(upRect))[0];
	//cv::threshold(templ_gray(upRect), templBw(upRect), meanTemplGrayUp, 255, cv::THRESH_BINARY_INV);
	//int meanTemplGrayDown = mean(templ_gray(downRect), mask_roi(downRect))[0];
	//cv::threshold(templ_gray(downRect), templBw(downRect), meanTemplGrayDown, 255, cv::THRESH_BINARY_INV);
	//局部自适应二值化
	//templBw = detectFunc->myThresh(curCol, curRow, templGray, *maskRoi_bl, *maskRoi_tr);

	//直接二值化
	templBw = detectFunc->myThresh(curCol, curRow, templGray, *maskRoi_bl, *maskRoi_tr);

	Mat elementTempl = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	cv::morphologyEx(templBw, templBw, cv::MORPH_OPEN, elementTempl);
	cv::morphologyEx(templBw, templBw, cv::MORPH_CLOSE, elementTempl);

	//透射变换后有一个roi
	Mat templ_roi = Mat::ones(templGray.size(), templGray.type()) * 255;
	cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());

	Mat templRoiReverse = 255 - templ_roi;
	cv::add(sampGrayReg, templGray, sampGrayReg, templRoiReverse);

	//总的roi
	Mat roi;
	cv::bitwise_and(templ_roi, mask_roi, roi);

	//直接对roi掩膜做投射变换
	cv::warpPerspective(mask_roi, roi, h, templ_roi.size());
	cv::bitwise_and(roi, mask_roi, roi);

	//做差
	cv::warpPerspective(sampBw, sampBw, h, roi.size());//样本二值图做相应的变换，以和模板对齐
	Mat diff = detectFunc->sub_process_new(templBw, sampBw, roi);
	//Mat diff = detectFunc->sub_process_direct(templBw, sampBw, templGray, sampGrayReg, roi);
	//调试时候的边缘处理
	Size szDiff = diff.size();
	Mat diff_roi = Mat::zeros(szDiff, diff.type());
	int zoom = 50;//忽略的边缘宽度
	diff_roi(cv::Rect(zoom, zoom, szDiff.width - 2 * zoom, szDiff.height - 2 * zoom)) = 255;
	bitwise_and(diff_roi, diff, diff);



	//标记缺陷
	rectBlack = cv::Mat(templGray.size(), CV_8UC3, cv::Scalar(0, 0, 0));

	
	markedSubImage = detectFunc->markDefect_test(curCol, diff, sampGrayReg, scalingFactor, templBw, templGray, defectNum, detailImage,rectBlack);

//if (curRow == 1 && curCol == 0) {
	//保存用于调试的图片
	std::string debug_path = "D:\\PCBData\\debugImg\\" + std::to_string(curRow + 1) + "_" + std::to_string(curCol + 1) + "_";
	cv::imwrite(debug_path + std::to_string(1) + ".bmp", templGray);
	cv::imwrite(debug_path + std::to_string(2) + ".bmp", templBw);
	cv::imwrite(debug_path + std::to_string(3) + ".bmp", sampGrayReg);
	cv::imwrite(debug_path + std::to_string(4) + ".bmp", sampBw);
	cv::imwrite(debug_path + std::to_string(5) + ".bmp", diff);
	cv::imwrite(debug_path + std::to_string(6) + ".bmp", rectBlack);
	cv::imwrite(debug_path + std::to_string(7) + ".bmp", sampBw_direct);
	//sampBw_direct
//}


	//保存样本图片
	double t5 = clock();
	QString sampPath = runtimeParams->currentSampleDir + "/" + QString("%1_%2").arg(curRow + 1).arg(curCol + 1) + ".bmp";
	cv::imwrite(sampPath.toStdString(), samp);

	double t6 = clock();
	qDebug() << "==========" << pcb::chinese("保存样本分图") << (t6 - t5) / CLOCKS_PER_SEC << "s" 
		<< " ( curCol = " << curCol << ")" << endl;
}
