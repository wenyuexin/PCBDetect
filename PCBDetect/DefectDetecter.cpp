#include "DefectDetecter.h"
#include <exception>
#include <string>
#include <map>

using pcb::CvMatVector;
using pcb::UserConfig;
using pcb::RuntimeParams;
using pcb::DetectResult;
using pcb::QImageVector;
using pcb::CvMatVector;
using cv::Mat;
using cv::Point;
using cv::Size;
using cv::Rect;
using std::string;
using std::vector;
using std::to_string;


DefectDetecter::DefectDetecter() 
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	userConfig = Q_NULLPTR; //�û�����
	runtimeParams = Q_NULLPTR; //���в���
	detectResult = Q_NULLPTR; //�����
	cvmatSamples = Q_NULLPTR; //���ڼ���һ������
	detectFunc = Q_NULLPTR; //��⸨����
	detectState = Default; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�
	totalDefectNum = INT_MIN; //ȱ������
}

DefectDetecter::~DefectDetecter() 
{
	qDebug() << "~DefectDetecter";
	deleteDetectUnits();
	delete detectFunc;
	detectFunc = Q_NULLPTR;
}


void DefectDetecter::init()
{
	generateBigTempl(); //���ɴ�ģ��
	initDetectFunc(); //��ʼ����⺯����
	initDetectUnits(); //�����ɼ�ⵥԪ���г�ʼ��
}

//��ʼ��templFunc
void DefectDetecter::initDetectFunc()
{
	delete detectFunc;
	detectFunc = new DetectFunc;
	detectFunc->setAdminConfig(adminConfig);
	detectFunc->setUserConfig(userConfig);
	detectFunc->setRuntimeParams(runtimeParams);
	//detectFunc->setDetectResult(detectResult);
}

//���������ߴ��ȱ�ݼ��ͼ��
void DefectDetecter::generateBigTempl()
{
	Size originalfullImgSize = Size(adminConfig->ImageSize_W * runtimeParams->nCamera,
		adminConfig->ImageSize_H * runtimeParams->nPhotographing); //��ͼ��ԭʼ�ߴ�

	double factorW = 1.0 * runtimeParams->ScreenRect.width() / originalfullImgSize.width;
	double factorH = 1.0 * runtimeParams->ScreenRect.height() / originalfullImgSize.height;
	scalingFactor = qMin(factorW, factorH); //��������

	scalingFactor = 1;

	scaledSubImageSize = Size(scalingFactor * adminConfig->ImageSize_W,
		scalingFactor * adminConfig->ImageSize_H); //��ͼ�������ź�ĳߴ�

	scaledFullImageSize = Size(scaledSubImageSize.width * runtimeParams->nCamera,
		scaledSubImageSize.height * runtimeParams->nPhotographing); //��ͼ�������ź�ĳߴ�

	bigTempl = Mat(scaledFullImageSize, CV_8UC3); //�������ڼ�¼ȱ�ݵ���ͼ
}


/********* ��ⵥԪ�ĳ�ʼ����delete ***********/

void DefectDetecter::initDetectUnits()
{
	deleteDetectUnits(); //�����ʷ����
	detectUnits.resize(MAX_DETECT_UNITS_NUM);
	for (int i = 0; i < MAX_DETECT_UNITS_NUM; i++) {
		detectUnits[i] = new DetectUnit();
		detectUnits[i]->setCurrentCol(i);//�����к�
		detectUnits[i]->setDetectFunc(detectFunc);//���е�Ԫ����һ��detectFunc
		detectUnits[i]->setAdminConfig(adminConfig); //����ϵͳ����
		detectUnits[i]->setUserConfig(userConfig); //�����û�����
		detectUnits[i]->setRuntimeParams(runtimeParams); //�������в���
	}
}

void DefectDetecter::deleteDetectUnits()
{
	for (int i = 0; i < detectUnits.size(); i++) {
		delete detectUnits[i];
		detectUnits[i] = Q_NULLPTR;
	}
	detectUnits.clear();
}


/**************** ��⵱ǰ������ ****************/

