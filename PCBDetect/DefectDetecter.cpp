#include "DefectDetecter.h"
#include <exception>

using pcb::CvMatVector;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::DetectResult;
using pcb::QImageVector;
using cv::Mat;
using std::string;


DefectDetecter::DefectDetecter() 
{
	adminConfig = Q_NULLPTR; //系统参数
	detectConfig = Q_NULLPTR; //用户参数
	detectParams = Q_NULLPTR; //运行参数
	detectResult = Q_NULLPTR; //检测结果
	cvmatSamples = Q_NULLPTR; //正在检测的一行样本
	detectFunc = Q_NULLPTR; //检测辅助类
	detectState = Default; //检测状态（用于界面显示和程序调试）
	defectNum = 0; //缺陷数
}

DefectDetecter::~DefectDetecter() 
{
	qDebug() << "~DefectDetecter";
	delete detectFunc;
	detectFunc = Q_NULLPTR;
}

//初始化templFunc
void DefectDetecter::initDetectFunc()
{
	detectFunc = new DetectFunc;
	detectFunc->setAdminConfig(adminConfig);
	detectFunc->setDetectConfig(detectConfig);
	detectFunc->setDetectParams(detectParams);
	detectFunc->setDetectResult(detectResult);
	detectFunc->generateBigTempl();
}


/***************** 检测 ******************/

