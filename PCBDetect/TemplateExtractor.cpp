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
	delete templFunc; templFunc = Q_NULLPTR;
}

//��ʼ��templFunc
void TemplateExtractor::initTemplFunc()
{
	templFunc = new TemplFunc;
	templFunc->setAdminConfig(adminConfig);
	templFunc->setDetectConfig(detectConfig);
	templFunc->setDetectParams(detectParams);
	templFunc->generateBigTempl();
}

/******************** ��ȡ **********************/

void TemplateExtractor::extract()
{
	extractState = ExtractState::Start;
	emit extractState_extractor(extractState);

	pcb::delay(1500); //��ȡ

	string curr_path = detectConfig->TemplDirPath.toStdString() + "/";
	string templ_path = curr_path + detectParams->sampleModelNum.toStdString();//���ģ���ļ�����mask�ļ��Ƿ����
	if (0 != _access(templ_path.c_str(), 0))
		_mkdir(templ_path.c_str());
	string	mask_path = templ_path + "/mask";//��Ĥ·��
	if (0 != _access(mask_path.c_str(), 0))
		_mkdir(mask_path.c_str());
	string templbw_path = templ_path + "/bw";//ģ���ֵͼ·��
	if (0 != _access(templbw_path.c_str(), 0))
		_mkdir(templbw_path.c_str());
	string templbin_path = templ_path + "/bin";//ģ������·��
	if (0 != _access(templbin_path.c_str(), 0))
		_mkdir(templbin_path.c_str());
	string subtempl_path = templ_path + "/subtempl";//ģ��·��
	if (0 != _access(subtempl_path.c_str(), 0))
		_mkdir(subtempl_path.c_str());

	for (int col = 0; col < detectParams->nCamera; col++) {
		Mat src = *((*cvmatSamples)[detectParams->currentRow_extract][col]);

		//����ģ��ͼƬ
		cv::imwrite(subtempl_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" + std::to_string(col + 1) + detectConfig->ImageFormat.toStdString(), src);//����ģ��ͼƬ
		
		//������ĤͼƬ
		Mat mask = templFunc->find1(col, src);
		cv::imwrite(mask_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" 
			+ std::to_string(col + 1) + "_mask" + detectConfig->ImageFormat.toStdString(), mask);

		//�����ֵ��ͼƬ
		Mat srcGray, templbw;
		cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);
		cv::adaptiveThreshold(srcGray, templbw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		cv::Mat element_b = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		cv::morphologyEx(templbw, templbw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templbw, templbw, cv::MORPH_CLOSE, element_b);
		cv::imwrite(templbw_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" + std::to_string(col + 1) + "_bw" + detectConfig->ImageFormat.toStdString(), templbw);

		//��ȡģ������������
		templFunc->save(templbin_path + "/" + std::to_string(detectParams->currentRow_extract + 1) + "_" + std::to_string(col + 1) + ".bin", src);


		//�ϳɴ�ģ��
		Rect roiRect = Rect(col*adminConfig->ImageSize_W, detectParams->currentRow_extract*adminConfig->ImageSize_H,
			adminConfig->ImageSize_W, adminConfig->ImageSize_H);
		Mat roi = templFunc->getBigTempl(roiRect);
		src.copyTo(roi);
	}

	if (detectParams->currentRow_extract+ 1 == detectParams->nPhotographing) {
		cv::Size sz = templFunc->getBigTempl().size();
		cv::Mat dst;
		cv::resize(templFunc->getBigTempl(), dst, cv::Size(sz.width*0.25,sz.height*0.25), (0, 0), (0, 0), cv::INTER_LINEAR);
		cv::imwrite(templ_path + "/" + "fullImage_"+std::to_string(sz.width)+"_"+std::to_string(sz.height)+".jpg", dst);
	}


	extractState = ExtractState::Finished;
	emit extractState_extractor(extractState);
}