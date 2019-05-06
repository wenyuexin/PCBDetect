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
	qDebug() << "~TemplateExtractor";
	delete templFunc;
	templFunc = Q_NULLPTR;
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

	pcb::delay(1500); //提取

	string curr_path = detectConfig->TemplDirPath.toStdString() + "/";
	string templ_path = curr_path + detectParams->sampleModelNum.toStdString();//检查模板文件夹中mask文件是否存在
	if (0 != _access(templ_path.c_str(), 0))
		_mkdir(templ_path.c_str());
	string	mask_path = templ_path + "/mask";//掩膜路径
	if (0 != _access(mask_path.c_str(), 0))
		_mkdir(mask_path.c_str());
	string templbw_path = templ_path + "/bw";//模板二值图路径
	if (0 != _access(templbw_path.c_str(), 0))
		_mkdir(templbw_path.c_str());
	string templbin_path = templ_path + "/bin";//模板特征路径
	if (0 != _access(templbin_path.c_str(), 0))
		_mkdir(templbin_path.c_str());
	string subtempl_path = templ_path + "/subtempl";//模板路径
	if (0 != _access(subtempl_path.c_str(), 0))
		_mkdir(subtempl_path.c_str());



	for (int col = 0; col < detectParams->nCamera; col++) {
		Mat src = *((*cvmatSamples)[detectParams->currentRow_extract][col]);

		//保存模版图片
		cv::imwrite(subtempl_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" + std::to_string(col + 1) + detectConfig->ImageFormat.toStdString(), src);//保存模板图片
		

		//保存二值化图片
		Mat srcGray, templbw;
		cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);
		cv::adaptiveThreshold(srcGray, templbw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		cv::Mat element_b = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		cv::morphologyEx(templbw, templbw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templbw, templbw, cv::MORPH_CLOSE, element_b);
		cv::imwrite(templbw_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" + std::to_string(col + 1) + "_bw" + detectConfig->ImageFormat.toStdString(), templbw);

		//提取模板特征并保存
		templFunc->save(templbin_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" + std::to_string(col + 1) + ".bin", src);


		//合成大模板
		Rect roiRect = Rect(col*adminConfig->ImageSize_W, detectParams->currentRow_extract*adminConfig->ImageSize_H,
			adminConfig->ImageSize_W, adminConfig->ImageSize_H);
		Mat roi = templFunc->getBigTempl(roiRect);
		src.copyTo(roi);
	}

	if (detectParams->currentRow_extract+ 1 == detectParams->nPhotographing) {


		Point point_left, point_right;
		int lf_x = 175, lf_y = 3285, br_x = 0, br_y = 0;/* lf_x = 168, lf_y = 3276,*//* br_x = 3043, br_y = 325*/
		br_x = (lf_x + detectConfig->ActualProductSize_W*adminConfig->PixelsNumPerUnitLength + int(adminConfig->ImageOverlappingRate_W * adminConfig->ImageSize_W) * (detectParams->nCamera - 1)) % adminConfig->ImageSize_W + 160;//pcb板右上角在图像中的位置,160为偏移量
		br_y = adminConfig->ImageSize_H - ((adminConfig->ImageSize_H - lf_y + detectConfig->ActualProductSize_H*adminConfig->PixelsNumPerUnitLength + int(adminConfig->ImageOverlappingRate_H * adminConfig->ImageSize_H)* (detectParams->nPhotographing - 1)) % adminConfig->ImageSize_H);
		Mat image_lf = cv::imread(subtempl_path + "/" + std::to_string(detectParams->nPhotographing) + "_1" + detectConfig->ImageFormat.toStdString());
		point_left = templFunc->corner_lf(image_lf, lf_x, lf_y);
		Mat image_br = cv::imread(subtempl_path + "/" + std::to_string(1) + "_" + std::to_string(detectParams->nCamera) + detectConfig->ImageFormat.toStdString());
		point_right = templFunc->corner_br(image_br, br_x, br_y);

		Mat image;//n1,n2文件中的图像
		int num_cols = 0, num_rows = 0, num = detectParams->nCamera * detectParams->nPhotographing;

		std::vector<cv::String> image_name(num);//用来存放图像名

		cv::String path = subtempl_path;
		int rowLen_path = path.length() + 1, colLen_path = rowLen_path + 2;
		vector<cv::String> filename;//创建一个数组，存放文件名，即图像名
		cv::glob(path, filename);//将path路径下的所用文件名放在filename

		for (num = 0; num < filename.size(); num++)
		{


			image_name[num] = filename[num];
			templFunc->str2int(num_rows, image_name[num].substr(rowLen_path, 1));//图像行数
			templFunc->str2int(num_cols, image_name[num].substr(colLen_path, 1));//图像列数

			std::stringstream stream1(image_name[num].substr(rowLen_path, 1));
			stream1 >> num_rows;
			std::stringstream stream2(image_name[num].substr(colLen_path, 1));
			stream2 >> num_cols;

			//str2int(num_rows, image_name1[num].substr(47, 1));//图像行数
			//str2int(num_cols, image_name1[num].substr(49, 1));//图像列数

			//stringstream stream1(image_name1[num].substr(47, 1));
			//stream1 >> num_rows;


			image = cv::imread(filename[num]);//读图像
			Mat mask = templFunc->cutting(num_cols, num_rows, detectParams->nCamera, detectParams->nPhotographing, image, point_left, point_right);//调函数
			string resPath = mask_path + "/" + std::to_string(num_rows) + "_" + std::to_string(num_cols) + "_mask" + detectConfig->ImageFormat.toStdString();
			/*	imwrite(mask_path + "/2.jpg", mask);*/
			cv::imwrite(resPath, mask);

		}

		cv::Size sz = templFunc->getBigTempl().size();
		cv::Mat dst;
		cv::resize(templFunc->getBigTempl(), dst, cv::Size(sz.width*0.25,sz.height*0.25), (0, 0), (0, 0), cv::INTER_LINEAR);
		cv::imwrite(templ_path + "/" + "fullImage_"+std::to_string(sz.width)+"_"+std::to_string(sz.height)+".jpg", dst);
	}


	extractState = ExtractState::Finished;
	emit extractState_extractor(extractState);
}