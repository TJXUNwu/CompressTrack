#include"ReadFile.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATH 260
using std::vector;
using namespace cv;
using namespace std;







void readConfig(char* configFileName, char* imgFilePath, Rect &box, int &num)
{
	int x;
	int y;
	int w;
	int h;
	int nums;
	fstream f;
	char cstring[1000];
	int readS = 0;

	f.open(configFileName, fstream::in);

	char param1[200]; strcpy(param1, "");
	char param2[200]; strcpy(param2, "");
	char param3[200]; strcpy(param3, "");

	f.getline(cstring, sizeof(cstring));
	readS = sscanf(cstring, "%s %s %s", param1, param2, param3);

	strcpy(imgFilePath, param3);

	f.getline(cstring, sizeof(cstring));
	f.getline(cstring, sizeof(cstring));
	//f.getline(cstring, sizeof(cstring));

	readS = sscanf(cstring, "%s %s %i %i %i %i %i", param1, param2, &x, &y, &w, &h, &nums);

	box = Rect(x, y, w, h);
	num = nums;
	f.close();
}

void readImageSequenceFiles(char* imgFilePath, vector <string> &imgNames, int &num)
{
	imgNames.clear();
	/*
	char tmpDirSpec[MAX_PATH+1];
	sprintf (tmpDirSpec, "%s/*", imgFilePath);

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(tmpDirSpec , &f);
	if(h != INVALID_HANDLE_VALUE)
	{
	FindNextFile(h, &f);	//read ..
	FindNextFile(h, &f);	//read .
	do
	{
	imgNames.push_back(f.cFileName);
	} while(FindNextFile(h, &f));

	}
	FindClose(h);
	*/

	String sequencesName = "%04d.jpg";
	for (int i = 1; i <= num; i++)
	{
		char imgName[256];
		sprintf(imgName, sequencesName.c_str(), i);
		String name = imgName;
		imgNames.push_back(name);
		//cout<<"the name of this frame is "<<name<<endl;  
	}
}





