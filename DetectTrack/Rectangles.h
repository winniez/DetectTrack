#pragma once
#ifndef RECTANGLES_H
#define RECTANGLES_H
#include "defs.h"
#include "RectList.h"
#include "AllRect.h"


/* outer margin */
const int OUTTER_MARGIN = 35;

/* to merge contour color difference max of three channels */
const int COLOR_DIFF_MAX = 50;
/* to merge rect distance */
const int BORDER_DISTANCE_MAX = 50;
/* to merge rect side difference */
const int SIDE_DIFF_MAX = 70;
/* smaller rect area thresh */
const int SMALL_AREA_THRESH = 3000;
/* bigger rect area thresh */
const int BIG_AREA_THRESH = 34000;
// STREET3 SMALL_AREA_THRESH = 2500 BIG_AREA_THRESH = 10000
// STREET2 SMALL_AREA_THRESH = 8000 BIG_AREA_THRESH = 35000
// STREET1 SMALL_AREA_THRESH = 3000 BIG_AREA_THRESH = 34000

/*********************************** Structure ********************************/
typedef struct matchrectinfo
{
	int id;
	int overlapa;
	CvRect rr;

}matchrectinfo;


/***************************** Function Declaration ***************************/
// Foreground and tracking match and manage
void ObjectMatch(AllRectList *objects, RectList *motiondetect,long int *ObjID,CvSize ssize,matchrectinfo *MRinfo);

// Get Outter Rectangles
RectList* OutterRects(IplImage *frame, IplImage *tempf,int Thresh);
/* Get Specific Foreground */
void GetCleanFrGnd(IplImage *src, IplImage *dst,IplImage *tempf);
/* Get Specific Foreground and Rectangles*/
RectList* GetCleanFrAndRects(IplImage *src, IplImage *dst,IplImage *tempf);

// Match
int Match2Rect(CvRect r1, CvRect r2);                                /* Calculate Trust Level */ 
int OverLapArea(CvRect r1, CvRect r2);                               /* Calculate Overlap Area */
// Draw Retangle on frame
void DrawRectID(IplImage* Img, RectList *rect, CvScalar color);      /* Draw Rects and ID*/
void DrawRect(IplImage* Img, RectList *rect, CvScalar color);
bool EnterInner(CvRect rc,CvSize size);
bool NearInner(CvRect rc, CvSize size);
bool NearOutter(CvRect rc, CvSize size);
bool CarLikeWH(CvRect rc, int direction);
bool CarLikeArea(CvRect rc);
double SimilarSize(CvRect r1, CvRect r2);
bool SameObject(CvRect r1, CvRect r2, int direction);

CvRect Merge2Rect(CvRect r1, CvRect r2);

#endif