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

//���в�ͬ�汾��ѹ����֪������
/*����Real-Time Compressive Tracking �汾�ĸ�������ʹ��ͼƬ����*/
void RTCompTrackSque();

/*����Real-Time Compressive Tracking �汾�ĸ�������ʹ������ͷ*/
int RTCompTrackCamf(int argc, char * argv[]);