//#include <opencv2/opencv.hpp>
//#include <cv.h>
//#include <highgui.h>
//#include <cxcore.h>
#pragma once
#ifndef DIRECTION_H
#define DIRECTION_H

#include "defs.h"

using namespace cv;

CvSize2D32f CalculateDirection(IplImage* lst,IplImage *cur,IplImage *mask);


#endif