#include "DefectDetecter.h"
#include <exception>

using pcb::CvMatVector;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::DetectResult;
using pcb::QImageVector;
using cv::Mat;
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
	detectResult = Q_NULLPTR; //检测结�?
	cvmatSamples = Q_NULLPTR; //正在检测的一行样�?
	detectFunc = Q_NULLPTR; //检测辅助类
	detectState = Default; //检测状态（用于界面显示和程序调试）
	defectNum = 0; //缺陷�?
}

void DefectDetecter::init()
{
	initDetectFunc();
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
	delete detectFunc;
	detectFunc = new DetectFunc;
	detectFunc->setAdminConfig(adminConfig);
	detectFunc->setUserConfig(userConfig);
	detectFunc->setRuntimeParams(runtimeParams);
	detectFunc->setDetectResult(detectResult);
	detectFunc->generateBigTempl();
}


/***************** 检�?******************/

void DefectDetecter::detect()
{
<<<<<<< HEAD
	qDebug() << ">>>>>>>>>> " << pcb::chinese("开始检�?... ") <<
		"( currentRow_detect -" << runtimeParams->currentRow_detect << ")";
=======
	qDebug() << ">>>>>>>>>> " << pcb::chinese("��ʼ��� ... ") <<
		"( currentRow_detect =" << runtimeParams->currentRow_detect << ")" << endl;
>>>>>>> wenyuexin


	detectState = DetectState::Start; //设置检测状�?
	emit updateDetectState_detecter(detectState);
	double t1 = clock();

<<<<<<< HEAD
	//检测对应的输出目录是否存在
	makeCurrentOutputDir();

	//开始检�?
	int currentRow_detect = runtimeParams->currentRow_detect;
	for (int i = 0; i < (*cvmatSamples)[runtimeParams->currentRow_detect].size(); i++) {
		int curRow = runtimeParams->currentRow_detect;//当前�?
		int curCol = i;//当前�?

		qDebug() << curRow << "_" << curCol;

		double t1 = clock();
		//读取模板掩膜
		string mask_path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString() + "/mask/" 
			+ to_string(curRow+1)+ "_" + to_string(curCol+1) + "_mask" 
			+ userConfig->ImageFormat.toStdString();
		cv::Mat mask_roi = cv::imread(mask_path, 0);

		//读取模板图片
		string templPath = (userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/subtempl/"
			+ QString::number(currentRow_detect + 1) + "_" + QString::number(i + 1)  
			+ userConfig->ImageFormat).toStdString();
		Mat templ_gray = cv::imread(templPath, 0);

		//获取样本图片
		Mat samp = *((*cvmatSamples)[runtimeParams->currentRow_detect][i]);
=======
	int currentRow_detect = runtimeParams->currentRow_detect;
	int nCamera = runtimeParams->nCamera;
	int nPhotographing = runtimeParams->nPhotographing;

	//����Ӧ�����Ŀ¼�Ƿ����
	vector<QString> subFolders { "fullImage" };
	if (currentRow_detect == 0) makeCurrentOutputDir(subFolders);

	//��ʼ���
	for (int i = 0; i < (*cvmatSamples)[currentRow_detect].size(); i++) {
		int curRow = currentRow_detect;//��ǰ��
		int curCol = i;//��ǰ��

		double t1 = clock();
		//��ȡģ����Ĥ
		QString mask_path = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/mask/" 
			+ QString("%1_%2_mask").arg(curRow+1).arg(curCol+1) + userConfig->ImageFormat;
		cv::Mat mask_roi = cv::imread(mask_path.toStdString(), 0);

		//��ȡģ��ͼƬ
		QString templPath = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/subtempl/"
			+ QString("%1_%2").arg(currentRow_detect + 1).arg(i + 1) + userConfig->ImageFormat;
		Mat templ_gray = cv::imread(templPath.toStdString(), 0);

		//��ȡ����ͼƬ
		Mat samp = *((*cvmatSamples)[currentRow_detect][i]);
>>>>>>> wenyuexin
		Mat samp_gray;
		cvtColor(samp, samp_gray, cv::COLOR_BGR2GRAY);

		//测试时后保存样本图片
		string batch_path = (userConfig->SampleDirPath).toStdString() + "\\" + runtimeParams->sampleModelNum.toStdString();//检查输出文件夹中型号文件是否存�?
		if (0 != _access(batch_path.c_str(), 0))
			_mkdir(batch_path.c_str());
		string num_path = batch_path + "\\" + runtimeParams->sampleBatchNum.toStdString();//检查批次号文件夹是否存�?
		if (0 != _access(num_path.c_str(), 0))
			_mkdir(num_path.c_str());
		string out_path = num_path + "\\" + runtimeParams->sampleNum.toStdString();//检查编号文件夹是否存在
		if (0 != _access(out_path.c_str(), 0))
			_mkdir(out_path.c_str());
		string sampPath = out_path + "\\" + to_string(currentRow_detect + 1) + "_" + std::to_string(i + 1) + userConfig->ImageFormat.toStdString();
		imwrite(sampPath,samp);

		double t2 = clock();
		qDebug() << QString::fromLocal8Bit("模板形态学处理") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;

		//try {
			//样本与模板配�?
			cv::Mat samp_gray_reg, h;
			cv::Mat imMatches;
			//载入特征的方�?
			//string bin_path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString()
			//	+ "/bin/" + to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + ".bin";
			//detectFunc->load(bin_path);
			//detectFunc->alignImages_test_load(detectFunc->keypoints, detectFunc->descriptors, samp_gray, samp_gray_reg, h, imMatches);

			//每次计算的方�?
			Mat templGrayRoi, sampGrayRoi;
			cv::bitwise_and(mask_roi, templ_gray, templGrayRoi);
			detectFunc->alignImages_test(templGrayRoi, samp_gray, samp_gray_reg, h, imMatches);
			double t3 = clock();
			qDebug() << QString::fromLocal8Bit("配准时间") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;

			double ratio = 0.67;
			Size roiSize = samp_gray.size();
			Rect upRect = Rect(0, 0, roiSize.width, int(ratio*roiSize.height));
			Rect downRect = Rect(0, int(ratio*roiSize.height), roiSize.width, roiSize.height - int(ratio*roiSize.height));
			//样本二值化
			Mat sampBw = Mat::zeros(samp_gray.size(), CV_8UC1);
			//自适应二值化
			//cv::adaptiveThreshold(samp_gray, sampBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
			//均值二值化
			//int meanSampGray = mean(samp_gray,mask_roi)[0];
			//cv::threshold(samp_gray, sampBw, meanSampGray, 255, cv::THRESH_BINARY_INV);
			//分块二值化
			int meanSampGrayUp = mean(samp_gray(upRect), mask_roi(upRect))[0];
			cv::threshold(samp_gray(upRect), sampBw(upRect), meanSampGrayUp, 255, cv::THRESH_BINARY_INV);
			int meanSampGrayDown = mean(samp_gray(downRect), mask_roi(downRect))[0];
			cv::threshold(samp_gray(downRect), sampBw(downRect), meanSampGrayDown, 255, cv::THRESH_BINARY_INV);

			Mat element_a = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
			cv::morphologyEx(sampBw, sampBw, cv::MORPH_OPEN, element_a);
			cv::morphologyEx(sampBw, sampBw, cv::MORPH_CLOSE, element_a);
			//直接载入二值化模板
			//std::string templBwPath = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString() + "/bw/"
			//	+ to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_bw" + userConfig->ImageFormat.toStdString();
			//Mat templBw = cv::imread(templBwPath, 0);

			//每次生成模板的二值化
			Mat templBw = Mat::zeros(samp_gray.size(), CV_8UC1);
			//自适应二值化
			//cv::adaptiveThreshold(templ_gray, templBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
			//均值二值化
			//int meanTemplGray = mean(templ_gray, mask_roi)[0];
			//cv::threshold(templ_gray, templBw, meanTemplGray, 255, cv::THRESH_BINARY_INV);
			//分块二值化
			int meanTemplGrayUp = mean(templ_gray(upRect), mask_roi(upRect))[0];
			cv::threshold(templ_gray(upRect), templBw(upRect), meanTemplGrayUp, 255, cv::THRESH_BINARY_INV);
			int meanTemplGrayDown = mean(templ_gray(downRect), mask_roi(downRect))[0];
			cv::threshold(templ_gray(downRect), templBw(downRect), meanTemplGrayDown, 255, cv::THRESH_BINARY_INV);

			Mat elementTempl = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
			cv::morphologyEx(templBw, templBw, cv::MORPH_OPEN, elementTempl);
			cv::morphologyEx(templBw, templBw, cv::MORPH_CLOSE, elementTempl);


			qDebug() << QString::fromLocal8Bit("==========二值化完成");

			//透射变换后有一个roi
			Mat templ_roi = Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
			cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());

			Mat templRoiReverse = 255 - templ_roi;
			cv::add(samp_gray_reg, templ_gray, samp_gray_reg, templRoiReverse);

			//总的roi
			Mat roi;
			cv::bitwise_and(templ_roi, mask_roi, roi);

			//做差
			cv::warpPerspective(sampBw, sampBw, h, roi.size());//样本二值图做相应的变换，以和模板对�?
			Mat diff = detectFunc->sub_process_new(templBw, sampBw, roi);

			

			//调试时候的边缘处理
			Size szDiff = diff.size();
			Mat diff_roi = Mat::zeros(szDiff, diff.type());
			int zoom = 50;//忽略的边缘宽�?
			diff_roi(cv::Rect(zoom, zoom, szDiff.width - 2 * zoom, szDiff.height - 2 * zoom)) = 255;
			bitwise_and(diff_roi, diff, diff);

			qDebug() << QString::fromLocal8Bit("");

			//标记缺陷
			detectFunc->markDefect_test(diff, samp_gray_reg, templBw, templ_gray, defectNum, i);
			continue;
		//}catch (std::exception e) {
		//	qDebug() << runtimeParams->currentRow_detect + 1 << "_" << i + 1 << "_" <<
		//		QString::fromLocal8Bit("出现异常");
		//}	
	}

<<<<<<< HEAD
	//如果当前检测的是最后一行图�?
	if (runtimeParams->currentRow_detect + 1 == runtimeParams->nPhotographing) {
		if (defectNum > 0) {

			Size sz(adminConfig->ImageSize_W*runtimeParams->nCamera, adminConfig->ImageSize_H*runtimeParams->nCamera);
			Mat dst = detectFunc->getBigTempl();
			//cv::resize(detectFunc->getBigTempl(), dst, cv::Size(sz.width*0.25, sz.height*0.25), (0, 0), (0, 0), cv::INTER_LINEAR);
			string fullImagePath = runtimeParams->currentOutputDir.toStdString() + "/fullImage";

			_mkdir(fullImagePath.c_str());
			string defectNumStr = to_string(defectNum);
			switch (defectNumStr.size()) {
			case 1:
				defectNumStr = "00" + defectNumStr;
				break;
			case 2:
				defectNumStr = "0" + defectNumStr;
				break;
			}
			cv::imwrite(fullImagePath + "/fullImage_" + std::to_string(sz.width) + "_" + std::to_string(sz.height) + "_" + defectNumStr + ".jpg", dst);
			detectFunc->generateBigTempl();
			defectNum = 0;//将整体缺陷置0
		}
=======
	//�����ǰ���������һ��ͼ��
	if (currentRow_detect + 1 == nPhotographing) {
		Size sz(adminConfig->ImageSize_W*nCamera, adminConfig->ImageSize_H*nCamera);
		Mat dst = detectFunc->getBigTempl();
		QString fullImageDir = runtimeParams->currentOutputDir + "/" + subFolders[0] + "/";

		QString filePath = fullImageDir;
		filePath += QString("fullImage_%1_%2_%3").arg(sz.width).arg(sz.height).arg(defectNum);
		filePath += userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), dst);
		detectFunc->generateBigTempl();
		defectNum = 0;//������ȱ����0
>>>>>>> wenyuexin
	}

	//检测结�?
	double t2 = clock();
