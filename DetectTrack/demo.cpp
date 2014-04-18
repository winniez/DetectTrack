//#define GSL_DLL  //Properties->C/C++->Command Line->additional options->/D "GSL_DLL" 
#include "defs.h" //TYS_remove 

#include "utils.h"
#include "ParticalItem.h"
#include "initilise.h"
#include "Direction.h"
#include "Background.h"

#include "List.h"
#include "AllRect.h"

using namespace cv;
using namespace std;

/******************************** Definitions ********************************/

/* command line options */
#define OPTIONS ":p:oah"

//!!!!!!!!!!!!!!!!!!!!!!!!DIFFERENT BY VEDIO!!!!!!!!!!!!!!!!!//
/* smaller rect area thresh */
const int SMALL_AREA_THRESH1 = 2400;
/* bigger rect area thresh */
const int BIG_AREA_THRESH1 = 14000;
// STREET3 SMALL_AREA_THRESH = 2400 BIG_AREA_THRESH = 14000
// STREET2 SMALL_AREA_THRESH = 8000 BIG_AREA_THRESH = 50000
// STREET1 SMALL_AREA_THRESH = 3000 BIG_AREA_THRESH = 70000
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

/***************************** Function Prototypes ***************************/                                                     /* Get vedio */     
// Motion Area Detect Motion Detect module, first frame, initiate
void ImageIni(CvSize ssize);                                                        /* Motion Area Initiation */
void Release();  

IplImage* hsv_frame = NULL; 
IplImage* lst_hsv_frame = NULL;
IplImage *pFrame1 = NULL; //Get from orignal vedio
IplImage *pFrame2 = NULL;
IplImage *pFrame3 = NULL;
IplImage *pFrame4 = NULL;
IplImage *pFrame5 = NULL;
IplImage *STFr =NULL;
IplImage *ReSTFr =NULL;
IplImage *GauFr =NULL;
IplImage *ReGauFr =NULL;
IplImage *lstf = NULL;
IplImage *curf = NULL;
IplImage *combine1 = NULL;
IplImage *combine2 = NULL;
IplImage *maskroi = NULL;

