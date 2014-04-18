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
typedef struct matchrectinfo
{
	int id;
	int overlapa;
	CvRect rr;

}matchrectinfo;

/* command line options */
#define OPTIONS ":p:oah"

/* smaller rect area thresh */
const int SMALL_AREA_THRESH1 = 8000;
/* bigger rect area thresh */
const int BIG_AREA_THRESH1 = 50000;
// STREET3 SMALL_AREA_THRESH = 2400 BIG_AREA_THRESH = 14000
// STREET2 SMALL_AREA_THRESH = 8000 BIG_AREA_THRESH = 50000
// STREET1 SMALL_AREA_THRESH = 3000 BIG_AREA_THRESH = 70000

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
	// --------------------------------Initialize--------------------------------
	// vedio and show related
	int lowersize = 6000;
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
	CvVideoWriter *twriter = cvCreateVideoWriter( "s2motion.track.avi", //name
        CV_FOURCC('X','V','I','D'),fps,ssize,3);
	CvVideoWriter *mwriter = cvCreateVideoWriter( "s2motion.avi", //name
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

	// set mask
	cvZero(maskroi);
	cvRectangle(maskroi,cvPoint(0,0),cvPoint(ssize.width,ssize.height),White,CV_FILLED,8);

	/// Motion Detection related
	IplConvKernel *element1s,*element2s,*element1l,*element2l,*elementL,*elementS;
	
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
	element1l=cvCreateStructuringElementEx(9,9,5,5,CV_SHAPE_CUSTOM,values1);

	int *values2 = new int[36];
	values2[0]=0; values2[1]=0; values2[2]=0; values2[3]=0; values2[4]=1; values2[5]=1;
	values2[6]=0; values2[7]=0; values2[8]=1; values2[9]=1; values2[10]=1;values2[11]=1;
	values2[12]=0;values2[13]=1;values2[14]=1;values2[15]=1;values2[16]=1;values2[17]=0;
	values2[18]=0;values2[19]=1;values2[20]=1;values2[21]=1;values2[22]=1;values2[23]=0;
	values2[24]=1;values2[25]=1;values2[26]=1;values2[27]=1;values2[28]=0;values2[29]=0;
	values2[30]=1;values2[31]=1;values2[32]=0;values2[33]=0;values2[34]=0;values2[35]=0;
	element1s=cvCreateStructuringElementEx(6,6,3,3,CV_SHAPE_CUSTOM,values2);

	element2l=cvCreateStructuringElementEx(7,7,4,4,CV_SHAPE_RECT,0);
	element2s=cvCreateStructuringElementEx(5,5,3,3,CV_SHAPE_RECT,0);
	// open operation 
	bkg_object->SetElement(element1s,element1l);
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

	int RectNum=0;
	//ForeGndRectList *MotionRectList = new ForeGndRectList();   //Rect List from Motion Detection module
	AllRectList *TrustRectList = new AllRectList();    //AllRect List from Track module     
	RectList *finalRectList = new RectList();    //Final merged determined Rect List
	RectNode *rntemp1=NULL,*rntemp2=NULL;
	//ForeGndRectNode *ftemp1=NULL, *ftemp2=NULL;
	RectNode *ftemp1=NULL, *ftemp2=NULL;
	AllRectNode *atemp1=NULL, *atemp2=NULL;
	LinkedList<int> DelID;
	ParticalItem *particalitem;

	// Get first frame and Initiate//
	IplImage *showt = cvCloneImage(pFrame1);
	IplImage *showm = cvCloneImage(pFrame1);
	bgr2hsv(pFrame1,lst_hsv_frame);

	// Match Retangles //
	CvRect r0 = cvRect(0,0,0,0);
	matchrectinfo *MRinfo = new matchrectinfo[10];
	for (int i = 0; i<10; i++)
	{MRinfo[i].id=-1; MRinfo[i].overlapa = 0; MRinfo[i].rr = r0;}
	matchrectinfo tempMRinfo;
	CvRect merge1 = r0;

	// radom gengerator (for Partical filter) //
	gsl_rng* rng;
	gsl_rng_env_setup();                       //setup the enviorment of random number generator
	rng = gsl_rng_alloc( gsl_rng_mt19937 );    //create a random number generator
	gsl_rng_set( rng, time(NULL) );            //initializes the random number generator.

	/*******PROCESS BY FRAME*************/
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
		// get moving direction
		if (NumFrm%100==0)
		{
			cvCopyImage(pFrame1,lstf);
		}
		if (NumFrm%101==0)
		{
			cvCopyImage(pFrame1,curf);
			dire2 = CalculateDirection(lstf,curf,maskroi);
			double xx=abs((double)dire2.width);
			double yy=abs((double)dire2.height);
			cout<<"dire.x="<<(double)dire2.width<<"  dire.y="<<(double)dire2.height<<endl;
			if ((yy==0)&&(xx!=0)){elementL=element2l;elementS=element2s;}
			if (yy!=0)
			{//	float k=float(yy/xx);
				if (yy>0.2*xx){elementL=element1l;elementS=element1s;}
				else {elementL=element2l;elementS=element2s;}
			}
			bkg_object->SetElement(elementS,elementL);
		}
		// Get Foreground
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
				{
					atemp1->stopped = true; 
					atemp1->ZeroLostcnt();
				}
				//detect moving
				if (atemp1->stopped == true)
				{
					if (templapmxS<(ro.height*ro.width*0.5))
					{atemp1->stopped = false;}
				}
			}
		}

		// Initialize object list 
		DelID.Dellist();

		if (NumFrm==2)
		{//initiate trustlist
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
		// track update and correct
		if (NumFrm>2)
		{
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

			int idm=0; int lapping=0; double sizeratio=-1;int area1=0; int area2=0;
			double prr = 0, pro = 0, prr2=0;
			// match current track rect and motion rect
			// 2 rects with most overlapping
			CvRect motempr = cvRect(0,0,0,0);
			ftemp1=combinelist->head;
			atemp1=TrustRectList->head;
			while (atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				atemp1->SetMtrack(false);
				atemp1->SetShow(true);
				if (atemp1->GetTtrack()==true)
				{temptr = atemp1->GetRectT();}
				else {temptr = atemp1->GetRectMain();}
				area1 = (temptr.height) * (temptr.width);
				for (int i = 0; i<10; i++)
	            {MRinfo[i].id=-1; MRinfo[i].overlapa = 0; MRinfo[i].rr = r0;}
				//match with current frame track rect
				ftemp1=combinelist->head;
				idm = 0; lapping=0; sizeratio = -1;
				while (ftemp1->next!=NULL)
				{
					ftemp1=ftemp1->next;
					if (ftemp1->trusted==false)
					{
						motempr=ftemp1->GetRect();
						if (idm>=10){break;}
						lapping = OverLapArea(motempr,temptr);
						sizeratio = SimilarSize(temptr,motempr);
						// one object matches several motion rects
						// distance of two center1 is smaller than half of the large side
						if ((lapping >= area1*0.2)&&(sizeratio<1.4))
						{ MRinfo[idm].overlapa = lapping; MRinfo[idm].id = ftemp1->GetID(); MRinfo[idm].rr = ftemp1->GetRect(); idm++;}
					}
				}

				// sort MRinfo by MRinfo.overlapa in descending order
				tempMRinfo.id=-1;tempMRinfo.overlapa=0; tempMRinfo.rr=r0;
				for (int j = 0; j<idm-1; j++)
				{
					for (int jj = j+1; jj<idm; jj++)
					{
						if (MRinfo[jj].overlapa>MRinfo[j].overlapa)
						{tempMRinfo = MRinfo[j]; MRinfo[j]=MRinfo[jj]; MRinfo[jj]=tempMRinfo;}
					}
				}

				/*if (idm==0)
				{
					bool nearo=NearOutter(temptr,ssize);
					if ((atemp1->GetLostcnt()>=3)&&(nearo==false))
					{
						int index=atemp1->GetMoTrackcnt();

					}
				}
*/
				if (idm==1)
				{// only one rect could possibly match
					pro = SimilarSize(temptr,MRinfo[0].rr);
					prr = (double) ((double)MRinfo[0].overlapa/(double)area1);
					prr2 = (double) ((double)MRinfo[0].overlapa/(double)(MRinfo[0].rr.width*MRinfo[0].rr.height));
					ftemp1=combinelist->GetByID(MRinfo[0].id);
					motempr=ftemp1->GetRect();
					area2 = motempr.height*motempr.width;
					if (prr>=0.5)
					{
						if ((pro>=0.7)&&(pro<=1.3))
						{
							ftemp1->trusted = true;
							atemp1->SetMtrack(true);
							atemp1->SetRectMo(motempr);  //(ftemp1->GetRect());
							//atemp1->CalculateRate(motempr);
							atemp1->SetMoasMain();
							//atemp1->SetTasMain();
						}
						else
						{// sudden area/shape change
							// matches, but trust tracking result
							ftemp1->trusted=true;
							if (atemp1->GetLostcnt()>=3) 
							{
								if ((area2>BIG_AREA_THRESH1)||(area2<SMALL_AREA_THRESH1))
								{// 2 cars merged as 1 in foreground
									atemp1->SetMtrack(true);
									atemp1->SetTasMain();
								}
								else
								{
								atemp1->SetRectMo(motempr);  //(ftemp1->GetRect());
							    //atemp1->CalculateRate(motempr);
								atemp1->SetMtrack(true);
								}
							}
							else 
							{
								//atemp1->CalculateRate(atemp1->GetRectT());
								atemp1->SetTasMain();
							}
						}
					}
					else
					{// not much
						if ((prr<=0.3)||(prr2<0.3))
						{atemp1->SetMtrack(false);//atemp1->CalculateRate(atemp1->GetRectT());
						atemp1->SetTasMain();}
					}
				}
				if (idm >=2)
				{// fist one matches or unproper size
					pro = SimilarSize(temptr,MRinfo[0].rr);
					prr = (double) ((double)MRinfo[0].overlapa/(double)area1);
					if (prr>=0.5)
					{
						ftemp1=combinelist->GetByID(MRinfo[0].id);
						if ((pro>=0.7)&&(pro<=1.3))
						{
							ftemp1->trusted = true;
							atemp1->SetMtrack(true);
							atemp1->SetRectMo(ftemp1->GetRect());
							//atemp1->CalculateRate(ftemp1->GetRect());
							atemp1->SetMoasMain();
							//atemp1->SetTasMain();
						}
						else
						{// sudden area/shape change
							// matches, but trust tracking result
							ftemp1->trusted=true;
							if ((area2>BIG_AREA_THRESH1)||(area2<SMALL_AREA_THRESH1))
							{// 2 cars merged as 1 in foreground
								atemp1->SetMtrack(true);
								//atemp1->CalculateRate(atemp1->GetRectT());
								atemp1->SetTasMain();
							}
							else
							{
								if (atemp1->GetLostcnt()>=3) 
								{
									atemp1->SetRectMo(ftemp1->GetRect());
									//atemp1->CalculateRate(ftemp1->GetRect());
									atemp1->SetMoasMain();
									atemp1->SetMtrack(true);
								}
								else {
									//atemp1->CalculateRate(atemp1->GetRectT());
									atemp1->SetTasMain();}
							}	
						}
					}
					else
					// if first one size not match, second one may need merge
					{
						merge1 = Merge2Rect(MRinfo[0].rr, MRinfo[1].rr);
						pro = SimilarSize(temptr,merge1);
						if ((pro>=0.6)&&(pro<=1.4))
						{//merge rect as 
							atemp1->SetMtrack(true);
							atemp1->SetRectMo(merge1);
							//atemp1->CalculateRate(merge1);
							atemp1->SetMoasMain();
							ftemp1 = combinelist->GetByID(MRinfo[0].id);
							ftemp1->trusted = true;
							ftemp1 = combinelist->GetByID(MRinfo[1].id);
							ftemp1->trusted = true;
						}
						else
						{//can not find match motion rect
							atemp1->SetMtrack(false);
							atemp1->SetRectMo(r0);
							//atemp1->CalculateRate(atemp1->GetRectT());
							atemp1->SetTasMain();
						}
					}
				}
			}

			//After finish matching all possible existing object, deal with potential new objects
			//match motion rect with objects
			int maxlapindex=0;
			ftemp1 = combinelist->head;
			while (ftemp1->next!=NULL)
			{
				ftemp1 = ftemp1->next;
				
				if (ftemp1->trusted==false)
				{
					motempr = ftemp1->GetRect();
					area2=motempr.height*motempr.width;
					if ((area2>BIG_AREA_THRESH1)||(area2<SMALL_AREA_THRESH1))
					{ftemp1->trusted=true;}
				}
				if (ftemp1->trusted == false)
				{//if near inner border
					// or not match with any existing object
					motempr = ftemp1->GetRect();
					bool neari = NearInner(motempr,ssize);
					double olapmax = 0, olaptemp = 0, per = 10;
					double farea = (motempr.height) * (motempr.width);
					atemp1=TrustRectList->head;
					while (atemp1->next!=NULL)
					{
						atemp1=atemp1->next;
						temptr = atemp1->GetRectMain();
						olaptemp = OverLapArea(motempr,temptr);
						//record the biggest overlap area
						if (olaptemp>olapmax) {olapmax=olaptemp;maxlapindex=atemp1->GetID();}
					}
					
					per = (double)((double)olapmax/(double)farea);
					//if(neari||(per<=0.2))
					if (per>0.2)
					{
						atemp2=TrustRectList->GetByID(maxlapindex);
		                if (atemp2!=NULL)
						{
							if (atemp2->GetLostcnt()>2)
							{//
								ftemp1->trusted = true;
								atemp2->SetMtrack(true);
								atemp2->SetRectMo(ftemp1->GetRect());
								atemp2->SetMoasMain();
							}
						}
					}
					if (per<=0.2)
					{// add new entering
						TrustRectList->Insertid(motempr,ObjID);
						if (neari)
						{TrustRectList->GetByID(ObjID)->insertMiddle=false;}
					    ObjID++;
					}
					else
					{// match with existing objects
						idm = 0; lapping=0; sizeratio = -1;
						for (int i = 0; i<10; i++)
	                    {MRinfo[i].id=-1; MRinfo[i].overlapa = 0; MRinfo[i].rr = r0;}
						atemp1 = TrustRectList->head;
						while (atemp1->next!=NULL)
						{
							atemp1 = atemp1->next;
							temptr = atemp1->GetRectMain();
							
							if (atemp1->GetMtrack()==false)
							{
					            lapping = OverLapArea(motempr,temptr);
					            sizeratio = SimilarSize(motempr,temptr);
								if (idm>=10){break;}
					            // one object matches several motion rects
					            if ((lapping >= farea*0.25)&&(sizeratio<1.5))
								{ 
									MRinfo[idm].overlapa = lapping; 
									MRinfo[idm].id = atemp1->GetID(); 
									MRinfo[idm].rr = atemp1->GetRectT(); 
									idm++;
								}
							}
						}
						// sort MRinfo by MRinfo.overlapa in descending order
						tempMRinfo.id=-1;tempMRinfo.overlapa=0; tempMRinfo.rr=r0;
						for (int j = 0; j<idm-1; j++)
						{
							for (int jj = j+1; jj<idm; jj++)
							{
								if (MRinfo[jj].overlapa>MRinfo[j].overlapa)
								{tempMRinfo = MRinfo[j]; MRinfo[j]=MRinfo[jj]; MRinfo[jj]=tempMRinfo;}
							}
						}
						if ((idm==1))
						{//remain one to one, but has sudden change in shape
							// if near the border, update main rect
							neari=NearOutter(ftemp1->GetRect(),ssize);
							atemp1=TrustRectList->GetByID(MRinfo[0].id);
							pro = SimilarSize(motempr,MRinfo[0].rr);
							prr = (double) ((double)MRinfo[0].overlapa/(double)farea);
							if (prr>=0.4)
							{
								if ((pro<1.3)&&(pro>0.7))
								{
									ftemp1->trusted = true;
									atemp1->SetRectMo(ftemp1->GetRect());
									atemp1->SetMtrack(true);
									//atemp1->CalculateRate(atemp1->GetRectMo());
									atemp1->SetMoasMain();		
								}
								else
								{
									if (neari)
									{
										ftemp1->trusted = true;
										atemp1->SetRectMo(ftemp1->GetRect());
										atemp1->SetMtrack(true);
										//atemp1->CalculateRate(atemp1->GetRectMo());
										atemp1->SetMoasMain();		
									}
									else 
									{
										ftemp1->trusted = true;
										atemp1->SetMtrack(true);
										//atemp1->CalculateRate(atemp1->GetRectT());
										atemp1->SetTasMain();
									}
								}
							}	
						}
						if ((idm==2)||(idm>2))
						{
							atemp1=TrustRectList->GetByID(MRinfo[0].id);
							neari=NearOutter(ftemp1->GetRect(),ssize);
							pro = SimilarSize(motempr,MRinfo[0].rr);
							prr = (double) ((double)MRinfo[0].overlapa/(double)farea);
							atemp2=TrustRectList->GetByID(MRinfo[1].id);
							
							if (prr>=0.4)
							{
								if ((pro<1.3)&&(pro>0.7))
								{
									ftemp1->trusted = true;
									atemp1->SetRectMo(ftemp1->GetRect());
									atemp1->SetMtrack(true);
									//atemp1->CalculateRate(atemp1->GetRectMo());
									atemp1->SetMoasMain();		
								}
								else
								{
									if (neari)
									{
										ftemp1->trusted = true;
										atemp1->SetRectMo(ftemp1->GetRect());
										atemp1->SetMtrack(true);
										//atemp1->CalculateRate(atemp1->GetRectMo());
										atemp1->SetMoasMain();		
									}
									else 
									{
										ftemp1->trusted = true;
										atemp1->SetMtrack(true);
										//atemp1->CalculateRate(atemp1->GetRectT() );
										atemp1->SetTasMain();
									}
								}
							}
							else
							{
								merge1 = Merge2Rect(MRinfo[0].rr, MRinfo[1].rr);
							    pro = SimilarSize(temptr,merge1);
								if ((pro>=0.7)&&(pro<=1.3))
								{//merge rect as 
									ftemp1->trusted=true;
									if (atemp1->GetID()<atemp2->GetID())
									{
										atemp1->SetMtrack(true);
										atemp1->SetRectMo(merge1);
										//atemp1->CalculateRate(merge1);
										atemp1->SetMoasMain();
										DelID.insertnode(atemp2->GetID());
									}
									else
									{
										atemp2->SetMtrack(true);
										atemp2->SetRectMo(merge1);
										//atemp2->CalculateRate(merge1);
										atemp2->SetMoasMain();
										DelID.insertnode(atemp1->GetID());
									}
								}
								else
								{//can not find match motion rect
									atemp1->SetMtrack(false);
									atemp1->SetRectMo(r0);
									//atemp1->CalculateRate(atemp1->GetRectT());
									atemp1->SetTasMain();
									atemp2->SetMtrack(false);
									atemp2->SetRectMo(r0);
									//atemp2->CalculateRate(atemp2->GetRectT());
									atemp2->SetTasMain();
								}
							}
						}
					}

				}
			}

			// track and lose update the counter
			atemp1=TrustRectList->head;
			bool mm1=false;
			bool mm2=false;
			while (atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				mm1=atemp1->GetMtrack();
				mm2=atemp1->GetTtrack();
				if (!(mm1))
				{
					if (atemp1->stopped==false)
					{atemp1->UpLostcnt();}
				}
				else
				{atemp1->UpMoTrackcnt();atemp1->ZeroLostcnt();}
			}

			// delete losing rects if lose for times
			int toreiniid=-1;
			// delete 
			atemp1 = TrustRectList->head;
			while(atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				if (atemp1->stopped == false)
				{
					if (atemp1->GetLostcnt()>=5)
					{
						if (NearOutter(atemp1->GetRectMain(),ssize)==true)
						{DelID.insertnode(atemp1->GetID());}
						else
						{
							if (atemp1->GetLostcnt()>=10) 
							{DelID.insertnode(atemp1->GetID());}
						}
					}
				}
				
			}
			// do delete
			ListNode <int> * ptrdelid = DelID.head;
			if (ptrdelid->next!=NULL)
			{
				ptrdelid=ptrdelid->next;
				TrustRectList->DeleteNodeByID((long int)ptrdelid->Getdata());
			}
			
			// motion rect exist for less than 3 times, set show as false
			atemp1 = TrustRectList->head;
			bool like1=false,like2=false;
			while (atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				if (atemp1->GetMoTrackcnt()<3)
				{atemp1->SetShow(false);}
				else
				{
					if (atemp1->insertMiddle==true)
					{if (atemp1->GetMoTrackcnt()<5){atemp1->SetShow(false);}}
				}
				if (atemp1->GetLostcnt()>=3)
				{
					if (NearOutter(atemp1->GetRectMain(),ssize)==true)
					{atemp1->SetShow(false);}
					else
					{
						if (atemp1->GetLostcnt()>=8)
						{atemp1->SetShow(false);}
					}
				}
				//like1 = CarLikeWH(atemp1->GetRectMain(),dire);
				//like2 = CarLikeArea(atemp1->GetRectMain());
				//if ((like1&&like2)==false)
				//{atemp1->SetShow(false);}
			}

			// forcely set overlap rects not shwo
			double ar1=0,ar2=0,amin=0; int sindex=-1;
			CvRect rd1=r0,rd2=r0;
			atemp1=TrustRectList->head;
			while (atemp1->next!=NULL)
			{
				atemp1=atemp1->next;
				if (atemp1->GetShow()==true)
				{
					atemp2=atemp1;
					while(atemp2->next!=NULL)
					{
						atemp2=atemp2->next;
						if (atemp2->GetShow()==true)
						{
							rd1=atemp1->GetRectMain(); rd2=atemp2->GetRectMain();
							ar1=rd1.width*rd1.height; ar2 = rd2.width*rd2.height;
							if (ar1<ar2) {amin=ar1;sindex=1;}
							else {amin=ar2;sindex=2;}
							lapping=OverLapArea(rd1,rd2);
							if (lapping>=amin*0.5)
							{
								if (sindex==1){atemp1->SetShow(false);}
								else {atemp2->SetShow(false);}
							}
						}
					}
				}
			}


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

			// draw
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

		}
		// update lstFrame
		//cvCopyImage(pFrame,lstFrame);
		cvCopyImage(hsv_frame,lst_hsv_frame);
		
		cvShowImage("VEDIO",showt);
		cvShowImage("MOTION MODULE",showm);
		cvShowImage("MOVING",STFr);
		cvShowImage("GAUSSIAN",GauFr);
		cvShowImage("COMBINE",combine1);
		// write frame
		cvWriteFrame(twriter,showt);
		cvWriteFrame(mwriter,showm);
		if (cvWaitKey(33) == 'q') break;

	}
	cvDestroyWindow("VEDIO");
	cvDestroyWindow("MOTION MODULE");
	cvDestroyWindow("MOVING");
	cvDestroyWindow("GAUSSIAN");
	cvDestroyWindow("COMBINE1");
	

	//release resource
	delete combinelist;
	delete TrustRectList;
	delete finalRectList;
	delete []MRinfo;

	Release();
	cvReleaseImage(&lstf);
	cvReleaseImage(&curf);
	cvReleaseVideoWriter( &twriter );
	cvReleaseVideoWriter( &mwriter );
	cvReleaseCapture(&pCapture);
	delete bkg_object;
	if (element1s != NULL) {cvReleaseStructuringElement(&element1s);}
	if (element1l != NULL) {cvReleaseStructuringElement(&element1l);}
	if (element2s != NULL) {cvReleaseStructuringElement(&element2s);}
	if (element2l != NULL) {cvReleaseStructuringElement(&element2l);}


	return 0;
}

//---------------------------Motion Detect-----------------------------------------
// initialize
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
