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
		filePath = subtempl_path + fileName + userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), src);//����ģ��ͼƬ
		
		//������ĤͼƬ
		Mat mask = templFunc->findLocationMark(col, src);
		filePath = mask_path + fileName + "_mask" + userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), mask);

		//�����ֵ��ͼƬ
		Mat srcGray, templbw;
		cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);
		cv::adaptiveThreshold(srcGray, templbw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		Mat element_b = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
		cv::morphologyEx(templbw, templbw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templbw, templbw, cv::MORPH_CLOSE, element_b);

		filePath = bw_path + fileName + "_bw" + userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), templbw);

		//��ȡģ������������
		filePath = bin_path + fileName + ".bin";
		templFunc->save(filePath.toStdString(), src);

		//�ϳɴ�ģ��
		Rect roiRect = Rect(col*adminConfig->ImageSize_W, currentRow_extract*adminConfig->ImageSize_H,
			adminConfig->ImageSize_W, adminConfig->ImageSize_H);
		Mat roi = templFunc->getBigTempl(roiRect);
		src.copyTo(roi);
	}

	//�洢PCB��ͼ
	if (runtimeParams->currentRow_extract + 1 == nPhotographing) {
		Mat scaledFullImage;
		cv::resize(templFunc->getBigTempl(), scaledFullImage, scaledFullImageSize); //����

		filePath = templ_path;
		filePath += QString("fullImage_%1_%2").arg(originalFullImageSize.width).arg(originalFullImageSize.height);
		filePath += userConfig->ImageFormat;
		cv::imwrite(filePath.toStdString(), scaledFullImage);
	}

	//��ȡ����
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