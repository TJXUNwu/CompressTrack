/************************************************************************
* File:	RunTracker.cpp
* Brief: C++ demo for paper: Kaihua Zhang, Lei Zhang, Ming-Hsuan Yang,"Real-Time Compressive Tracking," ECCV 2012.
* Version: 1.0
* Author: Yang Xian
* Email: yang_xian521@163.com
* Date:	2012/08/03
* History:
* Revised by Kaihua Zhang on 14/8/2012, 23/8/2012
* Email: zhkhua@gmail.com
* Homepage: http://www4.comp.polyu.edu.hk/~cskhzhang/
* Project Website: http://www4.comp.polyu.edu.hk/~cslzhang/CT/CT.htm
************************************************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include"DiffCompressTrack.h"

using namespace cv;
using namespace std;


int main(int argc, char * argv[])
{
	RTCompTrackSque();
	//RTCompTrackCamf(argc, argv);
	cout << " run over ..."<<endl;
	getchar();
	return 0;
}
