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

	double factorW = 2.0 * runtimeParams->ScreenRect.width() / originalfullImgSize.width;
	double factorH = 2.0 * runtimeParams->ScreenRect.height() / originalfullImgSize.height;
	scalingFactor = qMin(factorW, factorH); //��������

	//scalingFactor = 1; 1

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
		QString cornerPointsPath = userConfig->TemplDirPath + "/" + runtimeParams->productID.modelType + "/mask/cornerPoints.bin";
		cv::FileStorage store_new(cornerPointsPath.toStdString(), cv::FileStorage::READ);
		cv::FileNode node = store_new["cornerPoints"];
		vector<cv::Point2i> res;
		cv::read(node, res);
		if (res.size() == 0) {
			qDebug() << pcb::chinese("DefectDetecter: ����ģ�����ģ�����������ֵʧ��");
			errorCode = LoadTemplMaskRoiError; return;
		}
		maskRoi_bl = res[0];
		maskRoi_tr = res[1];
		segThresh = res[2].x;//ȫ����ֵ
		UsingDefaultSegThresh = res[2].y;//�ָ��־

		store_new.release();
	}

	static int templateFlag = -999;
	if (templateFlag != ((runtimeParams->productID.modelType).toInt()) || ((detectFunc->templateVec).empty() || (detectFunc->maskVec).empty())) {
		(detectFunc->templateVec).clear();
		(detectFunc->maskVec).clear();
		detectFunc->templateVec = std::vector<std::vector<cv::Mat>>(runtimeParams->nPhotographing, std::vector<cv::Mat>{});
		detectFunc->maskVec = std::vector<std::vector<cv::Mat>>(runtimeParams->nPhotographing, std::vector<cv::Mat>{});
		for (int i = 0; i < runtimeParams->nPhotographing; i++)
			for (int j = 0; j < runtimeParams->nCamera; j++)
			{
				QString templPath = userConfig->TemplDirPath + "/" + runtimeParams->productID.modelType + "/subtempl/"
					+ QString("%1_%2").arg(i + 1).arg(j + 1) + ".bmp";
				Mat templGray = cv::imread(templPath.toStdString(), 0);
				detectFunc->templateVec[i].push_back(templGray);

				QString mask_path = userConfig->TemplDirPath + "/" + runtimeParams->productID.modelType + "/mask/"
					+ QString("%1_%2_mask").arg(i + 1).arg(j + 1) + ".bmp";
				Mat mask_roi = cv::imread(mask_path.toStdString(), 0);
				detectFunc->maskVec[i].push_back(mask_roi);
			}
		templateFlag = (runtimeParams->productID.modelType).toInt();
	}

	//���ⵥԪ��������������PCB��Ĳ���
	if (currentRow_detect == 0) {
		totalDefectNum = 0; //ȱ������
		for (int i = 0; i < nCamera; i++) {
			detectUnits[i]->setMaskRoi(&maskRoi_bl, &maskRoi_tr);//������ģ��������
			detectUnits[i]->setSegThresh(segThresh);//����ȫ����ֵ
			detectUnits[i]->setThreshFlag(UsingDefaultSegThresh);//�����Զ��ָ��־
			detectUnits[i]->setScalingFactor(scalingFactor); //������������
			detectUnits[i]->setScaledFullImageSize(&scaledFullImageSize); //�������ź����ͼͼ��ߴ�
			detectUnits[i]->setScaledSubImageSize(&scaledSubImageSize); //�������ź�ķ�ͼͼ��ߴ�
		}
	}

	//nPhotographing
	//���ⵥԪ���������ڵ�ǰ�еĲ���
	CvMatVector subImages = (*cvmatSamples)[currentRow_detect];
	for (int i = 0; i < nCamera; i++) {
		detectUnits[i]->setSubImage(*subImages[i]);//������Ҫ���ķ�ͼ
	}

	//�������ɼ���̣߳���⵱ǰ��һ�з�ͼ
	for (int i = 0; i < nCamera; i++) {
		detectUnits[i]->start(); //��ʼת��
	}

	//�ȴ����м�ⵥԪ���н���
	for (int i = 0; i < nCamera; i++) {
		detectUnits[i]->wait();
	}

	//�����������ϵ�ǰ�еļ����
	
	for (int i = 0; i < nCamera; i++) {
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
		detectUnits[i]->clearDetailImage();
	}

	//������
	double t2 = clock();
	qDebug() << "====================" << pcb::chinese("��ǰ�м�������") <<
		(t2 - t1) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;

	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);

	//�����ǰ���������һ��ͼ�� 
	if (currentRow_detect == nPhotographing-1) {
		//����洢�����Ϣ�Ķ���
		vector<pcb::FlawInfo> flawInfos(allDetailImage.size());
		pcb::DetectResult *detectResult = new DetectResult;
		
		Size sz(adminConfig->ImageSize_W*nCamera, adminConfig->ImageSize_H*nCamera);
		QString fullImageDir = runtimeParams->currentOutputDir + "/" + subFolders[0] + "/";

		//�洢��ͼ
		QString filePath = fullImageDir; //����ļ���·��
		filePath += QString("fullImage_%1_%2_%3").arg(sz.width).arg(sz.height).arg(totalDefectNum); //����ļ���
		filePath += userConfig->ImageFormat; //����ļ���׺
		cv::imwrite(filePath.toStdString(), bigTempl); //���ͼ

		//�洢ϸ��ͼ
		QChar fillChar = '0'; //���ַ������Ȳ���ʱʹ�ô��ַ��������
 		int defectNum = 0;// ȱ�����
		for (auto beg = allDetailImage.begin(); beg!=allDetailImage.end(); beg++) {

			defectNum++;
			cv::Point3i info = (*beg).first;
			cv::Mat imgSeg = (*beg).second;
			QString outPath = runtimeParams->currentOutputDir + "/"; //��ǰ��Ŷ�Ӧ�����Ŀ¼
			outPath += QString("%1_%2_%3_%4").arg(defectNum, 4, 10, fillChar).arg(info.x, 5, 10, fillChar).arg(info.y, 5, 10, fillChar).arg(info.z);
			outPath += userConfig->ImageFormat; //���ͼ���ʽ�ĺ�׺
			cv::imwrite(outPath.toStdString(), imgSeg);//��ϸ��ͼ�洢������Ӳ����

			//����ͼȱ����Ϣ�����FlawInfo����
			pcb::FlawInfo temp;
			temp.flawIndex = defectNum;
			temp.flawType = info.z;
			temp.xPos = info.x;
			temp.yPos = info.y;
			flawInfos[defectNum-1] = temp;
		}
		
		//������淢���Ƿ�ϸ����Ϣ
		bool qualified = (totalDefectNum < 1);
		emit detectFinished_detectThread(qualified);

		//�������Ϣ����������
		detectResult->flawInfo = flawInfos;
		detectResult->fullImage = bigTempl.clone();
		detectResult->SampleIsQualified = qualified;

		//�����ʷ����
		allDetailImage.clear();
		generateBigTempl(); //�������ɴ�ͼ
		totalDefectNum = -1;//��ȱ��������λ

		double t3 = clock();
		qDebug() << "====================" << pcb::chinese("�洢�������") <<
			(t3 - t2) << "ms  ( currentRow_detect =" << currentRow_detect << ")" << endl;
	}
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
	runtimeParams->currentSampleDir += "/" + runtimeParams->productID.modelType;
	QDir modelDir(runtimeParams->currentSampleDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentSampleDir);

	//�ж϶�Ӧ�����κ��ļ����Ƿ����
	runtimeParams->currentSampleDir += "/" + runtimeParams->productID.getDateString();
	QDir batchDir(runtimeParams->currentSampleDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentSampleDir);

	//�ж϶�Ӧ����������ļ����Ƿ����
	runtimeParams->currentSampleDir += "/" + QString::number(runtimeParams->productID.serialNum);
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
	runtimeParams->currentOutputDir += "/" + runtimeParams->productID.modelType;
	QDir modelDir(runtimeParams->currentOutputDir);
	if (!modelDir.exists()) modelDir.mkdir(runtimeParams->currentOutputDir);

	//�ж϶�Ӧ�����κ��ļ����Ƿ����
	runtimeParams->currentOutputDir += "/" + runtimeParams->productID.getDateString();
	QDir batchDir(runtimeParams->currentOutputDir);
	if (!batchDir.exists()) batchDir.mkdir(runtimeParams->currentOutputDir);

	//�ж϶�Ӧ����������ļ����Ƿ����
	runtimeParams->currentOutputDir += "/" + QString::number(runtimeParams->productID.serialNum);
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
