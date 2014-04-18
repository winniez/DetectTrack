/*
  This file contains general program definitions.
  
  @author Rob Hess
  @version 1.0.0-20060306
*/
#pragma once
#ifndef DEFS_H
#define DEFS_H

/********************************* Includes **********************************/

/* From standard C library */
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>
#include <istream>
//#include <unistd.h>  //TYS_A

/* From OpenCV library */
#include <cv.h>  //TYS_A  ""-><>
#include <opencv2/opencv.hpp>
#include <highgui.h>
#include <cxcore.h>

#include "RectList.h"
/* From GSL */
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


/******************************* Defs and macros *****************************/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MIN
#define MIN(x,y) ( ( x < y )? x : y )
#endif
#ifndef MAX
#define MAX(x,y) ( ( x > y )? x : y )
#endif
#ifndef ABS
#define ABS(x) ( ( x < 0 )? -x : x )
#endif
#ifndef CONTOUR_MIN_AERA
#define CONTOUR_MIN_AERA 300
#endif
#ifndef CONTOUR_MAX_AERA
#define CONTOUR_MAX_AERA 50000
#endif


/********************************** Structures *******************************/

#endif
