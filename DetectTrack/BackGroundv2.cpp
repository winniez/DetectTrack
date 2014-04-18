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
	// average foreground pic color
	CvScalar GauColor, STColor;
	double GauGrey=0, STGrey=0;
	double ratio = 0;
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
	GauColor = cvAvg(pFrImg_Gaussian);
	GauGrey = GauColor.val[0];
	//Get bigger Contour
	/*GauList = OutterRects(pFrImg_Gaussian,tempframe12);
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
	}*/

	// short term background
	if (pframe->nChannels==3)
	{cvCvtColor(pframe,tempframe21,CV_BGR2GRAY);}
	
	cvSmooth(tempframe21, tempframe21, CV_GAUSSIAN, 7, 0, 0); //pFrame2 for motion detect 2
	cvConvert(tempframe21,tempMat1);
	cvAbsDiff(tempMat1,MatBkgST,tempMat2);
	cvThreshold(tempMat2,tempframe22,20,255, CV_THRESH_BINARY);;

	cvErode(tempframe22,tempframe22,0,1);
	cvDilate(tempframe22,tempframe22,0,1);

	cvDilate(tempframe22,tempframe23,Selement,2);
	cvErode(tempframe23,tempframe23,0,2);
	STColor = cvAvg(tempframe23);
	STGrey = STColor.val[0];
	
	// Check if Gaussian Background is right
	if (GauGrey>=120)
	{// Gaussian is not right
		cvDilate(tempframe22,tempframe23,Lelement,2);
	    cvErode(tempframe23,tempframe23,0,2);
		GauISright= false;
	}
	else
	{GauISright= true;}
	cvCopyImage(tempframe23,pFrImg_shortterm);

	//refresh short term background
	cvRunningAvg(tempMat1,MatBkgST,0.75,0);
	cvConvert(MatBkgST,pBkgImg_shortterm);
}

