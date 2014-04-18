#pragma once
#ifndef INITILISE_H
#define INITILISE_H

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

using namespace cv;
using namespace std;

/* Get Video Method */
CvCapture* GetVideo();  


CvCapture* GetVideo()
{
	CvCapture* capture = NULL;
	string video;                     /* input video file name */
	const char* filename;
	cout<<"Input vedio path and name."<<endl;
	cin>>video;
	filename=video.data();
	/*capture.open(video);*/
	capture = cvCaptureFromFile(filename);
	while (capture==NULL)//(!capture.isOpened())
	{
		cout<<"Please input path and name of original avi file. Extention required. Input q to exit."<<endl;
	    cout<<"e.g. \"F:\\test\\test.avi\""<<endl;
		cin>>video;
		if (video=="q") return NULL;
		filename=video.data();
		capture = cvCaptureFromFile(filename);//capture.open(video);
	}
	return capture;
}

#endif
