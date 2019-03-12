#include "TemplateExtractor.h"


using Ui::DetectConfig;
using Ui::DetectParams;
using Ui::CvMatVector;
using Ui::CvMatArray;

using std::vector;
using std::string;
using cv::Mat;
using cv::imread;


TemplateExtractor::TemplateExtractor(QObject *parent)
	: QObject(parent)
{
	extractState = -1;
}

TemplateExtractor::~TemplateExtractor()
{
}


/******************** 配置 *********************/

void TemplateExtractor::setDetectConfig(DetectConfig *ptr) { config = ptr; }

void TemplateExtractor::setDetectParams(DetectParams *ptr) { params = ptr; }

void TemplateExtractor::setSampleImages(CvMatArray *ptr) { samples = ptr; }


/******************** 提取 **********************/

void TemplateExtractor::extract(TemplFunc *templFunc)
{
	extractState = 0;
	emit sig_extractState_extractor(extractState);

	//Ui::delay(1000); //提取

	(*templFunc).rows = config->nPhotographing;
	(*templFunc).cols = config->nCamera;




	string curr_path = config->TemplDirPath.toStdString() + "/";
	string templ_path = curr_path + params->sampleModelNum.toStdString();//检查模板文件夹中mask文件是否存在
	if (0 != _access(templ_path.c_str(), 0))
		_mkdir(templ_path.c_str());
	string	mask_path = templ_path + "/mask";
	if (0 != _access(mask_path.c_str(), 0))
		_mkdir(mask_path.c_str());


	for (int col = 0; col < (*samples)[0].size(); col++) {
		string temp = templ_path + "/" + std::to_string(params->currentRow_extract + 1) + "_" + std::to_string(col + 1)  + config->ImageFormat.toUpper().toStdString();
		Mat src = (*samples)[params->currentRow_extract][col];
		cv::imwrite(temp, src);//保存模板图片
		Mat mask = (*templFunc).find1(col + 1, params->currentRow_extract + 1, src);

		cv::imwrite(mask_path + "/" + std::to_string(params->currentRow_extract + 1) + "_" + std::to_string(col + 1) + "_mask" + config->ImageFormat.toStdString(), mask);
		Mat roi = templFunc->big_templ(cv::Rect(col*config->imageSize.width(), params->currentRow_extract*config->imageSize.height(), config->imageSize.width(), config->imageSize.height()));
		src.copyTo(roi);
	}

	if (params->currentRow_extract + 1 == config->nPhotographing) {
		cv::Size sz = templFunc->big_templ.size();
		cv::Mat dst;
		cv::resize(templFunc->big_templ, dst, cv::Size(sz.width*0.25,sz.height*0.25), (0, 0), (0, 0), cv::INTER_LINEAR);
		cv::imwrite(templ_path + "/" + "fullImage_"+std::to_string(sz.width)+"_"+std::to_string(sz.height)+".jpg", dst);
	}


	extractState = 1;
	emit sig_extractState_extractor(extractState);
}