void DefectDetecter::detect()
{
	detectState = DetectState::Start;
	emit updateDetectState_detecter(detectState);
	double t1 = clock();
	qDebug() << "====================" << pcb::chinese("��ʼ���") <<
		"( currentRow_detect =" << runtimeParams->currentRow_detect << ")" << endl;

	int currentRow_detect = runtimeParams->currentRow_detect;
	int nCamera = runtimeParams->nCamera;
	int nPhotographing = runtimeParams->nPhotographing;

	//����Ӧ������Ŀ¼�Ƿ���ڣ��������򴴽�
	if (currentRow_detect == 0) makeCurrentSampleDir();

	//����Ӧ�����Ŀ¼�Ƿ���ڣ��������򴴽�
	vector<QString> subFolders { "fullImage" };
	if (currentRow_detect == 0) makeCurrentOutputDir(subFolders);

	//��ȡ��Ĥ��������ֵ
	if (currentRow_detect == 0) {
		QString cornerPointsPath = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/mask/cornerPoints.bin";
		cv::FileStorage store_new(cornerPointsPath.toStdString(), cv::FileStorage::READ);
		cv::FileNode node = store_new["cornerPoints"];
		vector<cv::Point2i> res;
		cv::read(node, res);
		if (res.size() == 0) {
			qDebug() << pcb::chinese("DefectDetecter: ����ģ�����ģ��������ʧ��");
			errorCode = LoadTemplMaskRoiError; return;
		}
		maskRoi_bl = res[0];
		maskRoi_tr = res[1];
		store_new.release();
	}

	//���ⵥԪ��������������PCB��Ĳ���
	if (currentRow_detect == 0) {
		for (int i = 0; i < nPhotographing; i++) {
			detectUnits[i]->setMaskRoi(&maskRoi_bl, &maskRoi_tr);//������ģ��������
			detectUnits[i]->setScalingFactor(scalingFactor); //������������
			detectUnits[i]->setScaledFullImageSize(&scaledFullImageSize); //�������ź����ͼͼ��ߴ�
			detectUnits[i]->setScaledSubImageSize(&scaledSubImageSize); //�������ź�ķ�ͼͼ��ߴ�
		}
	}

	//���ⵥԪ���������ڵ�ǰ�еĲ���
	CvMatVector subImages = (*cvmatSamples)[currentRow_detect];
	for (int i = 0; i < nPhotographing; i++) {
		detectUnits[i]->setSubImage(*subImages[i]);//������Ҫ���ķ�ͼ
	}

	//�������ɼ���̣߳���⵱ǰ��һ�з�ͼ
	for (int i = 0; i < nPhotographing; i++) {
		detectUnits[i]->start(); //��ʼת��
	}

	//�ȴ����м�ⵥԪ���н���
	for (int i = 0; i < nPhotographing; i++) {
		detectUnits[i]->wait();
	}

	//���ϵ�ǰ�еļ����
	totalDefectNum = 0; //ȱ������
	for (int i = 0; i < nPhotographing; i++) {
		//ͳ��ȱ������
		int defectNum = detectUnits[i]->getDefectNum();
		totalDefectNum += defectNum;

		//�������ȱ�ݵķ�ͼ���Ƶ���ͼ
		Mat markedSubImage = detectUnits[i]->getMarkedSubImage();
		int curCol = detectUnits[i]->getcurCol();
		int curRow = detectUnits[i]->getcurRow();
		Rect rect(Point(curCol*markedSubImage.cols, curRow*markedSubImage.rows), markedSubImage.size());
		markedSubImage.copyTo(bigTempl(rect));

		//��ȱ��ϸ��ͼ���������Ϣ�ϲ���һ��
		std::map<cv::Point3i, cv::Mat, cmp_point3i> detailImage = detectUnits[i]->getDetailImage();
		allDetailImage.insert(detailImage.begin(), detailImage.end());
	}
	
	
	//for (int i = 0; i < (*cvmatSamples)[currentRow_detect].size(); i++) {
	//	int curRow = currentRow_detect;//��ǰ��
	//	int curCol = i;//��ǰ��
	//
	//	double t1 = clock();
	//	//��ȡģ����Ĥ
	//	QString mask_path = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/mask/" 
	//		+ QString("%1_%2_mask").arg(curRow+1).arg(curCol+1) + userConfig->ImageFormat;
	//	cv::Mat mask_roi = cv::imread(mask_path.toStdString(), 0);
	//
	//	//��ȡģ��ͼƬ
	//	QString templPath = userConfig->TemplDirPath + "/" + runtimeParams->sampleModelNum + "/subtempl/"
	//		+ QString("%1_%2").arg(curRow + 1).arg(curCol + 1) + userConfig->ImageFormat;
	//	Mat templ_gray = cv::imread(templPath.toStdString(), 0);
	//
	//	//��ȡ����ͼƬ
	//	Mat samp = *((*cvmatSamples)[curRow][curCol]);
	//	Mat samp_gray;
	//	cv::cvtColor(samp, samp_gray, cv::COLOR_BGR2GRAY);
	//
	//	//��������ͼƬ
	//	QString sampPath = runtimeParams->currentSampleDir + "/" + QString("%1_%2").arg(curRow + 1).arg(curCol + 1) + ".bmp";
	//	cv::imwrite(sampPath.toStdString(), samp);
	//
	//	double t2 = clock();
	//	qDebug() << "==========" << pcb::chinese("ģ����̬ѧ����") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;
	//
	//		
	//	//������ģ����׼
	//	cv::Mat samp_gray_reg, h;
	//	cv::Mat imMatches;
	//	//���������ķ���
	//	//string bin_path = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString()
	//	//	+ "/bin/" + to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + ".bin";
	//	//detectFunc->load(bin_path);
	//	//detectFunc->alignImages_test_load(detectFunc->keypoints, detectFunc->descriptors, samp_gray, samp_gray_reg, h, imMatches);
	//
	//	//ÿ�μ���ķ���
	//	Mat templGrayRoi, sampGrayRoi;
	//	cv::bitwise_and(mask_roi, templ_gray, templGrayRoi);
	//	detectFunc->alignImages_test(templGrayRoi, samp_gray, samp_gray_reg, h, imMatches);
	//	double t3 = clock();
	//	qDebug() << "==========" << pcb::chinese("ģ��ƥ�䣺") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;
	//
	//	double ratio = 0.67;
	//	Size roiSize = samp_gray.size();
	//	Rect upRect = Rect(0, 0, roiSize.width, int(ratio*roiSize.height));
	//	Rect downRect = Rect(0, int(ratio*roiSize.height), roiSize.width, roiSize.height - int(ratio*roiSize.height));
	//	//������ֵ��
	//	Mat sampBw = Mat::zeros(samp_gray.size(), CV_8UC1);
	//	//����Ӧ��ֵ��
	//	//cv::adaptiveThreshold(samp_gray, sampBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
	//	//��ֵ��ֵ��
	//	//int meanSampGray = mean(samp_gray,mask_roi)[0];
	//	//cv::threshold(samp_gray, sampBw, meanSampGray, 255, cv::THRESH_BINARY_INV);
	//	//�ֿ��ֵ��
	//	//int meanSampGrayUp = mean(samp_gray(upRect), mask_roi(upRect))[0];
	//	//cv::threshold(samp_gray(upRect), sampBw(upRect), meanSampGrayUp, 255, cv::THRESH_BINARY_INV);
	//	//int meanSampGrayDown = mean(samp_gray(downRect), mask_roi(downRect))[0];
	//	//cv::threshold(samp_gray(downRect), sampBw(downRect), meanSampGrayDown, 255, cv::THRESH_BINARY_INV);
	//	//�ֲ�����Ӧ��ֵ��
	//	cv::Point bl(detectFunc->get_bl());
	//	cv::Point tr(detectFunc->get_tr());
	//
	//	sampBw = detectFunc->myThresh(curCol, curRow, samp_gray, bl, tr);
	//
	//	Mat element_a = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3));
	//	cv::morphologyEx(sampBw, sampBw, cv::MORPH_OPEN, element_a);
	//	cv::morphologyEx(sampBw, sampBw, cv::MORPH_CLOSE, element_a);
	//
	//
	//	//ֱ�������ֵ��ģ��
	//	//std::string templBwPath = userConfig->TemplDirPath.toStdString() + "/" + runtimeParams->sampleModelNum.toStdString() + "/bw/"
	//	//	+ to_string(runtimeParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_bw" + userConfig->ImageFormat.toStdString();
	//	//Mat templBw = cv::imread(templBwPath, 0);
	//
	//	//ÿ������ģ��Ķ�ֵ��
	//	Mat templBw = Mat::zeros(samp_gray.size(), CV_8UC1);
	//	//����Ӧ��ֵ��
	//	//cv::adaptiveThreshold(templ_gray, templBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
	//	//��ֵ��ֵ��
	//	//int meanTemplGray = mean(templ_gray, mask_roi)[0];
	//	//cv::threshold(templ_gray, templBw, meanTemplGray, 255, cv::THRESH_BINARY_INV);
	//	//�ֿ��ֵ��
	//	//int meanTemplGrayUp = mean(templ_gray(upRect), mask_roi(upRect))[0];
	//	//cv::threshold(templ_gray(upRect), templBw(upRect), meanTemplGrayUp, 255, cv::THRESH_BINARY_INV);
	//	//int meanTemplGrayDown = mean(templ_gray(downRect), mask_roi(downRect))[0];
	//	//cv::threshold(templ_gray(downRect), templBw(downRect), meanTemplGrayDown, 255, cv::THRESH_BINARY_INV);
	//	//�ֲ�����Ӧ��ֵ��
	//	templBw = detectFunc->myThresh(curCol, curRow, templ_gray, bl, tr);
	//
	//	Mat elementTempl = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	//	cv::morphologyEx(templBw, templBw, cv::MORPH_OPEN, elementTempl);
	//	cv::morphologyEx(templBw, templBw, cv::MORPH_CLOSE, elementTempl);
	//
	//
	//	//͸��任����һ��roi
	//	Mat templ_roi = Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
	//	cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());
	//
	//	Mat templRoiReverse = 255 - templ_roi;
	//	cv::add(samp_gray_reg, templ_gray, samp_gray_reg, templRoiReverse);
	//
	//	//�ܵ�roi
	//	Mat roi;
	//	cv::bitwise_and(templ_roi, mask_roi, roi);
	//
	//	//ֱ�Ӷ�roi��Ĥ��Ͷ��任
	//	cv::warpPerspective(mask_roi, roi, h, templ_roi.size());
	//	cv::bitwise_and(roi, mask_roi, roi);
	//
	//	//����
	//	cv::warpPerspective(sampBw, sampBw, h, roi.size());//������ֵͼ����Ӧ�ı任���Ժ�ģ�����
	//	//Mat diff = detectFunc->sub_process_new(templBw, sampBw, roi);
	//	Mat diff = detectFunc->sub_process_direct(templBw, sampBw, templ_gray, samp_gray_reg, roi);
	//	//����ʱ��ı�Ե����
	//	Size szDiff = diff.size();
	//	Mat diff_roi = Mat::zeros(szDiff, diff.type());
	//	int zoom = 50;//���Եı�Ե���
	//	diff_roi(cv::Rect(zoom, zoom, szDiff.width - 2 * zoom, szDiff.height - 2 * zoom)) = 255;
	//	bitwise_and(diff_roi, diff, diff);
	//
	//	string debug_path = "D:\\PCBData\\debugImg\\" + to_string(curRow) + "_" + to_string(curCol) + "_";
	//	cv::imwrite(debug_path + to_string(1) + ".bmp", templ_gray);
	//	cv::imwrite(debug_path + to_string(2) + ".bmp", templBw);
	//	cv::imwrite(debug_path + to_string(3) + ".bmp", samp_gray_reg);
	//	cv::imwrite(debug_path + to_string(4) + ".bmp", sampBw);
	//	cv::imwrite(debug_path + to_string(5) + ".bmp", diff);
	//
	//	//���ȱ��
	//	detectFunc->markDefect_test(diff, samp_gray_reg, templBw, templ_gray, defectNum, i);
	//	continue;
	//}


	//�����ǰ���������һ��ͼ��
	if (currentRow_detect == nPhotographing-1) {
		Size sz(adminConfig->ImageSize_W*nCamera, adminConfig->ImageSize_H*nCamera);
		QString fullImageDir = runtimeParams->currentOutputDir + "/" + subFolders[0] + "/";

		QString filePath = fullImageDir; //����ļ���·��
		filePath += QString("fullImage_%1_%2_%3").arg(sz.width).arg(sz.height).arg(totalDefectNum); //����ļ���
		filePath += userConfig->ImageFormat; //����ļ���׺
		cv::imwrite(filePath.toStdString(), bigTempl); //��ͼ

		//�洢ϸ��ͼ
		QChar fillChar = '0'; //���ַ������Ȳ���ʱʹ�ô��ַ��������
		int defectNum = 0;//ȱ�����
		for (auto beg = allDetailImage.begin(); beg!=allDetailImage.end(); beg++) {
			defectNum++;
			cv::Point3i info = (*beg).first;
			cv::Mat imgSeg = (*beg).second;
			QString outPath = runtimeParams->currentOutputDir + "/"; //��ǰ��Ŷ�Ӧ�����Ŀ¼
			outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(info.x, 5, 10, fillChar).arg(info.y, 5, 10, fillChar).arg(info.z);
			outPath += userConfig->ImageFormat; //���ͼ���ʽ�ĺ�׺
			cv::imwrite(outPath.toStdString(), imgSeg);//��ϸ��ͼ�洢������Ӳ����
		}
		
		//������淢���Ƿ�ϸ����Ϣ
		bool qualified = (totalDefectNum < 100);
		emit detectFinished_detectThread(qualified);

		//�����ʷ����
		generateBigTempl(); //�������ɴ�ͼ
		totalDefectNum = -1;//��ȱ��������λ
	}

	//������
	double t2 = clock();
	qDebug() << "====================" << pcb::chinese("��ǰ�м�������") << 
		(t2 - t1) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;
	
	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);
}


