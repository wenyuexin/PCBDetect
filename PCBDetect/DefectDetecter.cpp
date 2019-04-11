#include "DefectDetecter.h"
#include <exception>

using pcb::CvMatVector;
using pcb::DetectConfig;
using pcb::DetectParams;
using pcb::DetectResult;
using pcb::QImageVector;
using cv::Mat;
using std::string;


DefectDetecter::DefectDetecter() 
{
	adminConfig = Q_NULLPTR; //ϵͳ����
	detectConfig = Q_NULLPTR; //�û�����
	detectParams = Q_NULLPTR; //���в���
	detectResult = Q_NULLPTR; //�����
	cvmatSamples = Q_NULLPTR; //���ڼ���һ������
	detectFunc = Q_NULLPTR; //��⸨����
	detectState = Default; //���״̬�����ڽ�����ʾ�ͳ�����ԣ�
	defectNum = 0; //ȱ����
}

DefectDetecter::~DefectDetecter() 
{
	delete detectFunc;
}

//��ʼ��templFunc
void DefectDetecter::initDetectFunc()
{
	detectFunc = new DetectFunc;
	detectFunc->setAdminConfig(adminConfig);
	detectFunc->setDetectConfig(detectConfig);
	detectFunc->setDetectParams(detectParams);
	detectFunc->setDetectResult(detectResult);
}


/***************** ��� ******************/

void DefectDetecter::detect()
{
	qDebug() << ">>>>>>>>>> " << pcb::chinese("��ʼ��� ... ") <<
		"( currentRow_detect -" << detectParams->currentRow_detect << ")";

	detectState = DetectState::Start; //���ü��״̬
	emit updateDetectState_detecter(detectState);
	double t1 = clock();

	//��ʼ���
	int currentRow_detect = detectParams->currentRow_detect;
	for (int i = 0; i < (*cvmatSamples)[detectParams->currentRow_detect].size(); i++) {
		double t1 = clock();

		//��ȡģ��ͼƬ
		string templPath = (detectConfig->TemplDirPath + "/" + detectParams->sampleModelNum + "/"
			+ QString::number(currentRow_detect + 1) + "_" + QString::number(i + 1)  
			+ detectConfig->ImageFormat).toStdString();
		Mat templ_gray = cv::imread(templPath, 0);

		//��ȡ����ͼƬ
		Mat samp = *((*cvmatSamples)[detectParams->currentRow_detect][i]);
		Mat samp_gray;
		cvtColor(samp, samp_gray, CV_BGR2GRAY);

		//ģ���ֵ������̬ѧ����
		Mat templ_bw;
		cv::threshold(templ_gray, templ_bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		Mat element_b = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::morphologyEx(templ_bw, templ_bw, cv::MORPH_OPEN, element_b);
		cv::morphologyEx(templ_bw, templ_bw, cv::MORPH_CLOSE, element_b);

		double t2 = clock();
		qDebug() << QString::fromLocal8Bit("==========ģ����̬ѧ������") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;

		try {
			//������ģ����׼
			Mat samp_gray_reg, h;
			Mat imMatches;
			detectFunc->alignImages(samp_gray, templ_gray, samp_gray_reg, h, imMatches);
			double t3 = clock();
			qDebug() << QString::fromLocal8Bit("==========��׼ʱ�䣺") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;

			//��ģ������Ĥ����,�������任�󲿷����򳬳��߽磬����Ҫ��ģ�弰���ֵͼ����Ĥ����
			Mat templ_gray_reg, templ_bw_reg;
			Mat templ_roi = cv::Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
			cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());
			cv::bitwise_and(templ_roi, templ_gray, templ_gray_reg);
			cv::bitwise_and(templ_roi, templ_bw, templ_bw_reg);

			//Ԥ����
			Mat diff = detectFunc->sub_process(templ_gray_reg, samp_gray_reg);
			string mask_path = detectConfig->TemplDirPath.toStdString() + "/" 
				+ detectParams->sampleModelNum.toStdString()
				+ "/mask/" + std::to_string(detectParams->currentRow_detect + 1) + "_" 
				+ std::to_string(i + 1) + "_mask" + detectConfig->ImageFormat.toStdString();
			Mat roi_mask = cv::imread(mask_path, 0);
			cv::bitwise_and(diff, roi_mask, diff);

			time_t t4 = clock();
			qDebug() << QString::fromLocal8Bit("===========����ʱ�䣺") << double(t4 - t3) / CLOCKS_PER_SEC << "s" << endl;
			string path_head;
			//path_head += to_string(params->currentRow_detect + 1) + "_" + to_string(i + 1) + "_";
			//cv::imwrite("./res_06/diff/" + path_head + "_diff.jpg", diff);
			//cv::imwrite("./res_06/match/" + path_head + "_imMatches.jpg", imMatches);
			//cv::imwrite("./res_06/reg/" + path_head + "_samp_gray_reg.jpg", samp_gray_reg);
			detectFunc->markDefect(diff, samp_gray_reg, templ_bw_reg, templ_gray_reg, defectNum, i);
			qDebug() << endl;
		}catch (std::exception e) {
			qDebug() << detectParams->currentRow_detect + 1 << "_" << i + 1 << "_" <<
				QString::fromLocal8Bit("�����쳣");
		}	
	}
	if (detectParams->currentRow_detect + 1 == detectParams->nPhotographing)
		defectNum = 0;

	//������
	double t2 = clock();
	qDebug() << ">>>>>>>>>> " << pcb::chinese("��ǰ�м�����") << 
		(t2 - t1) << "ms  ( currentRow_detect -" << detectParams->currentRow_detect << ")";
	
	detectState = DetectState::Finished;
	emit updateDetectState_detecter(detectState);
}