<<<<<<< HEAD
	qDebug() << "" << pcb::chinese("current row finish") << 
		(t2 - t1) << "ms  ( currentRow_detect -" << runtimeParams->currentRow_detect << ")";
=======
	qDebug() << ">>>>>>>>>> " << pcb::chinese("��ǰ�м�����") << 
		(t2 - t1) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;
>>>>>>> wenyuexin
	
	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);
	pcb::delay(10);
	bool qualified = (defectNum < 100);

	if (currentRow_detect == nPhotographing-1)
		emit detectFinished_detectThread(qualified);
}

<<<<<<< HEAD
//判断与产品序号对应的输出文件夹是否存在，若不存在则创�?
void DefectDetecter::makeCurrentOutputDir()
=======
//�ж����Ʒ��Ŷ�Ӧ������ļ����Ƿ���ڣ����������򴴽�
void DefectDetecter::makeCurrentOutputDir(vector<QString> &subFolders)
>>>>>>> wenyuexin
{
	//判断顶层的output文件夹是否存�?
	runtimeParams->currentOutputDir = userConfig->OutputDirPath; 
	QDir outputDir(runtimeParams->currentOutputDir);
	if (!outputDir.exists()) outputDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的型号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleModelNum;
	QDir modelDir(runtimeParams->currentOutputDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的批次号文件夹是否存�?
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleBatchNum;
	QDir batchDir(runtimeParams->currentOutputDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentOutputDir);

	//判断对应的样本编号文件夹是否存在
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleNum;
	QDir resultDir(runtimeParams->currentOutputDir);
	if (!resultDir.exists()) {
		//创建文件�?
		resultDir.mkdir(runtimeParams->currentOutputDir);
	}
	else {
		//清空文件�?
		pcb::clearFolder(runtimeParams->currentOutputDir, false);
	}

	//�������ļ���
	for (int i = 0; i < subFolders.size(); i++) {
		resultDir.mkdir(subFolders[i]);//�������ļ���
	}
}