/*********** ������Ӧ�������ļ��С�����ļ��� ************/

//�ж����Ʒ��Ŷ�Ӧ�������ļ����Ƿ���ڣ����������򴴽�
void DefectDetecter::makeCurrentSampleDir(std::vector<QString> &subFolders)
{
	//�ж϶����sample�ļ����Ƿ����
	runtimeParams->currentSampleDir = userConfig->SampleDirPath;
	QDir outputDir(runtimeParams->currentSampleDir);
	if (!outputDir.exists()) outputDir.mkdir(runtimeParams->currentSampleDir);

	//�ж϶�Ӧ���ͺ��ļ����Ƿ����
	runtimeParams->currentSampleDir += "/" + runtimeParams->sampleModelNum;
	QDir modelDir(runtimeParams->currentSampleDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentSampleDir);

	//�ж϶�Ӧ�����κ��ļ����Ƿ����
	runtimeParams->currentSampleDir += "/" + runtimeParams->sampleBatchNum;
	QDir batchDir(runtimeParams->currentSampleDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentSampleDir);

	//�ж϶�Ӧ����������ļ����Ƿ����
	runtimeParams->currentSampleDir += "/" + runtimeParams->sampleNum;
	QDir resultDir(runtimeParams->currentSampleDir);
	if (!resultDir.exists()) {
		resultDir.mkdir(runtimeParams->currentSampleDir);//�����ļ���
	}
	else if (!runtimeParams->DeveloperMode) { //�ļ��д����ҿ�����ģʽδ����
		pcb::clearFolder(runtimeParams->currentSampleDir, false);//����ļ���
	}

	//������ļ���
	for (int i = 0; i < subFolders.size(); i++) {
		resultDir.mkdir(subFolders[i]);
	}
}

