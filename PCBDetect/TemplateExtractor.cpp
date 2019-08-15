#include "TemplateExtractor.h"
#include "ExtractFunc.h"
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"


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
	qDebug() << "====================" << pcb::chinese("开始提取") <<
		"( currentRow_extract =" << runtimeParams->currentRow_extract << ")" << endl;
	double t1 = clock();

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
		filePath = subtempl_path + fileName + ".bmp";
		cv::imwrite(filePath.toStdString(), src);//保存模板图片
		
		//保存掩膜图片
		//Mat mask = templFunc->findLocationMark(col, src);
		//filePath = mask_path + fileName + "_mask" + userConfig->ImageFormat;
		//cv::imwrite(filePath.toStdString(), mask);

		//保存二值化图片
		Mat srcGray, templbw;
		cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);
		cv::adaptiveThreshold(srcGray, templbw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		Mat element_b = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
		cv::morphologyEx(templbw, templbw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templbw, templbw, cv::MORPH_CLOSE, element_b);

		filePath = bw_path + fileName + "_bw" + ".bmp";
		cv::imwrite(filePath.toStdString(), templbw);

		////提取模板特征并保存
		//filePath = bin_path + fileName + ".bin";
		//templFunc->save(filePath.toStdString(), src);



		//合成大模板
		Point roiPos(col*adminConfig->ImageSize_W, currentRow_extract*adminConfig->ImageSize_H);
		Rect roiRect = Rect(roiPos, originalSubImageSize);
		Mat roiImage = templFunc->getBigTempl(roiRect);
		src.copyTo(roiImage);
	}

	//生成mask,存储PCB整图
	if (runtimeParams->currentRow_extract + 1 == nPhotographing) {

		
		Point point_bl(runtimeParams->maskRoi_tl.x(), runtimeParams->maskRoi_br.y());
		Point point_tr(runtimeParams->maskRoi_br.x(), runtimeParams->maskRoi_tl.y());

		string pointsPath = mask_path.toStdString() + "cornerPoints.bin";
		vector<Point2i> cornerPoints{ point_bl,point_tr };
		cv::FileStorage store(pointsPath, cv::FileStorage::WRITE);
		cv::write(store, "cornerPoints", cornerPoints);
		store.release();

		//cv::FileStorage store_new(pointsPath, cv::FileStorage::READ);
		//cv::FileNode n1 = store_new["cornerPoints"];
		//vector<Point2i>  res;
		//cv::read(n1, res);
	
		//int lf_x = 175, lf_y = 3285, br_x = 0, br_y = 0;/* lf_x = 168, lf_y = 3276,*//* br_x = 3043, br_y = 325*/
		//br_x = int(lf_x + userConfig->ActualProductSize_W*adminConfig->PixelsNumPerUnitLength + int(adminConfig->ImageOverlappingRate_W * adminConfig->ImageSize_W) * (runtimeParams->nCamera - 1)) % adminConfig->ImageSize_W + 160;//pcb板右上角在图像中的位置,160为偏移量
		//br_y = adminConfig->ImageSize_H - (int(adminConfig->ImageSize_H - lf_y + userConfig->ActualProductSize_H*adminConfig->PixelsNumPerUnitLength + int(adminConfig->ImageOverlappingRate_H * adminConfig->ImageSize_H)* (runtimeParams->nPhotographing - 1)) % adminConfig->ImageSize_H);
		//Mat image_lf = cv::imread(subtempl_path.toStdString() + "/" + std::to_string(runtimeParams->nPhotographing) + "_1" + userConfig->ImageFormat.toStdString());
		//point_left = templFunc->corner_lf(image_lf, lf_x, lf_y);
		//Mat image_br = cv::imread(subtempl_path.toStdString() + "/" + std::to_string(1) + "_" + std::to_string(runtimeParams->nCamera) + userConfig->ImageFormat.toStdString());
		//point_right = templFunc->corner_br(image_br, br_x, br_y);

		//提取掩模
		Mat image;//n1,n2文件中的图像
		int num_cols = 0, num_rows = 0, num = runtimeParams->nCamera * runtimeParams->nPhotographing;

		std::vector<cv::String> image_name(num);//用来存放图像名

		cv::String path = subtempl_path.toStdString();
		int rowLen_path = path.length() + 1, colLen_path = rowLen_path + 2;
		vector<cv::String> filename;//创建一个数组，存放文件名，即图像名
		cv::glob(path, filename);//将path路径下的所用文件名放在filename

		for (num = 0; num < filename.size(); num++)
		{

			//image_name[num] = filename[num];
			//templFunc->str2int(num_rows, image_name[num].substr(rowLen_path, 1));//图像行数
			//templFunc->str2int(num_cols, image_name[num].substr(colLen_path, 1));//图像列数

			//std::stringstream stream1(image_name[num].substr(rowLen_path, 1));
			//stream1 >> num_rows;
			//std::stringstream stream2(image_name[num].substr(colLen_path, 1));
			//stream2 >> num_cols;
			num_rows = num / runtimeParams->nCamera + 1;
			num_cols = num % runtimeParams->nCamera + 1;

			//str2int(num_rows, image_name1[num].substr(47, 1));//图像行数
			//str2int(num_cols, image_name1[num].substr(49, 1));//图像列数

			//stringstream stream1(image_name1[num].substr(47, 1));
			//stream1 >> num_rows;

			image = cv::imread(filename[num]);//读图像
			Mat mask = templFunc->cutting(num_cols, num_rows, runtimeParams->nCamera, runtimeParams->nPhotographing, image, point_bl, point_tr);//调函数
			string resPath = mask_path.toStdString() + "/" + std::to_string(num_rows) + "_" + std::to_string(num_cols) + "_mask" + ".bmp";
			/*	imwrite(mask_path + "/2.jpg", mask);*/
			cv::imwrite(resPath, mask);


			//提取模板特征并保存
			Mat src_mask;
			cv::bitwise_and(image, mask, src_mask);
			string file_Path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString()
				+ "/bin/" + std::to_string(num_rows) + "_" + std::to_string(num_cols) + ".bin";
			templFunc->save(file_Path, src_mask);
		}

		Mat scaledFullImage;
		cv::resize(templFunc->getBigTempl(), scaledFullImage, scaledFullImageSize); //缩放

		filePath = templ_path;
		filePath += QString("fullImage_%1_%2").arg(originalFullImageSize.width).arg(originalFullImageSize.height);
		filePath += userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), scaledFullImage);
	}

	//提取结束
	double t2 = clock();
	qDebug() << "====================" << pcb::chinese("当前行提取结束：") <<
		(t2 - t1) << "ms  ( currentRow_extract =" << currentRow_extract << ")" << endl;

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