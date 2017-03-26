#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>


using std::vector;
using namespace cv;
/*  Description: read the tracking information from file "config.txt"
Arguments:
-configFileName: config file name
-ImgFilePath:    Path of the storing image sequences
-box:            [x y width height] intial tracking position
-num:  number of  the image sequence
*/
void readConfig(char* configFileName, char* imgFilePath, Rect &box, int &num);


/*  Description: search the image names in the image sequences
Arguments:
-ImgFilePath: path of the image sequence
-imgNames:  vector that stores image name
-num:  number of  the image sequence
*/
void readImageSequenceFiles(char* ImgFilePath, vector <string> &imgNames, int &num);