//�ж����Ʒ��Ŷ�Ӧ������ļ����Ƿ���ڣ����������򴴽�
void DefectDetecter::makeCurrentOutputDir(vector<QString> &subFolders)
{
	//�ж϶����output�ļ����Ƿ����
	runtimeParams->currentOutputDir = userConfig->OutputDirPath; 
	QDir outputDir(runtimeParams->currentOutputDir);
	if (!outputDir.exists()) outputDir.mkdir(runtimeParams->currentOutputDir);

	//�ж϶�Ӧ���ͺ��ļ����Ƿ����
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleModelNum;
	QDir modelDir(runtimeParams->currentOutputDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentOutputDir);

	//�ж϶�Ӧ�����κ��ļ����Ƿ����
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleBatchNum;
	QDir batchDir(runtimeParams->currentOutputDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentOutputDir);

	//�ж϶�Ӧ����������ļ����Ƿ����
	runtimeParams->currentOutputDir += "/" + runtimeParams->sampleNum;
	QDir resultDir(runtimeParams->currentOutputDir);
	if (!resultDir.exists()) {
		resultDir.mkdir(runtimeParams->currentOutputDir);//�����ļ���
	}
	else {
		pcb::clearFolder(runtimeParams->currentOutputDir, false);//����ļ���
	}

	//������ļ���
	for (int i = 0; i < subFolders.size(); i++) {
		resultDir.mkdir(subFolders[i]);
	}
}
