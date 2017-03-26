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

/*运行Real-Time Compressive Tracking 版本的跟踪器，使用图片序列*/
void RTCompTrackSque()
{
	double t = (double)cvGetTickCount();

	char imgFilePath[100];//存放图片文件的路径
	char  conf[100];//配置文件
	strcpy(conf, "../config/config.txt");

	char tmpDirPath[MAX_PATH + 1];//存放图片全路径的中间变量

	//Rect box; // [x y width height] tracking position
	int num;//图片序列中图片的个数
	vector <string> imgNames;//图片名称集合

	//读取视频帧的配置信息
	readConfig(conf, imgFilePath, box, num);

	//将每一帧的名称放入数组imgNames//num是输入图片帧的个数，这里仅作测试使用
	readImageSequenceFiles(imgFilePath, imgNames, num);

	// CT framework
	CompressiveTracker ct;

	Mat frame;
	Mat grayImg;

	sprintf(tmpDirPath, "%s/", imgFilePath);
	imgNames[0].insert(0, tmpDirPath);//读取第一帧图片的全路径
	frame = imread(imgNames[0]);//读取第一帧图片
	cvtColor(frame, grayImg, CV_RGB2GRAY); //转换成灰度图
	ct.init(grayImg, box); //通过第一帧初始化分类器等参数

	char strFrame[20];//用于显示帧数

	FILE* resultStream;//保存结果
	resultStream = fopen("../result/TrackingResults.txt", "w");
	fprintf(resultStream, "%i %i %i %i\n", (int)box.x, (int)box.y, (int)box.width, (int)box.height);

	//处理之后的每一帧图片
	int i = 1;
	for (; i < imgNames.size() - 1; i++)
	{
		//sprintf(tmpDirPath, "%s/", imgFilePath);
		imgNames[i].insert(0, tmpDirPath);

		frame = imread(imgNames[i]);// get frame
		cvtColor(frame, grayImg, CV_RGB2GRAY);

		ct.processFrame(grayImg, box);//Process frame//处理图片帧

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


/*运行Real-Time Compressive Tracking 版本的跟踪器，使用摄像头*/
int RTCompTrackCamf(int argc, char * argv[])
{
	
	return 1;
}


