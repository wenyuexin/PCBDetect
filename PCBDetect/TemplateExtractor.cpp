#include "TemplateExtractor.h"

using pcb::UserConfig;
using pcb::RuntimeParams;
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
	userConfig = Q_NULLPTR;
	runtimeParams = Q_NULLPTR;
	cvmatSamples = Q_NULLPTR;
	templFunc = Q_NULLPTR;
	extractState = InitialState;
}

void TemplateExtractor::init()
{
	initExtractFunc();

	//分图图像的原始尺寸
	originalSubImageSize = Size(adminConfig->ImageSize_W, adminConfig->ImageSize_H); 
	
	//整图的原始尺寸
	originalFullImageSize = Size(adminConfig->ImageSize_W * runtimeParams->nCamera,
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); 

	//缩放因子
	double factorW = 2.0 * runtimeParams->ScreenRect.width() / originalFullImageSize.width;
	double factorH = 2.0 * runtimeParams->ScreenRect.height() / originalFullImageSize.height;
	scalingFactor = qMin(factorW, factorH);

	//分图经过缩放后的尺寸
	scaledSubImageSize = Size(scalingFactor * originalSubImageSize.width,
		scalingFactor * originalSubImageSize.height); 

	//整图经过缩放后的尺寸
	scaledFullImageSize = Size(scaledSubImageSize.width * runtimeParams->nCamera,
		scaledSubImageSize.height * runtimeParams->nPhotographing);

}


TemplateExtractor::~TemplateExtractor()
{
	qDebug() << "~TemplateExtractor";
	delete templFunc; templFunc = Q_NULLPTR;
}

//初始化extractFunc
void TemplateExtractor::initExtractFunc()
{
	delete templFunc;
	templFunc = new ExtractFunc;
	templFunc->setAdminConfig(adminConfig);
	templFunc->setUserConfig(userConfig);
	templFunc->setRuntimeParams(runtimeParams);
	templFunc->generateBigTempl();
}


/******************** 提取 **********************/

void TemplateExtractor::extract()
{
	//开始提取
	extractState = ExtractState::Start;
	emit extractState_extractor(extractState);

	int currentRow_extract = runtimeParams->currentRow_extract;
	int nCamera = runtimeParams->nCamera;
	int nPhotographing = runtimeParams->nPhotographing;

	//创建文件夹
	vector<QString> subFolders { "mask", "bw", "bin", "subtempl" };
	if (currentRow_extract == 0) makeCurrentTemplDir(subFolders);

	QString templ_path = runtimeParams->currentTemplDir + "/";
	QString mask_path = templ_path + subFolders[0] + "/";//掩膜路径
	QString bw_path = templ_path + subFolders[1] + "/";//模板二值图路径
	QString bin_path = templ_path + subFolders[2] + "/";//模板特征路径
	QString subtempl_path = templ_path + subFolders[3] + "/";//模板路径

	QString filePath; //文件路径
	QString fileName; //文件名
	for (int col = 0; col < nCamera; col++) {
		Mat src = *((*cvmatSamples)[currentRow_extract][col]); //样本图
		fileName = QString("%1_%2").arg(currentRow_extract + 1).arg(col + 1);

		//保存模版图片
		filePath = subtempl_path + fileName + userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), src);//保存模板图片
		
		//保存掩膜图片
		Mat mask = templFunc->findLocationMark(col, src);
		filePath = mask_path + fileName + "_mask" + userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), mask);

		//保存二值化图片
		Mat srcGray, templbw;
		cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);
		cv::adaptiveThreshold(srcGray, templbw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		Mat element_b = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
		cv::morphologyEx(templbw, templbw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templbw, templbw, cv::MORPH_CLOSE, element_b);

		filePath = bw_path + fileName + "_bw" + userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), templbw);

		//提取模板特征并保存
		filePath = bin_path + fileName + ".bin";
		templFunc->save(filePath.toStdString(), src);

		//合成大模板
		Rect roiRect = Rect(col*adminConfig->ImageSize_W, currentRow_extract*adminConfig->ImageSize_H,
			adminConfig->ImageSize_W, adminConfig->ImageSize_H);
		Mat roi = templFunc->getBigTempl(roiRect);
		src.copyTo(roi);
	}

	//存储PCB整图
	if (runtimeParams->currentRow_extract + 1 == nPhotographing) {
		Mat scaledFullImage;
		cv::resize(templFunc->getBigTempl(), scaledFullImage, scaledFullImageSize); //缩放

		filePath = templ_path;
		filePath += QString("fullImage_%1_%2").arg(originalFullImageSize.width).arg(originalFullImageSize.height);
		filePath += userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), scaledFullImage);
	}

	//提取结束
	extractState = ExtractState::Finished;
	emit extractState_extractor(extractState);
}


//判断与产品序号对应的输出文件夹是否存在，若不存在则创建
void TemplateExtractor::makeCurrentTemplDir(vector<QString> &subFolders)
{
	//判断顶层的template文件夹是否存在
	runtimeParams->currentTemplDir = userConfig->TemplDirPath;
	QDir templateDir(runtimeParams->currentTemplDir);
	if (!templateDir.exists()) templateDir.mkdir(runtimeParams->currentTemplDir);

	//判断对应的型号文件夹是否存在
	runtimeParams->currentTemplDir += "/" + runtimeParams->sampleModelNum;
	QDir templDir(runtimeParams->currentTemplDir);
	if (!templDir.exists()) {
		templDir.mkdir(runtimeParams->currentTemplDir);//创建文件夹
	}
	else {
		pcb::clearFolder(runtimeParams->currentTemplDir, false);//清空文件夹
	}
	
	//添加子文件夹
	for (int i = 0; i < subFolders.size(); i++) {
		templDir.mkdir(subFolders[i]);//创建子文件夹
	}
}