void DefectDetecter::detect()
{
	qDebug() << ">>>>>>>>>> " << pcb::chinese("开始检测 ... ") <<
		"( currentRow_detect -" << detectParams->currentRow_detect << ")";

	detectState = DetectState::Start; //设置检测状态
	emit updateDetectState_detecter(detectState);
	double t1 = clock();

	//开始检测
	int currentRow_detect = detectParams->currentRow_detect;
	for (int i = 0; i < (*cvmatSamples)[detectParams->currentRow_detect].size(); i++) {
		double t1 = clock();

		//读取模板图片
		string templPath = (detectConfig->TemplDirPath + "/" + detectParams->sampleModelNum + "/subtempl/"
			+ QString::number(currentRow_detect + 1) + "_" + QString::number(i + 1)  
			+ detectConfig->ImageFormat).toStdString();
		Mat templ_gray = cv::imread(templPath, 0);

		//读取模板二值图
		std::string templBwPath = detectConfig->TemplDirPath.toStdString() + "/" + detectParams->sampleModelNum.toStdString() + "/bw/"
		+ to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1)+"_bw" + detectConfig->ImageFormat.toStdString();
		Mat templBw = cv::imread(templBwPath,0);



		//获取样本图片
		Mat samp = *((*cvmatSamples)[detectParams->currentRow_detect][i]);
		Mat samp_gray;
		cvtColor(samp, samp_gray, COLOR_BGR2GRAY);

	
		//保存样本图片
		string batch_path = (detectConfig->SampleDirPath).toStdString() + "\\" + detectParams->sampleModelNum.toStdString();//检查输出文件夹中型号文件是否存在
		if (0 != _access(batch_path.c_str(), 0))
			_mkdir(batch_path.c_str());
		string num_path = batch_path + "\\" + detectParams->sampleBatchNum.toStdString();//检查批次号文件夹是否存在
		if (0 != _access(num_path.c_str(), 0))
			_mkdir(num_path.c_str());
		string out_path = num_path + "\\" + detectParams->sampleNum.toStdString();//检查编号文件夹是否存在
		if (0 != _access(out_path.c_str(), 0))
			_mkdir(out_path.c_str());
		std::string sampPath = out_path + "\\" + to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + detectConfig->ImageFormat.toStdString();
		imwrite(sampPath,samp);


		//样本二值化
		cv::Mat sampBw;
		cv::adaptiveThreshold(samp_gray, sampBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		cv::Mat element_a = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		cv::morphologyEx(sampBw, sampBw, cv::MORPH_OPEN, element_a);
		cv::morphologyEx(sampBw, sampBw, cv::MORPH_CLOSE, element_a);



		double t2 = clock();
		qDebug() << QString::fromLocal8Bit("==========模板形态学处理：") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;

		try {
			//样本与模板配准
			cv::Mat samp_gray_reg, h;
			cv::Mat imMatches;
			//detectFunc.alignImages(samp_gray, templ_gray, samp_gray_reg, h, imMatches);
			string bin_path = detectConfig->TemplDirPath.toStdString() + "/" + detectParams->sampleModelNum.toStdString()
				+ "/bin/" + to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + ".bin";
			detectFunc->load(bin_path);
			detectFunc->alignImages_test_load(detectFunc->keypoints, detectFunc->descriptors, samp_gray, samp_gray_reg, h, imMatches);
			double t3 = clock();
			qDebug() << QString::fromLocal8Bit("==========配准时间：") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;

			//透射变换后有一个roi
			cv::Mat templ_roi = cv::Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
			cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());

			cv::Mat templRoiReverse = 255 - templ_roi;
			cv::add(samp_gray_reg, templ_gray, samp_gray_reg, templRoiReverse);

			//边缘有一个roi
			string mask_path = detectConfig->TemplDirPath.toStdString() + "/" + detectParams->sampleModelNum.toStdString()
				+ "/mask/" + to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_mask" + detectConfig->ImageFormat.toStdString();
			cv::Mat mask_roi = cv::imread(mask_path, 0);

			//总的roi
			cv::Mat roi;
			cv::bitwise_and(templ_roi, mask_roi, roi);

			//做差
			cv::warpPerspective(sampBw, sampBw, h, roi.size());//样本二值图做相应的变换，以和模板对齐
			cv::Mat diff = detectFunc->sub_process_new(templBw, sampBw, roi);

			//调试时候的边缘处理
			cv::Size szDiff = diff.size();
			cv::Mat diff_roi = cv::Mat::zeros(szDiff, diff.type());
			int zoom = 50;//忽略的边缘宽度
			diff_roi(cv::Rect(zoom, zoom, szDiff.width - 2 * zoom, szDiff.height - 2 * zoom)) = 255;
			bitwise_and(diff_roi, diff, diff);

			//标记缺陷
			detectFunc->markDefect_test(diff, samp_gray_reg, templBw, templ_gray, defectNum, i);
			continue;

			/*****测试代码, 保存疑分图的模板，配准样本，二值图，差值图*****/
			Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			dilate(diff, diff, kernel_small);//对差值图像做膨胀，方便对类型进行判断
			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i>   hierarchy;
			cv::findContours(diff, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
			cv::cvtColor(diff, diff, COLOR_GRAY2BGR);
			for (int i = 0; i < contours.size(); i++) {
				//if (contourArea(contours[i], false) > 50)//缺陷的最大和最小面积
				//{
				Rect rect = boundingRect(Mat(contours[i]));
				Rect rect_out = Rect(rect.x - 5, rect.y, rect.width + 10, rect.height + 10);
				Mat temp_area = templ_gray(rect_out);
				Mat samp_area = samp_gray_reg(rect_out);
				Mat mask;
				//double res = computeECC(temp_area, samp_area,mask);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
				auto res = detectFunc->getMSSIM(temp_area, samp_area);
				//qDebug() << QString::fromLocal8Bit("===========相关系数") << res;
				if (res[0] >= 0.8)
					continue;
				//Mat result;
				//result.create(1, 1, CV_32FC1);
				//matchTemplate(temp_area, samp_area, result, TM_SQDIFF_NORMED);
				//double minVal; double maxVal; Point minLoc; Point maxLoc;
				//Point matchLoc;
				//minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

				putText(diff, to_string(res[0]), Point(rect.x, rect.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//在图片上写文字
				rectangle(diff, rect_out, Scalar(0, 255, 0));
				drawContours(diff, contours, i, cv::Scalar(0, 0, 255), 2, 8);
				//}

			}

			string name_head = to_string(detectParams->currentRow_detect + 1) + "_" + to_string(i + 1);
			string testPath = "D:\\tr_project\\project\\pcb_detect_new\\detectfunc_test_res\\res_05\\diff_new\\";
			cv::imwrite(testPath + name_head + "_5diff.jpg", diff);
			cv::imwrite(testPath + name_head + "_2templBw.jpg", templBw);
			cv::imwrite(testPath + name_head + "_4sampBw.jpg", sampBw);
			cv::imwrite(testPath + name_head + "_1templ_gray.jpg", templ_gray);
			cv::imwrite(testPath + name_head + "_3samp_gray_reg.jpg", samp_gray_reg);


		}catch (std::exception e) {
			qDebug() << detectParams->currentRow_detect + 1 << "_" << i + 1 << "_" <<
				QString::fromLocal8Bit("出现异常");
		}	
	}
	if (detectParams->currentRow_detect + 1 == detectParams->nPhotographing) {
		if (defectNum > 0) {
			cv::Size sz = detectFunc->getBigTempl().size();
			cv::Mat dst;
			cv::resize(detectFunc->getBigTempl(), dst, cv::Size(sz.width*0.25, sz.height*0.25), (0, 0), (0, 0), cv::INTER_LINEAR);
			string fullImagePath = detectFunc->out_path + "/fullImage";
			_mkdir(fullImagePath.c_str());
			cv::imwrite(fullImagePath + "/fullImage_" + std::to_string(sz.width) + "_" + std::to_string(sz.height) + "_" + to_string(defectNum) + ".jpg", dst);
			detectFunc->generateBigTempl();
			defectNum = 0;//将整体缺陷置0
		}
	}

	//检测结束
	double t2 = clock();
	qDebug() << ">>>>>>>>>> " << pcb::chinese("当前行检测结束") << 
		(t2 - t1) << "ms  ( currentRow_detect -" << detectParams->currentRow_detect << ")";
	
	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);
	pcb::delay(10);
	bool qualified = (defectNum < 100);

	if (detectParams->currentRow_detect == detectParams->nPhotographing-1)
		emit detectFinished_detectThread(qualified);
}


