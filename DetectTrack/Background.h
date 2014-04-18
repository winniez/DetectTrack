#pragma once
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "defs.h"
#include "Rectangles.h"

// ----------------------- Initialize IplConvKernel used in foreground extraction -------------------------
//void KernalInitiate(IplConvKernel *element1s,IplConvKernel *element1l,IplConvKernel *element2s,IplConvKernel *element2l,
//	IplConvKernel *element3s,IplConvKernel *element3l,IplConvKernel *element4s,IplConvKernel *element4l);

// ---------------------- Background class --------------------- //
class BackGround
{
private:
	IplImage *tempframe11;
	IplImage *tempframe12;
	IplImage *tempframe13;
	IplImage *tempframe21;
	IplImage *tempframe22;
	IplImage *tempframe23;
	CvMat *MatBkgST;
	CvMat *MatBkgLT;
	CvMat *tempMat1;
	CvMat *tempMat2;
	IplConvKernel *Selement;
	IplConvKernel *Lelement;
	IplConvKernel *element;
	CvGaussBGStatModelParams* params;
	CvGaussBGModel* bg_model;
	CvSize oriSize;

public:
	IplImage *pFrImg_shortterm;
	IplImage *pBkgImg_shortterm;
	IplImage *pFrImg_longterm;
	IplImage *pBkgImg_longterm;
	IplImage *pBkgImg_Gaussian;
	IplImage *pFrImg_Gaussian;
	bool GauISright;

	BackGround();
	~BackGround();
	void BackGround::SetElement(IplConvKernel *elementtempS,IplConvKernel *elementtempL);
	void SetGauParams(CvGaussBGStatModelParams* tempps);
	void Initi(IplImage *pframe);
	void Update(IplImage *pframe);

};

#endif