int main()
{
	// -------------------------------- General Initialize START --------------------------------
	// vedio and show related
	int lowersize = 2200;
	CvSize2D32f dire2 = cvSize2D32f(1,1);
	CvCapture* pCapture = NULL;
	pCapture=GetVideo();
	if (pCapture==NULL)
	{return 0;}

	//write video
	pFrame1 = cvQueryFrame(pCapture);
	CvSize ssize = cvSize(pFrame1->width,pFrame1->height);

	ImageIni(ssize);
	lstf=cvCloneImage(pFrame1);
	curf=cvCloneImage(pFrame1);
	pFrame2=cvCloneImage(pFrame1);

	double fps = cvGetCaptureProperty (pCapture,CV_CAP_PROP_FPS);
	fps=fps-3;
	cout<<"fps = "<<fps<<endl;
	CvVideoWriter *twriter = cvCreateVideoWriter( "s3motion.track.avi", //name
        CV_FOURCC('X','V','I','D'),fps,ssize,3);
	
	int NumFrm=0; //the NumFrm-th frame
	BackGround *bkg_object = new BackGround();
	
	// Window
	cvNamedWindow("VEDIO",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("MOTION MODULE",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("MOVING",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("GAUSSIAN",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("COMBINE",CV_WINDOW_AUTOSIZE);
	cvMoveWindow("VEDIO",0, 0);
	cvMoveWindow("MOTION MODULE",360,0);
	cvMoveWindow("MOVING", 720, 0);
	cvMoveWindow("GAUSSIAN", 0, 350);
	cvMoveWindow("COMBINE", 360, 350);
	// color
	CvScalar Red =  CV_RGB(255,0,0);
	CvScalar White = CV_RGB(255,255,255);
	CvScalar Blue = CV_RGB(0,0,255);
	// -------------------------------- General Initialize END --------------------------------
	// -------------------------------- Background Initialize START --------------------------------
	//!!!!!!!!!!!!!!!!!!!!!!!!DIFFERENT BY VEDIO!!!!!!!!!!!!!!!!!//
	// set mask
	cvZero(maskroi);
	cvRectangle(maskroi,cvPoint(0,0),cvPoint(ssize.width*0.5,ssize.height),White,CV_FILLED,8);
	cvRectangle(maskroi,cvPoint(0,ssize.height*0.6),cvPoint(ssize.width,ssize.height),White,CV_FILLED,8);
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

	/// Motion Detection related
	IplConvKernel *element1s,*element1l,*element2s,*element2l,*element3s,*element3l,*element4s,*element4l;
	IplConvKernel *elementL,*elementS;
	//IplConvKernel for 4 directions, each direction has a larger element and smaller emelemt
	//4  3   2  
	//\  |  /
	// \ | /
	//------- 1  
	// / | \
	///  |  \
	//4  3   2
	element3l=cvCreateStructuringElementEx(4,7,2,4,CV_SHAPE_RECT,0);
	element3s=cvCreateStructuringElementEx(3,6,2,3,CV_SHAPE_RECT,0);

	element1l=cvCreateStructuringElementEx(7,5,4,3,CV_SHAPE_RECT,0);
	element1s=cvCreateStructuringElementEx(6,4,2,2,CV_SHAPE_RECT,0);
	// direction 2
	int* values1 = new int[81];
	values1[0]=0; values1[1]=0; values1[2]=0; values1[3]=0; values1[4]=0; values1[5]=0; values1[6]=0; values1[7]=1; values1[8]=1;
	values1[9]=0; values1[10]=0;values1[11]=0;values1[12]=0;values1[13]=1;values1[14]=1;values1[15]=1;values1[16]=1;values1[17]=1;
	values1[18]=0;values1[19]=0;values1[20]=1;values1[21]=1;values1[22]=1;values1[23]=1;values1[24]=1;values1[25]=1;values1[26]=0;
	values1[27]=0;values1[28]=0;values1[29]=1;values1[30]=1;values1[31]=1;values1[32]=1;values1[33]=1;values1[34]=1;values1[35]=0;
	values1[36]=0;values1[37]=0;values1[38]=1;values1[39]=1;values1[40]=1;values1[41]=1;values1[42]=1;values1[43]=0;values1[44]=0;
	values1[45]=0;values1[46]=1;values1[47]=1;values1[48]=1;values1[49]=1;values1[50]=1;values1[51]=1;values1[52]=0;values1[53]=0;
	values1[54]=0;values1[55]=1;values1[56]=1;values1[57]=1;values1[58]=1;values1[59]=1;values1[60]=1;values1[61]=0;values1[62]=0;
	values1[63]=1;values1[64]=1;values1[65]=1;values1[66]=1;values1[67]=0;values1[68]=0;values1[69]=0;values1[70]=0;values1[71]=0;
	values1[72]=1;values1[73]=1;values1[74]=0;values1[75]=0;values1[76]=0;values1[77]=0;values1[78]=0;values1[79]=0;values1[80]=0;
	
	int *values2 = new int[36];
	values2[0]=0; values2[1]=0; values2[2]=0; values2[3]=0; values2[4]=1; values2[5]=1;
	values2[6]=0; values2[7]=0; values2[8]=1; values2[9]=1; values2[10]=1;values2[11]=1;
	values2[12]=0;values2[13]=1;values2[14]=1;values2[15]=1;values2[16]=1;values2[17]=0;
	values2[18]=0;values2[19]=1;values2[20]=1;values2[21]=1;values2[22]=1;values2[23]=0;
	values2[24]=1;values2[25]=1;values2[26]=1;values2[27]=1;values2[28]=0;values2[29]=0;
	values2[30]=1;values2[31]=1;values2[32]=0;values2[33]=0;values2[34]=0;values2[35]=0;

	element2l=cvCreateStructuringElementEx(9,9,5,5,CV_SHAPE_CUSTOM,values1);
	element2s=cvCreateStructuringElementEx(6,6,3,3,CV_SHAPE_CUSTOM,values2);
	// direction 4
	int* values3 = new int[81];
	values3[0]=1; values3[1]=1; values3[2]=0; values3[3]=0; values3[4]=0; values3[5]=0; values3[6]=0; values3[7]=0; values3[8]=0;
	values3[9]=1; values3[10]=1;values3[11]=1;values3[12]=1;values3[13]=0;values3[14]=0;values3[15]=0;values3[16]=0;values3[17]=0;
	values3[18]=0;values3[19]=1;values3[20]=1;values3[21]=1;values3[22]=1;values3[23]=1;values3[24]=1;values3[25]=0;values3[26]=0;
	values3[27]=0;values3[28]=1;values3[29]=1;values3[30]=1;values3[31]=1;values3[32]=1;values3[33]=1;values3[34]=0;values3[35]=0;
	values3[36]=0;values3[37]=0;values3[38]=1;values3[39]=1;values3[40]=1;values3[41]=1;values3[42]=1;values3[43]=0;values3[44]=0;
	values3[45]=0;values3[46]=0;values3[47]=1;values3[48]=1;values3[49]=1;values3[50]=1;values3[51]=1;values3[52]=1;values3[53]=0;
	values3[54]=0;values3[55]=0;values3[56]=1;values3[57]=1;values3[58]=1;values3[59]=1;values3[60]=1;values3[61]=1;values3[62]=0;
	values3[63]=0;values3[64]=0;values3[65]=0;values3[66]=0;values3[67]=1;values3[68]=1;values3[69]=1;values3[70]=1;values3[71]=1;
	values3[72]=0;values3[73]=0;values3[74]=0;values3[75]=0;values3[76]=0;values3[77]=0;values3[78]=0;values3[79]=1;values3[80]=1;
	int *values4 = new int[36];
	values4[0]=1; values4[1]=1; values4[2]=0; values4[3]=0; values4[4]=0; values4[5]=0;
	values4[6]=1; values4[7]=1; values4[8]=1; values4[9]=1; values4[10]=0;values4[11]=0;
	values4[12]=0;values4[13]=1;values4[14]=1;values4[15]=1;values4[16]=1;values4[17]=0;
	values4[18]=0;values4[19]=1;values4[20]=1;values4[21]=1;values4[22]=1;values4[23]=0;
	values4[24]=0;values4[25]=0;values4[26]=1;values4[27]=1;values4[28]=1;values4[29]=1;
	values4[30]=0;values4[31]=0;values4[32]=0;values4[33]=0;values4[34]=1;values4[35]=1;
	element4l=cvCreateStructuringElementEx(9,9,5,5,CV_SHAPE_CUSTOM,values3);
	element4s=cvCreateStructuringElementEx(6,6,3,3,CV_SHAPE_CUSTOM,values4);
	//KernalInitiate(element1s,element1l,element2s,element2l,element3s,element3l,element4s,element4l);
	// open operation
	elementL=element1l;elementS=element1s;
	bkg_object->SetElement(elementS,elementL);
	//4,7,2,2.rect£¬0 (second third)//(first.earlier)7,6¡¢5,2,2 (first.later)4, 4, 1,1 , CV_SHAPE_RECT , 0

	// Gaussian Model params
	CvGaussBGStatModelParams* params = new CvGaussBGStatModelParams;
	params->win_size = 25000; 
	params->n_gauss = 5;
	params->bg_threshold = 0.7;
	params->std_threshold = 2.5;
	params->minArea = 15;
	params->weight_init = 0.05;
	params->variance_init = 30;
	bkg_object->SetGauParams(params);

	cvCopyImage(pFrame1,pFrame2);
	bkg_object->Initi(pFrame2);
	RectList *combinelist = new RectList();
	RectList *GauList = new RectList(); 
	RectList *STList = new RectList(); 
	// -------------------------------- Background Initialize END --------------------------------
	// -------------------------------- Objects Initialize START --------------------------------
	int RectNum=0;
	//ForeGndRectList *MotionRectList = new ForeGndRectList();   //Rect List from Motion Detection module
	AllRectList *TrustRectList = new AllRectList();    //AllRect List from Track module     
	RectList *finalRectList = new RectList();    //Final merged determined Rect List
	RectNode *rntemp1=NULL,*rntemp2=NULL;
	//ForeGndRectNode *ftemp1=NULL, *ftemp2=NULL;
	RectNode *ftemp1=NULL, *ftemp2=NULL;
	AllRectNode *atemp1=NULL, *atemp2=NULL;
	//LinkedList<int> DelID;
	ParticalItem *particalitem;

	// Match Retangles //
	CvRect r0 = cvRect(0,0,0,0);
	matchrectinfo *MRinfo = new matchrectinfo[10];
	for (int i = 0; i<10; i++)
	{MRinfo[i].id=-1; MRinfo[i].overlapa = 0; MRinfo[i].rr = r0;}
	CvRect merge1 = r0;

	// radom gengerator (for Partical filter) //
	gsl_rng* rng;
	gsl_rng_env_setup();                       //setup the enviorment of random number generator
	rng = gsl_rng_alloc( gsl_rng_mt19937 );    //create a random number generator
	gsl_rng_set( rng, time(NULL) );            //initializes the random number generator.
	// -------------------------------- Objects Initialize END --------------------------------//

	// Get first frame and Initiate //
	IplImage *showt = cvCloneImage(pFrame1);
	IplImage *showm = cvCloneImage(pFrame1);
	bgr2hsv(pFrame1,lst_hsv_frame);

	// -------------------------------- process by frame START --------------------------------//
	// set last frame
	// LOOP
	double dist1 = 0;
	double halfsidemax = -1;

	long int ObjID=1;
	while(pFrame1 = cvQueryFrame(pCapture))
	{
		NumFrm++;
		cout<<endl<<"No."<<NumFrm<<" frame."<<endl;

		bgr2hsv( pFrame1,hsv_frame );
		cvCopyImage(pFrame1,showt);
		cvCopyImage(pFrame1,showm);

		//Get Foreground
		combinelist->DelList();
		// ------------- START get moving direction ------------- //
		if ((NumFrm%100==0)||(NumFrm==1))
		{
			cvCopyImage(pFrame1,lstf);
		}
		if ((NumFrm%101==0)||(NumFrm==2))
		{
			cvCopyImage(pFrame1,curf);
			dire2 = CalculateDirection(lstf,curf,maskroi);
			double xx=(double)dire2.width;
			double yy=(double)dire2.height;
			double xxp=abs(xx);
			double yyp = abs(yy);
			cout<<"dire.x="<<(double)dire2.width<<"  dire.y="<<(double)dire2.height<<endl;
			if (yyp<=0.41*xxp)
			{elementL=element1l;elementS=element1s;}
			if (yyp>=2.4*xxp)
			{elementL=element3l;elementS=element3s;}
			if ((yyp>0.41*xxp)&&(yyp<2.4*xxp))
			{
				if (xx*yy>0)
				{elementL=element2l;elementS=element2s;}
				else
				{elementL=element4l;elementS=element4s;}
			}
			bkg_object->SetElement(elementS,elementL);
		}
		// ------------- END get moving direction ------------- //

		// ------------- START Get Foreground ------------- //
		cvCopyImage(pFrame1,pFrame2);
		bkg_object->Update(pFrame2);

		STFr=bkg_object->pFrImg_shortterm;
		GauFr=bkg_object->pFrImg_Gaussian;

		STList = GetCleanFrAndRects(STFr,ReSTFr,pFrame3);
		GauList = GetCleanFrAndRects(GauFr,ReGauFr,pFrame3);
		//GetCleanFrGnd(STFr,ReSTFr,pFrame3);
		//GetCleanFrGnd(GauFr,ReGauFr,pFrame3);
		if (bkg_object->GauISright==false)
		{
			cvCopyImage(STFr,combine1);
			//combinelist = GetCleanFrAndRects(STFr,combine1,pFrame3);
		}
		else
		{
			cvOr(ReSTFr,ReGauFr,combine1);
			//combinelist = GetCleanFrAndRects(GauFr,combine1,pFrame3);
		}
		combinelist=OutterRects(combine1,combine2,lowersize);
		// ------------- END Get Foreground ------------- //

		cvCopyImage(pFrame1,pFrame2);
		DrawRect(pFrame2,combinelist,Red);

		//detect stopped
		double templap9S=0,templapmxS=0,templap9G=0,templapmxG=0;
		CvRect ro,rcbG,rcbST;
		atemp1=TrustRectList->head;
		while (atemp1->next!=NULL)
		{
			atemp1=atemp1->next;
			ro=atemp1->GetRectMain();
			if (atemp1->GetMoTrackcnt()>3)
			{
				ftemp1=STList->head;
				ftemp2=GauList->head;
				while (ftemp1->next!=NULL)
				{
					ftemp1=ftemp1->next;
					rcbST=ftemp1->GetRect();
					templap9S=OverLapArea(ro,rcbST);
					if (templap9S>templapmxS)
					{templapmxS = templap9S;}
				}
				while (ftemp2->next!=NULL)
				{
					ftemp2=ftemp2->next;
					rcbG=ftemp2->GetRect();
					templap9G=OverLapArea(ro,rcbG);
					if (templap9G>templapmxG)
					{templapmxG=templap9G;}
				}
				if ((templapmxS<(ro.height*ro.width*0.1))&&(templapmxG>(ro.height*ro.width*0.6)))
				{atemp1->stopped = true;}
				else
				{atemp1->stopped = false;}
			}
		}

		//------------------ initiate object list(TrustRectList)START ---------------------//
		if (NumFrm==3)
		{
			RectNode *ptr = combinelist->head;
			while (ptr->next!=NULL)
			{
				ptr=ptr->next;
				//insert a new object (motionrect & mianrect)
				if (EnterInner(ptr->GetRect(),ssize))
				{
					TrustRectList->Insertid(ptr->GetRect(),ObjID);
					TrustRectList->GetByID(ObjID)->insertMiddle=false;
					ObjID++;
				}
			}
		}
		//------------------ initiate object list(TrustRectList)END ---------------------//
		//------------------ update object list(TrustRectList)START ---------------------//
		if (NumFrm>3)
		{
			// ----------------------- tracking START --------------------------//
			// track every motion rect of last frame
			// add to the track rect
			atemp1=TrustRectList->head;
			CvRect temptr = cvRect(0,0,0,0);
			while(atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				particalitem = new ParticalItem();
				particalitem->Init(lst_hsv_frame,atemp1->GetRectMain());
				temptr=particalitem->tracking(hsv_frame,rng);
				//check if temptr matches
				int mm = Match2Rect(temptr,atemp1->GetRectMain());
				if (mm>=4)
				{
					atemp1->SetRectT(temptr);
					atemp1->SetTtrack(true);
					//atemp1->SetTasMain();
				}
				else {atemp1->SetRectT(cvRect(0,0,0,0)); atemp1->SetTtrack(false);}
				delete particalitem;
			}
			// ----------------------- tracking END --------------------------//

			// ----------------------- matching START --------------------------//
			ObjectMatch(TrustRectList,combinelist,&ObjID,ssize,MRinfo);
			// ----------------------- matching END --------------------------//

			//print all objects info
			atemp2 = TrustRectList->head;
			cout<<endl;
			CvRect tr, fr, rm;
			while (atemp2->next!=NULL)
			{
				atemp2=atemp2->next;
				tr = atemp2->GetRectT(); fr = atemp2->GetRectMo(); rm = atemp2->GetRectMain();
				cout<<"Object No."<<atemp2->GetID()<<" main  "<<rm.x<<" "<<rm.y<<" "
					<<rm.height<<" "<<rm.width<<endl<<" track "<<tr.x<<" "<<tr.y<<" "
					<<tr.height<<" "<<tr.width<<endl<<" foreg "<<fr.x<<" "<<fr.y<<" "
					<<fr.height<<" "<<fr.width<<endl<<"show "<<atemp2->GetShow()
					<<" motrack "<<atemp2->GetMtrack()<<" Ttrack "<<atemp2->GetTtrack()<<endl
					<<" motrackcnt "<<atemp2->GetMoTrackcnt()<<" losttrackcnt "<<atemp2->GetLostcnt()<<endl;
			}

			finalRectList->DelList();
			atemp1=TrustRectList->head;
			while (atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				if (atemp1->GetShow())
				{finalRectList->Insertid(atemp1->GetRectMain(),atemp1->GetID());}
			}

			// draw object rectangles
			DrawRectID(showt,finalRectList,Red);

			finalRectList->DelList();
			ftemp1=combinelist->head;
			while (ftemp1->next!=NULL)
			{
				ftemp1=ftemp1->next;
				finalRectList->Insert(ftemp1->GetRect());
			}
			DrawRect(showm,finalRectList,Red);
			DrawRect(pFrame4,finalRectList,White);

			//------------------ update object list(TrustRectList)END ---------------------//
			/*
			TrustRectList is the vehicle object list.
			Every node in TrustRectList which showmain==true is a genuine object.
			ADD YOUR CODE HERE.....
			......
			...
			*/

		}
		
		// update lstFrame
		cvCopyImage(hsv_frame,lst_hsv_frame);
		
		cvShowImage("VEDIO",showt);
		cvShowImage("MOTION MODULE",showm);
		cvShowImage("MOVING",STFr);
		cvShowImage("GAUSSIAN",GauFr);
		cvShowImage("COMBINE",combine1);
		// write frame
		cvWriteFrame(twriter,showt);
		if (cvWaitKey(33) == 'q') break;
	}
	// -------------------------------- process by frame END --------------------------------//
	// -------------------------------- release resource START --------------------------------//
	cvDestroyWindow("VEDIO");
	cvDestroyWindow("MOTION MODULE");
	cvDestroyWindow("MOVING");
	cvDestroyWindow("GAUSSIAN");
	cvDestroyWindow("COMBINE1");
	
	delete combinelist;
	delete TrustRectList;
	delete finalRectList;
	delete []MRinfo;
	delete []values1;
	delete []values2;
	delete []values3;
	delete []values4;

	Release();
	cvReleaseImage(&lstf);
	cvReleaseImage(&curf);
	cvReleaseVideoWriter( &twriter );
	cvReleaseCapture(&pCapture);
	delete bkg_object;
	if (element1s != NULL) {cvReleaseStructuringElement(&element1s);}
	if (element1l != NULL) {cvReleaseStructuringElement(&element1l);}
	if (element2s != NULL) {cvReleaseStructuringElement(&element2s);}
	if (element2l != NULL) {cvReleaseStructuringElement(&element2l);}
	// -------------------------------- release resource END --------------------------------//

	return 0;
}

// --------------------------------  image initialize START --------------------------------//
void ImageIni(CvSize ssize)
{ 
    hsv_frame = cvCreateImage(ssize,IPL_DEPTH_32F,3);
    lst_hsv_frame = cvCreateImage(ssize,IPL_DEPTH_32F,3);

	pFrame3 = cvCreateImage (ssize,IPL_DEPTH_8U,1);
	pFrame4 = cvCreateImage (ssize,IPL_DEPTH_8U,1);
	pFrame5 = cvCreateImage (ssize,IPL_DEPTH_8U,1);
	STFr =  cvCreateImage (ssize,IPL_DEPTH_8U,1);
	ReSTFr =  cvCreateImage (ssize,IPL_DEPTH_8U,1);
	GauFr =  cvCreateImage (ssize,IPL_DEPTH_8U,1);
	ReGauFr =  cvCreateImage (ssize,IPL_DEPTH_8U,1);
	combine1 = cvCreateImage (ssize,IPL_DEPTH_8U,1);
	combine2 = cvCreateImage (ssize,IPL_DEPTH_8U,1);
	maskroi = cvCreateImage (ssize,IPL_DEPTH_8U,1);
}
// --------------------------------  image initialize END --------------------------------//

// --------------------------------  image release START --------------------------------//
void Release()
{
	cvReleaseImage(&lstf);
	cvReleaseImage(&curf);
	cvReleaseImage(&pFrame2);
	cvReleaseImage(&pFrame3);
	cvReleaseImage(&pFrame4);
	cvReleaseImage(&pFrame5);
    cvReleaseImage(&STFr);
	cvReleaseImage(&ReSTFr);
	cvReleaseImage(&GauFr);
	cvReleaseImage(&ReGauFr);
	cvReleaseImage(&hsv_frame);
	cvReleaseImage(&lst_hsv_frame);
}
// --------------------------------  image release END --------------------------------//