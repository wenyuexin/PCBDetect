#include "TemplateExtractor.h"

using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::CvMatVector;
using pcb::CvMatArray;
using std::vector;
using std::string;
using cv::Mat;
using cv::Point;
using cv::Point2i;
using cv::Point2f;
using cv::Vec4i;
using cv::Rect;
using cv::Size;


TemplateExtractor::TemplateExtractor(QObject *parent)
	: QObject(parent)
{
	adminConfig = Q_NULLPTR;
	detectConfig = Q_NULLPTR;
	detectParams = Q_NULLPTR;
	cvmatSamples = Q_NULLPTR;
	templFunc = Q_NULLPTR;
	extractState = InitialState;
}

TemplateExtractor::~TemplateExtractor()
{
	delete templFunc;
}

//初始化templFunc
void TemplateExtractor::initTemplFunc()
{
	templFunc = new TemplFunc;
	templFunc->setAdminConfig(adminConfig);
	templFunc->setDetectConfig(detectConfig);
	templFunc->setDetectParams(detectParams);
	templFunc->generateBigTempl();
}

/******************** 提取 **********************/

void TemplateExtractor::extract()
{
	extractState = ExtractState::Start;
	emit extractState_extractor(extractState);

	//pcb::delay(1000); //提取

	string curr_path = detectConfig->TemplDirPath.toStdString() + "/";
	string templ_path = curr_path + detectParams->sampleModelNum.toStdString();//检查模板文件夹中mask文件是否存在
	if (0 != _access(templ_path.c_str(), 0))
		_mkdir(templ_path.c_str());
	string	mask_path = templ_path + "/mask";
	if (0 != _access(mask_path.c_str(), 0))
		_mkdir(mask_path.c_str());

	for (int col = 0; col < detectParams->nCamera; col++) {
		string temp = templ_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" 
			+ std::to_string(col + 1)  + detectConfig->ImageFormat.toUpper().toStdString();
		Mat src = *((*cvmatSamples)[detectParams->currentRow_extract][col]);
		cv::imwrite(temp, src);//保存模板图片
		Mat mask = templFunc->find1(col, src);

		cv::imwrite(mask_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" 
			+ std::to_string(col + 1) + "_mask" + detectConfig->ImageFormat.toStdString(), mask);
		Rect roiRect = Rect(col*adminConfig->ImageSize_W, detectParams->currentRow_extract*adminConfig->ImageSize_H,
			adminConfig->ImageSize_W, adminConfig->ImageSize_H);
		Mat roi = templFunc->getBigTempl(roiRect);
		src.copyTo(roi);
	}

	if (detectParams->currentRow_extract + 1 == detectParams->nPhotographing) {
		cv::Size sz = templFunc->getBigTempl().size();
		cv::Mat dst;
		cv::resize(templFunc->getBigTempl(), dst, cv::Size(sz.width*0.25,sz.height*0.25), (0, 0), (0, 0), cv::INTER_LINEAR);
		cv::imwrite(templ_path + "/" + "fullImage_"+std::to_string(sz.width)+"_"+std::to_string(sz.height)+".jpg", dst);
	}


	extractState = ExtractState::Finished;
	emit extractState_extractor(extractState);
}