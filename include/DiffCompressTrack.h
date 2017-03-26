#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include"ReadFile.h"
#include "CompressiveTracker.h"
#define MAX_PATH 260
using namespace cv;
using namespace std;

//运行不同版本的压缩感知跟踪器
/*运行Real-Time Compressive Tracking 版本的跟踪器，使用图片序列*/
void RTCompTrackSque();

/*运行Real-Time Compressive Tracking 版本的跟踪器，使用摄像头*/
int RTCompTrackCamf(int argc, char * argv[]);