// ----------------------- Initialize IplConvKernel used in foreground extraction START -------------------------
//void KernalInitiate(IplConvKernel *element1s,IplConvKernel *element1l,IplConvKernel *element2s,IplConvKernel *element2l,
//	IplConvKernel *element3s,IplConvKernel *element3l,IplConvKernel *element4s,IplConvKernel *element4l)
//{//IplConvKernel for 4 directions, each direction has a larger element and smaller emelemt
//	//4  3   2  
//	//\  |  /
//	// \ | /
//	//------- 1  
//	// / | \
//	///  |  \
//	//4  3   2
//	element3l=cvCreateStructuringElementEx(4,7,2,4,CV_SHAPE_RECT,0);
//	element3s=cvCreateStructuringElementEx(3,6,2,3,CV_SHAPE_RECT,0);
//
//	element1l=cvCreateStructuringElementEx(7,5,4,3,CV_SHAPE_RECT,0);
//	element1s=cvCreateStructuringElementEx(6,4,2,2,CV_SHAPE_RECT,0);
//	// direction 2
//	int* values1 = new int[81];
//	values1[0]=0; values1[1]=0; values1[2]=0; values1[3]=0; values1[4]=0; values1[5]=0; values1[6]=0; values1[7]=1; values1[8]=1;
//	values1[9]=0; values1[10]=0;values1[11]=0;values1[12]=0;values1[13]=1;values1[14]=1;values1[15]=1;values1[16]=1;values1[17]=1;
//	values1[18]=0;values1[19]=0;values1[20]=1;values1[21]=1;values1[22]=1;values1[23]=1;values1[24]=1;values1[25]=1;values1[26]=0;
//	values1[27]=0;values1[28]=0;values1[29]=1;values1[30]=1;values1[31]=1;values1[32]=1;values1[33]=1;values1[34]=1;values1[35]=0;
//	values1[36]=0;values1[37]=0;values1[38]=1;values1[39]=1;values1[40]=1;values1[41]=1;values1[42]=1;values1[43]=0;values1[44]=0;
//	values1[45]=0;values1[46]=1;values1[47]=1;values1[48]=1;values1[49]=1;values1[50]=1;values1[51]=1;values1[52]=0;values1[53]=0;
//	values1[54]=0;values1[55]=1;values1[56]=1;values1[57]=1;values1[58]=1;values1[59]=1;values1[60]=1;values1[61]=0;values1[62]=0;
//	values1[63]=1;values1[64]=1;values1[65]=1;values1[66]=1;values1[67]=0;values1[68]=0;values1[69]=0;values1[70]=0;values1[71]=0;
//	values1[72]=1;values1[73]=1;values1[74]=0;values1[75]=0;values1[76]=0;values1[77]=0;values1[78]=0;values1[79]=0;values1[80]=0;
//	
//	int *values2 = new int[36];
//	values2[0]=0; values2[1]=0; values2[2]=0; values2[3]=0; values2[4]=1; values2[5]=1;
//	values2[6]=0; values2[7]=0; values2[8]=1; values2[9]=1; values2[10]=1;values2[11]=1;
//	values2[12]=0;values2[13]=1;values2[14]=1;values2[15]=1;values2[16]=1;values2[17]=0;
//	values2[18]=0;values2[19]=1;values2[20]=1;values2[21]=1;values2[22]=1;values2[23]=0;
//	values2[24]=1;values2[25]=1;values2[26]=1;values2[27]=1;values2[28]=0;values2[29]=0;
//	values2[30]=1;values2[31]=1;values2[32]=0;values2[33]=0;values2[34]=0;values2[35]=0;
//
//	element2l=cvCreateStructuringElementEx(9,9,5,5,CV_SHAPE_CUSTOM,values1);
//	element2s=cvCreateStructuringElementEx(6,6,3,3,CV_SHAPE_CUSTOM,values2);
//	// direction 4
//	values1[0]=1; values1[1]=1; values1[2]=0; values1[3]=0; values1[4]=0; values1[5]=0; values1[6]=0; values1[7]=0; values1[8]=0;
//	values1[9]=1; values1[10]=1;values1[11]=1;values1[12]=1;values1[13]=0;values1[14]=0;values1[15]=0;values1[16]=0;values1[17]=0;
//	values1[18]=0;values1[19]=1;values1[20]=1;values1[21]=1;values1[22]=1;values1[23]=1;values1[24]=1;values1[25]=0;values1[26]=0;
//	values1[27]=0;values1[28]=1;values1[29]=1;values1[30]=1;values1[31]=1;values1[32]=1;values1[33]=1;values1[34]=0;values1[35]=0;
//	values1[36]=0;values1[37]=0;values1[38]=1;values1[39]=1;values1[40]=1;values1[41]=1;values1[42]=1;values1[43]=0;values1[44]=0;
//	values1[45]=0;values1[46]=0;values1[47]=1;values1[48]=1;values1[49]=1;values1[50]=1;values1[51]=1;values1[52]=1;values1[53]=0;
//	values1[54]=0;values1[55]=0;values1[56]=1;values1[57]=1;values1[58]=1;values1[59]=1;values1[60]=1;values1[61]=1;values1[62]=0;
//	values1[63]=0;values1[64]=0;values1[65]=0;values1[66]=0;values1[67]=1;values1[68]=1;values1[69]=1;values1[70]=1;values1[71]=1;
//	values1[72]=0;values1[73]=0;values1[74]=0;values1[75]=0;values1[76]=0;values1[77]=0;values1[78]=0;values1[79]=1;values1[80]=1;
//	
//	values2[0]=1; values2[1]=1; values2[2]=0; values2[3]=0; values2[4]=0; values2[5]=0;
//	values2[6]=1; values2[7]=1; values2[8]=1; values2[9]=1; values2[10]=0;values2[11]=0;
//	values2[12]=0;values2[13]=1;values2[14]=1;values2[15]=1;values2[16]=1;values2[17]=0;
//	values2[18]=0;values2[19]=1;values2[20]=1;values2[21]=1;values2[22]=1;values2[23]=0;
//	values2[24]=0;values2[25]=0;values2[26]=1;values2[27]=1;values2[28]=1;values2[29]=1;
//	values2[30]=0;values2[31]=0;values2[32]=0;values2[33]=0;values2[34]=1;values2[35]=1;
//	element4l=cvCreateStructuringElementEx(9,9,5,5,CV_SHAPE_CUSTOM,values1);
//	element4s=cvCreateStructuringElementEx(6,6,3,3,CV_SHAPE_CUSTOM,values2);
//}
// ----------------------- Initialize IplConvKernel used in foreground extraction END -------------------------