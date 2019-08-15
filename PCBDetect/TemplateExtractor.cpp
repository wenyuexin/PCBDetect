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

	//��ͼͼ���ԭʼ�ߴ�
	originalSubImageSize = Size(adminConfig->ImageSize_W, adminConfig->ImageSize_H); 
	
	//��ͼ��ԭʼ�ߴ�
	originalFullImageSize = Size(adminConfig->ImageSize_W * runtimeParams->nCamera,
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); 

	//��������
	double factorW = 2.0 * runtimeParams->ScreenRect.width() / originalFullImageSize.width;
	double factorH = 2.0 * runtimeParams->ScreenRect.height() / originalFullImageSize.height;
	scalingFactor = qMin(factorW, factorH);

	//��ͼ�������ź�ĳߴ�
	scaledSubImageSize = Size(scalingFactor * originalSubImageSize.width,
		scalingFactor * originalSubImageSize.height); 

	//��ͼ�������ź�ĳߴ�
	scaledFullImageSize = Size(scaledSubImageSize.width * runtimeParams->nCamera,
		scaledSubImageSize.height * runtimeParams->nPhotographing);
}


TemplateExtractor::~TemplateExtractor()
{
	qDebug() << "~TemplateExtractor";
	delete templFunc; templFunc = Q_NULLPTR;
}

//��ʼ��extractFunc
void TemplateExtractor::initExtractFunc()
{
	delete templFunc;
	templFunc = new ExtractFunc;
	templFunc->setAdminConfig(adminConfig);
	templFunc->setUserConfig(userConfig);
	templFunc->setRuntimeParams(runtimeParams);
	templFunc->generateBigTempl();
}


/******************** ��ȡ **********************/

