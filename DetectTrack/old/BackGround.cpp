#include "Background.h"

BackGround::BackGround()
{
	oriSize = cvSize(1,1);
	tempframe11 = NULL;
	tempframe12 = NULL;
	tempframe13 = NULL;
	tempframe21 = NULL;
	tempframe22 = NULL;
	tempframe23 = NULL;

	pFrImg_shortterm = NULL;
	pBkgImg_shortterm = NULL;
	pFrImg_longterm = NULL;
	pBkgImg_longterm = NULL;
	pFrImg_Gaussian = NULL;
	pBkgImg_Gaussian = NULL;


	MatBkgST = NULL;
	MatBkgLT = NULL;
	tempMat1 = NULL;
	tempMat2 = NULL;
	//moving detection related
	Selement = NULL;
	Lelement = NULL;
	//GMM related
	params = NULL;
	bg_model = NULL;
	GauISright = true;
}

BackGround::~BackGround()
{
	if (tempframe11 != NULL){cvReleaseImage(&tempframe11);}
	if (tempframe12 != NULL){cvReleaseImage(&tempframe12);}
	if (tempframe13 != NULL){cvReleaseImage(&tempframe13);}
	if (tempframe21 != NULL){cvReleaseImage(&tempframe21);}
	if (tempframe22 != NULL){cvReleaseImage(&tempframe22);}
	if (tempframe23 != NULL){cvReleaseImage(&tempframe23);}
	if (pFrImg_shortterm != NULL){cvReleaseImage(&pFrImg_shortterm);}
	if (pBkgImg_shortterm != NULL){cvReleaseImage(&pBkgImg_shortterm);}
	if (pFrImg_longterm != NULL){cvReleaseImage(&pFrImg_longterm);}
	if (pBkgImg_longterm != NULL){cvReleaseImage(&pBkgImg_longterm);}
	if (pBkgImg_Gaussian != NULL) {cvReleaseImage(&pBkgImg_Gaussian);}
	if (pFrImg_Gaussian != NULL) {cvReleaseImage(&pFrImg_Gaussian);}
	if (MatBkgST != NULL) {cvReleaseMat(&MatBkgST);}
	if (MatBkgLT != NULL) {cvReleaseMat(&MatBkgLT);}
	if (tempMat1 != NULL) {cvReleaseMat(&tempMat1);}
	if (tempMat2 != NULL) {cvReleaseMat(&tempMat2);}

	if (Selement != NULL) {cvReleaseStructuringElement(&Selement);}
	if (Lelement != NULL) {cvReleaseStructuringElement(&Lelement);}
	if (params != NULL) {delete params;}
	if (bg_model != NULL) { cvReleaseBGStatModel((CvBGStatModel**)&bg_model); }
}

void BackGround::SetElement(IplConvKernel *elementtempS,IplConvKernel *elementtempL)
{
	if (Selement == NULL)
	{Selement = cvCreateStructuringElementEx(3,3,2,2,CV_SHAPE_RECT,0);}
	if (Lelement == NULL)
	{Lelement = cvCreateStructuringElementEx(5,5,3,3,CV_SHAPE_RECT,0);}
	Selement->anchorX=elementtempS->anchorX;
	Selement->anchorY=elementtempS->anchorY;
	Selement->nCols=elementtempS->nCols;
	Selement->nRows = elementtempS->nRows;
	Selement->nShiftR = elementtempS->nShiftR;
	Selement->values = elementtempS->values;

	Lelement->anchorX=elementtempL->anchorX;
	Lelement->anchorY=elementtempL->anchorY;
	Lelement->nCols=elementtempL->nCols;
	Lelement->nRows = elementtempL->nRows;
	Lelement->nShiftR = elementtempL->nShiftR;
	Lelement->values = elementtempL->values;
}

void BackGround::SetGauParams(CvGaussBGStatModelParams* tempps)
{
	if (params == NULL) 
	{params = new CvGaussBGStatModelParams;}
	if (tempps == NULL)
	{
		params->win_size = 25000; 
		params->n_gauss = 5;
		params->bg_threshold = 0.7;
		params->std_threshold = 2.5;
		params->minArea = 15;
		params->weight_init = 0.05;
		params->variance_init = 30;
	}
	else
	{
		params->win_size = tempps->win_size;
		params->n_gauss = tempps->n_gauss;
		params->bg_threshold = tempps->bg_threshold;
		params->std_threshold = tempps->std_threshold;
		params->minArea = tempps->minArea;
		params->weight_init = tempps->weight_init;
		params->variance_init = tempps->variance_init;
	}
}

