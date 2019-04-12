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
		string templPath = (detectConfig->TemplDirPath + "/" + detectParams->sampleModelNum + "/subtempl/"
			+ QString::number(currentRow_detect + 1) + "_" + QString::number(i + 1)  
			+ detectConfig->ImageFormat).toStdString();
		Mat templ_gray = cv::imread(templPath, 0);

		//��ȡģ���ֵͼ
		std::string templBwPath = detectConfig->TemplDirPath.toStdString() + "/" + detectParams->sampleModelNum.toStdString() + "/bw/"
		+ to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1)+"_bw" + detectConfig->ImageFormat.toStdString();
		Mat templBw = cv::imread(templBwPath,0);



		//��ȡ����ͼƬ
		Mat samp = *((*cvmatSamples)[detectParams->currentRow_detect][i]);
		Mat samp_gray;
		cvtColor(samp, samp_gray, CV_BGR2GRAY);

		//������ֵ��
		cv::Mat sampBw;
		cv::adaptiveThreshold(samp_gray, sampBw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 2001, 0);
		cv::Mat element_a = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		cv::morphologyEx(sampBw, sampBw, cv::MORPH_OPEN, element_a);
		cv::morphologyEx(sampBw, sampBw, cv::MORPH_CLOSE, element_a);



		double t2 = clock();
		qDebug() << QString::fromLocal8Bit("==========ģ����̬ѧ����") << (t2 - t1) / CLOCKS_PER_SEC << "s" << endl;

		try {
			//������ģ����׼
			cv::Mat samp_gray_reg, h;
			cv::Mat imMatches;
			//detectFunc.alignImages(samp_gray, templ_gray, samp_gray_reg, h, imMatches);
			string bin_path = detectConfig->TemplDirPath.toStdString() + "/" + detectParams->sampleModelNum.toStdString()
				+ "/bin/" + to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + ".bin";
			detectFunc->load(bin_path);
			detectFunc->alignImages_test_load(detectFunc->keypoints, detectFunc->descriptors, samp_gray, samp_gray_reg, h, imMatches);
			double t3 = clock();
			qDebug() << QString::fromLocal8Bit("==========��׼ʱ�䣺") << (t3 - t2) / CLOCKS_PER_SEC << "s" << endl;

			//͸��任����һ��roi
			cv::Mat templ_roi = cv::Mat::ones(templ_gray.size(), templ_gray.type()) * 255;
			cv::warpPerspective(templ_roi, templ_roi, h, templ_roi.size());

			//��Ե��һ��roi
			string mask_path = detectConfig->TemplDirPath.toStdString() + "/" + detectParams->sampleModelNum.toStdString()
				+ "/mask/" + to_string(detectParams->currentRow_detect + 1) + "_" + std::to_string(i + 1) + "_mask" + detectConfig->ImageFormat.toStdString();
			cv::Mat mask_roi = cv::imread(mask_path, 0);

			//�ܵ�roi
			cv::Mat roi;
			cv::bitwise_and(templ_roi, mask_roi, roi);

			//����
			cv::warpPerspective(sampBw, sampBw, h, roi.size());//������ֵͼ����Ӧ�ı任���Ժ�ģ�����
			cv::Mat diff = detectFunc->sub_process_new(templBw, sampBw, roi);

			//����ʱ��ı�Ե����
			cv::Size szDiff = diff.size();
			cv::Mat diff_roi = cv::Mat::zeros(szDiff, diff.type());
			int zoom = 20;
			diff_roi(cv::Rect(zoom, zoom, szDiff.width - 2 * zoom, szDiff.height - 2 * zoom)) = 255;
			bitwise_and(diff_roi, diff, diff);

			//���ȱ��
			//detectFunc->markDefect_new(diff, samp_gray_reg, templBw, templ_gray, defectNum, i);



			//����ʹ��
			Mat kernel_small = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			dilate(diff, diff, kernel_small);//�Բ�ֵͼ�������ͣ���������ͽ����ж�
			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i>   hierarchy;
			cv::findContours(diff, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
			cv::cvtColor(diff, diff, COLOR_GRAY2BGR);
			for (int i = 0; i < contours.size(); i++) {
				//if (contourArea(contours[i], false) > 50)//ȱ�ݵ�������С���
				//{
				Rect rect = boundingRect(Mat(contours[i]));
				Rect rect_out = Rect(rect.x - 5, rect.y, rect.width + 10, rect.height + 10);
				Mat temp_area = templ_gray(rect_out);
				Mat samp_area = samp_gray(rect_out);
				Mat mask;
				//double res = computeECC(temp_area, samp_area,mask);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
				auto res = detectFunc->getMSSIM(temp_area, samp_area);
				//qDebug() << QString::fromLocal8Bit("===========���ϵ��") << res;
				if (res[0] >= 0.5)
					continue;
				//Mat result;
				//result.create(1, 1, CV_32FC1);
				//matchTemplate(temp_area, samp_area, result, TM_SQDIFF_NORMED);
				//double minVal; double maxVal; Point minLoc; Point maxLoc;
				//Point matchLoc;
				//minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

				putText(diff, to_string(res[0]), Point(rect.x, rect.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);//��ͼƬ��д����
				rectangle(diff, rect_out, Scalar(0, 255, 0));
				drawContours(diff, contours, i, cv::Scalar(0, 0, 255), 2, 8);
				//}

			}

			string name_head = to_string(detectParams->currentRow_detect + 1) + "_" + to_string(i + 1);
			cv::imwrite("D:\\tr_project\\project\\pcb_detect_new\\detectfunc_test_res\\res_05\\diff_new\\" + name_head + "_5diff.jpg", diff);
			cv::imwrite("D:\\tr_project\\project\\pcb_detect_new\\detectfunc_test_res\\res_05\\diff_new\\" + name_head + "_2templBw.jpg", templBw);
			cv::imwrite("D:\\tr_project\\project\\pcb_detect_new\\detectfunc_test_res\\res_05\\diff_new\\" + name_head + "_4sampBw.jpg", sampBw);
			cv::imwrite("D:\\tr_project\\project\\pcb_detect_new\\detectfunc_test_res\\res_05\\diff_new\\" + name_head + "_1templ_gray.jpg", templ_gray);
			cv::imwrite("D:\\tr_project\\project\\pcb_detect_new\\detectfunc_test_res\\res_05\\diff_new\\" + name_head + "_3samp_gray_reg.jpg", samp_gray_reg);


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
	pcb::delay(10);
	bool qualified = (defectNum < 1);
	emit detectFinished_detectThread(qualified);
}