void TemplateExtractor::extract()
{
	qDebug() << "====================" << pcb::chinese("��ʼ��ȡ") <<
		"( currentRow_extract =" << runtimeParams->currentRow_extract << ")" << endl;
	double t1 = clock();

	//��ʼ��ȡ
	extractState = ExtractState::Start;
	emit extractState_extractor(extractState);

	int currentRow_extract = runtimeParams->currentRow_extract;
	int nCamera = runtimeParams->nCamera;
	int nPhotographing = runtimeParams->nPhotographing;

	//�����ļ���
	vector<QString> subFolders { "mask", "bw", "bin", "subtempl" };
	if (currentRow_extract == 0) makeCurrentTemplDir(subFolders);

	QString templ_path = runtimeParams->currentTemplDir + "/";
	QString mask_path = templ_path + subFolders[0] + "/";//��Ĥ·��
	QString bw_path = templ_path + subFolders[1] + "/";//ģ���ֵͼ·��
	QString bin_path = templ_path + subFolders[2] + "/";//ģ������·��
	QString subtempl_path = templ_path + subFolders[3] + "/";//ģ��·��

	QString filePath; //�ļ�·��
	QString fileName; //�ļ���
	for (int col = 0; col < nCamera; col++) {
		Mat src = *((*cvmatSamples)[currentRow_extract][col]); //����ͼ
		fileName = QString("%1_%2").arg(currentRow_extract + 1).arg(col + 1);

		//����ģ��ͼƬ
		filePath = subtempl_path + fileName + ".bmp";
		cv::imwrite(filePath.toStdString(), src);//����ģ��ͼƬ
		
		//������ĤͼƬ
		//Mat mask = templFunc->findLocationMark(col, src);
		//filePath = mask_path + fileName + "_mask" + userConfig->ImageFormat;
		//cv::imwrite(filePath.toStdString(), mask);

		//�����ֵ��ͼƬ
		Mat srcGray, templbw;
		cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);
		cv::adaptiveThreshold(srcGray, templbw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		Mat element_b = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
		cv::morphologyEx(templbw, templbw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templbw, templbw, cv::MORPH_CLOSE, element_b);

		filePath = bw_path + fileName + "_bw" + ".bmp";
		cv::imwrite(filePath.toStdString(), templbw);

		////��ȡģ������������
		//filePath = bin_path + fileName + ".bin";
		//templFunc->save(filePath.toStdString(), src);



		//�ϳɴ�ģ��
		Point roiPos(col*adminConfig->ImageSize_W, currentRow_extract*adminConfig->ImageSize_H);
		Rect roiRect = Rect(roiPos, originalSubImageSize);
		Mat roiImage = templFunc->getBigTempl(roiRect);
		src.copyTo(roiImage);
	}

	//����mask,�洢PCB��ͼ
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
		//br_x = int(lf_x + userConfig->ActualProductSize_W*adminConfig->PixelsNumPerUnitLength + int(adminConfig->ImageOverlappingRate_W * adminConfig->ImageSize_W) * (runtimeParams->nCamera - 1)) % adminConfig->ImageSize_W + 160;//pcb�����Ͻ���ͼ���е�λ��,160Ϊƫ����
		//br_y = adminConfig->ImageSize_H - (int(adminConfig->ImageSize_H - lf_y + userConfig->ActualProductSize_H*adminConfig->PixelsNumPerUnitLength + int(adminConfig->ImageOverlappingRate_H * adminConfig->ImageSize_H)* (runtimeParams->nPhotographing - 1)) % adminConfig->ImageSize_H);
		//Mat image_lf = cv::imread(subtempl_path.toStdString() + "/" + std::to_string(runtimeParams->nPhotographing) + "_1" + userConfig->ImageFormat.toStdString());
		//point_left = templFunc->corner_lf(image_lf, lf_x, lf_y);
		//Mat image_br = cv::imread(subtempl_path.toStdString() + "/" + std::to_string(1) + "_" + std::to_string(runtimeParams->nCamera) + userConfig->ImageFormat.toStdString());
		//point_right = templFunc->corner_br(image_br, br_x, br_y);

		//��ȡ��ģ
		Mat image;//n1,n2�ļ��е�ͼ��
		int num_cols = 0, num_rows = 0, num = runtimeParams->nCamera * runtimeParams->nPhotographing;

		std::vector<cv::String> image_name(num);//�������ͼ����

		cv::String path = subtempl_path.toStdString();
		int rowLen_path = path.length() + 1, colLen_path = rowLen_path + 2;
		vector<cv::String> filename;//����һ�����飬����ļ�������ͼ����
		cv::glob(path, filename);//��path·���µ������ļ�������filename

		for (num = 0; num < filename.size(); num++)
		{

			//image_name[num] = filename[num];
			//templFunc->str2int(num_rows, image_name[num].substr(rowLen_path, 1));//ͼ������
			//templFunc->str2int(num_cols, image_name[num].substr(colLen_path, 1));//ͼ������

			//std::stringstream stream1(image_name[num].substr(rowLen_path, 1));
			//stream1 >> num_rows;
			//std::stringstream stream2(image_name[num].substr(colLen_path, 1));
			//stream2 >> num_cols;
			num_rows = num / runtimeParams->nCamera + 1;
			num_cols = num % runtimeParams->nCamera + 1;

			//str2int(num_rows, image_name1[num].substr(47, 1));//ͼ������
			//str2int(num_cols, image_name1[num].substr(49, 1));//ͼ������

			//stringstream stream1(image_name1[num].substr(47, 1));
			//stream1 >> num_rows;

			image = cv::imread(filename[num]);//��ͼ��
			Mat mask = templFunc->cutting(num_cols, num_rows, runtimeParams->nCamera, runtimeParams->nPhotographing, image, point_bl, point_tr);//������
			string resPath = mask_path.toStdString() + "/" + std::to_string(num_rows) + "_" + std::to_string(num_cols) + "_mask" + ".bmp";
			/*	imwrite(mask_path + "/2.jpg", mask);*/
			cv::imwrite(resPath, mask);


			//��ȡģ������������
			Mat src_mask;
			cv::bitwise_and(image, mask, src_mask);
			string file_Path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString()
				+ "/bin/" + std::to_string(num_rows) + "_" + std::to_string(num_cols) + ".bin";
			templFunc->save(file_Path, src_mask);
		}

		Mat scaledFullImage;
		cv::resize(templFunc->getBigTempl(), scaledFullImage, scaledFullImageSize); //����

		filePath = templ_path;
		filePath += QString("fullImage_%1_%2").arg(originalFullImageSize.width).arg(originalFullImageSize.height);
		filePath += userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), scaledFullImage);
	}

	//��ȡ����
	double t2 = clock();
	qDebug() << "====================" << pcb::chinese("��ǰ����ȡ������") <<
		(t2 - t1) << "ms  ( currentRow_extract =" << currentRow_extract << ")" << endl;

	extractState = ExtractState::Finished;
	emit extractState_extractor(extractState);
}


//�ж����Ʒ��Ŷ�Ӧ������ļ����Ƿ���ڣ����������򴴽�
void TemplateExtractor::makeCurrentTemplDir(vector<QString> &subFolders)
{
	//�ж϶����template�ļ����Ƿ����
	runtimeParams->currentTemplDir = userConfig->TemplDirPath;
	QDir templateDir(runtimeParams->currentTemplDir);
	if (!templateDir.exists()) templateDir.mkdir(runtimeParams->currentTemplDir);

	//�ж϶�Ӧ���ͺ��ļ����Ƿ����
	runtimeParams->currentTemplDir += "/" + runtimeParams->sampleModelNum;
	QDir templDir(runtimeParams->currentTemplDir);
	if (!templDir.exists()) {
		templDir.mkdir(runtimeParams->currentTemplDir);//�����ļ���
	}
	else {
		pcb::clearFolder(runtimeParams->currentTemplDir, false);//����ļ���
	}
	
	//������ļ���
	for (int i = 0; i < subFolders.size(); i++) {
		templDir.mkdir(subFolders[i]);//�������ļ���
	}
}