void BackGround::Initi(IplImage *pframe)
{
	if (pframe != NULL)
	{
		oriSize.height=pframe->height;
		oriSize.width=pframe->width;
		if (tempframe11 != NULL){cvReleaseImage(&tempframe11);}
		if (tempframe12 != NULL){cvReleaseImage(&tempframe12);}
		if (tempframe13 != NULL){cvReleaseImage(&tempframe13);}
		if (tempframe21 != NULL){cvReleaseImage(&tempframe21);}
		if (tempframe22 != NULL){cvReleaseImage(&tempframe22);}
		if (tempframe23 != NULL){cvReleaseImage(&tempframe23);}
		if (pFrImg_shortterm != NULL){cvReleaseImage(&pFrImg_shortterm);}
		if (pBkgImg_shortterm != NULL){cvReleaseImage(&pBkgImg_shortterm);}
		if (pFrImg_longterm != NULL){cvReleaseImage(&pFrImg_longterm);}
		if (pBkgImg_longterm != NULL){cvReleaseImage(&pBkgImg_longterm);}
		if (pBkgImg_Gaussian != NULL) {cvReleaseImage(&pBkgImg_Gaussian);}
		if (pFrImg_Gaussian != NULL) {cvReleaseImage(&pFrImg_Gaussian);}
		if (MatBkgST != NULL) {cvReleaseMat(&MatBkgST);}
		if (MatBkgLT != NULL) {cvReleaseMat(&MatBkgLT);}
		if (tempMat1 != NULL) {cvReleaseMat(&tempMat1);}
		if (tempMat2 != NULL) {cvReleaseMat(&tempMat2);}
		tempframe11 = cvCreateImage(oriSize, IPL_DEPTH_8U,1);
		tempframe12 = cvCreateImage(oriSize, IPL_DEPTH_8U,1);
		tempframe13 = cvCreateImage(oriSize, IPL_DEPTH_8U,1);
		tempframe21 = cvCreateImage(oriSize, IPL_DEPTH_8U,1);
		tempframe22 = cvCreateImage(oriSize, IPL_DEPTH_8U,1);
		tempframe23 = cvCreateImage(oriSize, IPL_DEPTH_8U,1);

		pBkgImg_shortterm = cvCreateImage(oriSize,IPL_DEPTH_8U,1);
		pFrImg_shortterm = cvCreateImage(oriSize,IPL_DEPTH_8U,1);

		pBkgImg_Gaussian = cvCreateImage(oriSize,IPL_DEPTH_8U,3);
		pFrImg_Gaussian = cvCreateImage(oriSize,IPL_DEPTH_8U,1); 

		MatBkgST = cvCreateMat(oriSize.height,oriSize.width, CV_32FC1);
		MatBkgLT = cvCreateMat(oriSize.height,oriSize.width, CV_32FC1);
		tempMat1 = cvCreateMat(oriSize.height,oriSize.width, CV_32FC1);
		tempMat2 = cvCreateMat(oriSize.height,oriSize.width, CV_32FC1);

		if (pframe->nChannels==3)
		{cvCvtColor(pframe,pBkgImg_shortterm, CV_BGR2GRAY);}
		else 
		{cvCopyImage(pframe,pBkgImg_shortterm);}
		//build gaussian
		bg_model = (CvGaussBGModel*)cvCreateGaussianBGModel(pframe, params);
	}
}

void BackGround::Update(IplImage *pframe)
{
	RectList *GauList = new RectList();
	// Gaussian background
	cvUpdateBGStatModel(pframe, (CvBGStatModel *)bg_model );
    //pFrImg为前景图像，只能为单通道
    //pBkImg为背景图像，可以为单通道或与pFrame通道数相同
    cvCopy(bg_model->foreground,pFrImg_Gaussian,0);
    cvCopy(bg_model->background,pBkgImg_Gaussian,0);
    //把图像正过来
    pBkgImg_Gaussian->origin=0;
    pFrImg_Gaussian->origin=0;
	cvDilate(pFrImg_Gaussian,tempframe11,0,2);
	cvErode(tempframe11,tempframe11,0,1);
	cvErode(tempframe11,tempframe11,0,2);
	cvDilate(tempframe11,tempframe11,0,2);
	cvCopyImage(tempframe11,pFrImg_Gaussian);
	// check gaussian background
	//Get bigger Contour
	GauList = OutterRects(pFrImg_Gaussian,tempframe12);
	if (GauList->GetLength()==0)
	{element = Selement;}
	else
	{element = Lelement; }
	CvRect tempr = cvRect(0,0,0,0);
	RectNode *ptr1;
	double area=0;
	ptr1=GauList->head;
	while(ptr1->next!=NULL)
	{
		ptr1=ptr1->next;
		tempr=ptr1->GetRect();
		area=tempr.height*tempr.width;
		if (area>60000)
		{
			element = Selement;GauISright= false;
			break;
		}
	}

	// short term background
	if (pframe->nChannels==3)
	{cvCvtColor(pframe,tempframe21,CV_BGR2GRAY);}
	
	cvSmooth(tempframe21, tempframe21, CV_GAUSSIAN, 7, 0, 0); //pFrame2 for motion detect 2
	cvConvert(tempframe21,tempMat1);
	cvAbsDiff(tempMat1,MatBkgST,tempMat2);
	cvThreshold(tempMat2,tempframe22,20,255, CV_THRESH_BINARY);;

	cvErode(tempframe22,tempframe22,0,1);
	cvDilate(tempframe22,tempframe22,0,1);

	cvDilate(tempframe22,tempframe22,element,2);
	cvErode(tempframe22,tempframe22,0,4);
	cvCopyImage(tempframe22,pFrImg_shortterm);
	
	//refresh short term background
	cvRunningAvg(tempMat1,MatBkgST,0.75,0);
	cvConvert(MatBkgST,pBkgImg_shortterm);
	delete GauList;
}