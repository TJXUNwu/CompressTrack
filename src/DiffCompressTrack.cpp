#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include"ReadFile.h"
#include "CompressiveTracker.h"
#include"DiffCompressTrack.h"

#define MAX_PATH 260

using namespace cv;
using namespace std;

Rect box;

/*����Real-Time Compressive Tracking �汾�ĸ�������ʹ��ͼƬ����*/
void RTCompTrackSque()
{
	double t = (double)cvGetTickCount();

	char imgFilePath[100];//���ͼƬ�ļ���·��
	char  conf[100];//�����ļ�
	strcpy(conf, "../config/config.txt");

	char tmpDirPath[MAX_PATH + 1];//���ͼƬȫ·�����м����

	//Rect box; // [x y width height] tracking position
	int num;//ͼƬ������ͼƬ�ĸ���
	vector <string> imgNames;//ͼƬ���Ƽ���

	//��ȡ��Ƶ֡��������Ϣ
	readConfig(conf, imgFilePath, box, num);

	//��ÿһ֡�����Ʒ�������imgNames//num������ͼƬ֡�ĸ����������������ʹ��
	readImageSequenceFiles(imgFilePath, imgNames, num);

	// CT framework
	CompressiveTracker ct;

	Mat frame;
	Mat grayImg;

	sprintf(tmpDirPath, "%s/", imgFilePath);
	imgNames[0].insert(0, tmpDirPath);//��ȡ��һ֡ͼƬ��ȫ·��
	frame = imread(imgNames[0]);//��ȡ��һ֡ͼƬ
	cvtColor(frame, grayImg, CV_RGB2GRAY); //ת���ɻҶ�ͼ
	ct.init(grayImg, box); //ͨ����һ֡��ʼ���������Ȳ���

	char strFrame[20];//������ʾ֡��

	FILE* resultStream;//������
	resultStream = fopen("../result/TrackingResults.txt", "w");
	fprintf(resultStream, "%i %i %i %i\n", (int)box.x, (int)box.y, (int)box.width, (int)box.height);

	//����֮���ÿһ֡ͼƬ
	int i = 1;
	for (; i < imgNames.size() - 1; i++)
	{
		//sprintf(tmpDirPath, "%s/", imgFilePath);
		imgNames[i].insert(0, tmpDirPath);

		frame = imread(imgNames[i]);// get frame
		cvtColor(frame, grayImg, CV_RGB2GRAY);

		ct.processFrame(grayImg, box);//Process frame//����ͼƬ֡

		rectangle(frame, box, Scalar(200, 0, 0), 2);// Draw rectangle

		fprintf(resultStream, "%i %i %i %i\n", (int)box.x, (int)box.y, (int)box.width, (int)box.height);

		sprintf(strFrame, "#%d ", i);

		putText(frame, strFrame, cvPoint(0, 20), 2, 1, CV_RGB(25, 200, 25));

		imshow("CT", frame);// Display
		waitKey(1);
	}
	fclose(resultStream);

	t = (double)cvGetTickCount() - t;
	printf(" -- run time = %gms\n", t / (cvGetTickFrequency() *1000));
	float FPS = imgNames.size() / (t / (cvGetTickFrequency()*1000000));
	cout << "The FPS of CT is : " << FPS << endl;
	
}


/*����Real-Time Compressive Tracking �汾�ĸ�������ʹ������ͷ*/
int RTCompTrackCamf(int argc, char * argv[])
{
	
	return 1;
}


