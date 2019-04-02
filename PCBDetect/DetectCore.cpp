#include "DetectCore.h"
#include <exception>

using Ui::CvMatVector;
using Ui::DetectConfig;
using Ui::DetectParams;
using Ui::DetectResult;
using Ui::QImageVector;
using cv::Mat;
using std::string;
using std::to_string;


DetectCore::DetectCore() {}

DetectCore::~DetectCore() {}


/***************** 检测 ******************/

void DetectCore::doDetect()
{
	double t1 = clock();

	detectState = 0; //设置检测状态
	emit sig_detectState_detectCore(detectState);

	//开始检测
	detectState = 1;
	emit sig_detectState_detectCore(detectState);

	//Ui::delay(1000); //执行检测
	// ...
	DetectFunc detectFunc;
	detectFunc.setDetectConfig(config);
	detectFunc.setDetectParams(params);

	int currentRow_detect = params->currentRow_detect;
	for (int i = 0; i < samples->size(); i++) {
		double t1 = clock();

		//读取模板图片
		string templPath = (config->TemplDirPath + "/" + params->sampleModelNum + "/"
			+ QString::number(currentRow_detect + 1) + "_" + QString::number(i + 1)  + config->ImageFormat).toStdString();
		Mat templ_gray = cv::imread(templPath, 0);

		//读取样本图片
		Mat samp = *((*samples)[i]);
		Mat samp_gray;
		cvtColor(samp, samp_gray, CV_BGR2GRAY);

		//模板二值化及形态学处理
		Mat templ_bw;
		cv::threshold(templ_gray, templ_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::morphologyEx(templ_bw, templ_bw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templ_bw, templ_bw, cv::MORPH_CLOSE, element_b);

		double t2 = clock();
		qDebug() << QString::fromLocal8Bit("===========模板形态学处理：") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;

		try {
			//样本与模板配准
			Mat samp_gray_reg, h;
			Mat imMatches;
			detectFunc.alignImages(samp_gray, templ_gray, samp_gray_reg, h, imMatches);
			double t3 = clock();
			qDebug() << QString::fromLocal8Bit("===========配准时间：") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;

			//对模板做掩膜处理,样本做变换后部分区域超出边界，所以要对模板及其二值图做掩膜处理
			Mat templ_gray_reg, templ_bw_reg;
			Mat templ_roi = cv::Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
			cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());
			cv::bitwise_and(templ_roi, templ_gray, templ_gray_reg);
			cv::bitwise_and(templ_roi, templ_bw, templ_bw_reg);


			//预处理
			Mat diff = detectFunc.sub_process(templ_gray_reg, samp_gray_reg);
			string mask_path = config->TemplDirPath.toStdString() + "/" + params->sampleModelNum.toStdString()
				+ "/mask/" + to_string(params->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_mask" + config->ImageFormat.toStdString();
			Mat roi_mask = cv::imread(mask_path, 0);
			cv::bitwise_and(diff, roi_mask, diff);

			time_t t4 = clock();
			qDebug() << QString::fromLocal8Bit("===========做差时间：") << double(t4 - t3) / CLOCKS_PER_SEC << "s" << endl;
			string path_head;
			//path_head += to_string(params->currentRow_detect + 1) + "_" + to_string(i + 1) + "_";
			//cv::imwrite("./res_06/diff/" + path_head + "_diff.jpg", diff);
			//cv::imwrite("./res_06/match/" + path_head + "_imMatches.jpg", imMatches);
			//cv::imwrite("./res_06/reg/" + path_head + "_samp_gray_reg.jpg", samp_gray_reg);
			detectFunc.markDefect(diff, samp_gray_reg, templ_bw_reg, templ_gray_reg, defectNum, i);
			qDebug() << endl;
		}catch (std::exception e) {
			qDebug() << params->currentRow_detect + 1 << "_" << i + 1 << "_" <<
				QString::fromLocal8Bit("出现异常");
		}	
	}
	if (params->currentRow_detect + 1 == config->nPhotographing)
		defectNum = 0;



	QString s = config->TemplDirPath;

	//检测结束
	detectState = 2;
	emit sig_detectState_detectCore(detectState);

	double t2 = clock();
	qDebug() << "detectSampleImages :" << (t2 - t1) << "ms  ( currentRow_detect -" << params->currentRow_detect << ")";
}
