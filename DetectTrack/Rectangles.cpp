#include "Rectangles.h"
#include "List.h"

/***************************** Function Implementation ************************/
// Foreground and tracking match
void ObjectMatch(AllRectList *objects, RectList *motiondetect,long int *ObjID,CvSize ssize,matchrectinfo *MRinfo)
{
	// Initialize object list 
	matchrectinfo tempMRinfo;
	CvRect r0 = cvRect(0,0,0,0);
	CvRect temptr = r0, merge1 = r0;;
	RectNode *ftemp3=NULL, *ftemp4=NULL;
	AllRectNode *atemp3=NULL, *atemp4=NULL;
	LinkedList<int> DelID;
	DelID.Dellist();
	int idm=0; int lapping=0; double sizeratio=-1;int area1=0; int area2=0;
			double prr = 0, pro = 0, prr2=0;
			// match current track rect and motion rect
			// 2 rects with most overlapping
			CvRect motempr = cvRect(0,0,0,0);
			ftemp3=motiondetect->head;
			atemp3=objects->head;
			while (atemp3->next!=NULL)
			{
				atemp3=atemp3->next;
				atemp3->SetMtrack(false);
				atemp3->SetShow(true);
				if (atemp3->GetTtrack()==true)
				{temptr = atemp3->GetRectT();}
				else {temptr = atemp3->GetRectMain();}
				area1 = (temptr.height) * (temptr.width);
				for (int i = 0; i<10; i++)
	            {MRinfo[i].id=-1; MRinfo[i].overlapa = 0; MRinfo[i].rr = r0;}
				//match with current frame track rect
				ftemp3=motiondetect->head;
				idm = 0; lapping=0; sizeratio = -1;
				while (ftemp3->next!=NULL)
				{
					ftemp3=ftemp3->next;
					if (ftemp3->trusted==false)
					{
						motempr=ftemp3->GetRect();
						if (idm>=10){break;}
						lapping = OverLapArea(motempr,temptr);
						sizeratio = SimilarSize(temptr,motempr);
						// one object matches several motion rects
						// distance of two center1 is smaller than half of the large side
						if ((lapping >= area1*0.2)&&(sizeratio<1.4))
						{ MRinfo[idm].overlapa = lapping; MRinfo[idm].id = ftemp3->GetID(); MRinfo[idm].rr = ftemp3->GetRect(); idm++;}
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
				if (idm==1)
				{// only one rect could possibly match
					pro = SimilarSize(temptr,MRinfo[0].rr);
					prr = (double) ((double)MRinfo[0].overlapa/(double)area1);
					prr2 = (double) ((double)MRinfo[0].overlapa/(double)(MRinfo[0].rr.width*MRinfo[0].rr.height));
					ftemp3=motiondetect->GetByID(MRinfo[0].id);
					motempr=ftemp3->GetRect();
					area2 = motempr.height*motempr.width;
					if (prr>=0.5)
					{
						if ((pro>=0.7)&&(pro<=1.3))
						{
							ftemp3->trusted = true;
							atemp3->SetMtrack(true);
							atemp3->SetRectMo(motempr);  //(ftemp3->GetRect());
							//atemp3->CalculateRate(motempr);
							atemp3->SetMoasMain();
							//atemp3->SetTasMain();
						}
						else
						{// sudden area/shape change
							// matches, but trust tracking result
							ftemp3->trusted=true;
							if (atemp3->GetLostcnt()>=3) 
							{
								if ((area2>BIG_AREA_THRESH)||(area2<SMALL_AREA_THRESH))
								{// 2 cars merged as 1 in foreground
									atemp3->SetMtrack(true);
									atemp3->SetTasMain();
								}
								else
								{
								atemp3->SetRectMo(motempr);  //(ftemp3->GetRect());
							    //atemp3->CalculateRate(motempr);
								atemp3->SetMtrack(true);
								}
							}
							else 
							{
								//atemp3->CalculateRate(atemp3->GetRectT());
								atemp3->SetTasMain();
							}
						}
					}
					else
					{// not much
						if ((prr<=0.3)||(prr2<0.3))
						{atemp3->SetMtrack(false);//atemp3->CalculateRate(atemp3->GetRectT());
						atemp3->SetTasMain();}
					}
				}
				if (idm >=2)
				{// fist one matches or unproper size
					pro = SimilarSize(temptr,MRinfo[0].rr);
					prr = (double) ((double)MRinfo[0].overlapa/(double)area1);
					if (prr>=0.5)
					{
						ftemp3=motiondetect->GetByID(MRinfo[0].id);
						if ((pro>=0.7)&&(pro<=1.3))
						{
							ftemp3->trusted = true;
							atemp3->SetMtrack(true);
							atemp3->SetRectMo(ftemp3->GetRect());
							//atemp3->CalculateRate(ftemp3->GetRect());
							atemp3->SetMoasMain();
							//atemp3->SetTasMain();
						}
						else
						{// sudden area/shape change
							// matches, but trust tracking result
							ftemp3->trusted=true;
							if ((area2>BIG_AREA_THRESH)||(area2<SMALL_AREA_THRESH))
							{// 2 cars merged as 1 in foreground
								atemp3->SetMtrack(true);
								//atemp3->CalculateRate(atemp3->GetRectT());
								atemp3->SetTasMain();
							}
							else
							{
								if (atemp3->GetLostcnt()>=3) 
								{
									atemp3->SetRectMo(ftemp3->GetRect());
									//atemp3->CalculateRate(ftemp3->GetRect());
									atemp3->SetMoasMain();
									atemp3->SetMtrack(true);
								}
								else {
									//atemp3->CalculateRate(atemp3->GetRectT());
									atemp3->SetTasMain();}
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
							atemp3->SetMtrack(true);
							atemp3->SetRectMo(merge1);
							//atemp3->CalculateRate(merge1);
							atemp3->SetMoasMain();
							ftemp3 = motiondetect->GetByID(MRinfo[0].id);
							ftemp3->trusted = true;
							ftemp3 = motiondetect->GetByID(MRinfo[1].id);
							ftemp3->trusted = true;
						}
						else
						{//can not find match motion rect
							atemp3->SetMtrack(false);
							atemp3->SetRectMo(r0);
							//atemp3->CalculateRate(atemp3->GetRectT());
							atemp3->SetTasMain();
						}
					}
				}
			}

			//After finish matching all possible existing object, deal with potential new objects
			//match motion rect with objects
			int maxlapindex=0;
			ftemp3 = motiondetect->head;
			while (ftemp3->next!=NULL)
			{
				ftemp3 = ftemp3->next;
				
				if (ftemp3->trusted==false)
				{
					motempr = ftemp3->GetRect();
					area2=motempr.height*motempr.width;
					if ((area2>BIG_AREA_THRESH)||(area2<SMALL_AREA_THRESH))
					{ftemp3->trusted=true;}
				}
				if (ftemp3->trusted == false)
				{//if near inner border
					// or not match with any existing object
					motempr = ftemp3->GetRect();
					bool neari = NearInner(motempr,ssize);
					double olapmax = 0, olaptemp = 0, per = 10;
					double farea = (motempr.height) * (motempr.width);
					atemp3=objects->head;
					while (atemp3->next!=NULL)
					{
						atemp3=atemp3->next;
						temptr = atemp3->GetRectMain();
						olaptemp = OverLapArea(motempr,temptr);
						//record the biggest overlap area
						if (olaptemp>olapmax) {olapmax=olaptemp;maxlapindex=atemp3->GetID();}
					}
					
					per = (double)((double)olapmax/(double)farea);
					//if(neari||(per<=0.2))
					if (per>0.2)
					{
						atemp4=objects->GetByID(maxlapindex);
		                if (atemp4!=NULL)
						{
							if (atemp4->GetLostcnt()>2)
							{//
								ftemp3->trusted = true;
								atemp4->SetMtrack(true);
								atemp4->SetRectMo(ftemp3->GetRect());
								atemp4->SetMoasMain();
							}
						}
					}
					if (per<=0.2)
					{// add new entering
						objects->Insertid(motempr,*ObjID);
						if (neari)
						{objects->GetByID(*ObjID)->insertMiddle=false;}
					    *ObjID= *ObjID + 1;
					}
					else
					{// match with existing objects
						idm = 0; lapping=0; sizeratio = -1;
						for (int i = 0; i<10; i++)
	                    {MRinfo[i].id=-1; MRinfo[i].overlapa = 0; MRinfo[i].rr = r0;}
						atemp3 = objects->head;
						while (atemp3->next!=NULL)
						{
							atemp3 = atemp3->next;
							temptr = atemp3->GetRectMain();
							
							if (atemp3->GetMtrack()==false)
							{
					            lapping = OverLapArea(motempr,temptr);
					            sizeratio = SimilarSize(motempr,temptr);
								if (idm>=10){break;}
					            // one object matches several motion rects
					            if ((lapping >= farea*0.25)&&(sizeratio<1.5))
								{ 
									MRinfo[idm].overlapa = lapping; 
									MRinfo[idm].id = atemp3->GetID(); 
									MRinfo[idm].rr = atemp3->GetRectT(); 
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
							neari=NearOutter(ftemp3->GetRect(),ssize);
							atemp3=objects->GetByID(MRinfo[0].id);
							pro = SimilarSize(motempr,MRinfo[0].rr);
							prr = (double) ((double)MRinfo[0].overlapa/(double)farea);
							if (prr>=0.4)
							{
								if ((pro<1.3)&&(pro>0.7))
								{
									ftemp3->trusted = true;
									atemp3->SetRectMo(ftemp3->GetRect());
									atemp3->SetMtrack(true);
									//atemp3->CalculateRate(atemp3->GetRectMo());
									atemp3->SetMoasMain();		
								}
								else
								{
									if (neari)
									{
										ftemp3->trusted = true;
										atemp3->SetRectMo(ftemp3->GetRect());
										atemp3->SetMtrack(true);
										//atemp3->CalculateRate(atemp3->GetRectMo());
										atemp3->SetMoasMain();		
									}
									else 
									{
										ftemp3->trusted = true;
										atemp3->SetMtrack(true);
										//atemp3->CalculateRate(atemp3->GetRectT());
										atemp3->SetTasMain();
									}
								}
							}	
						}
						if ((idm==2)||(idm>2))
						{
							atemp3=objects->GetByID(MRinfo[0].id);
							neari=NearOutter(ftemp3->GetRect(),ssize);
							pro = SimilarSize(motempr,MRinfo[0].rr);
							prr = (double) ((double)MRinfo[0].overlapa/(double)farea);
							atemp4=objects->GetByID(MRinfo[1].id);
							
							if (prr>=0.4)
							{
								if ((pro<1.3)&&(pro>0.7))
								{
									ftemp3->trusted = true;
									atemp3->SetRectMo(ftemp3->GetRect());
									atemp3->SetMtrack(true);
									//atemp3->CalculateRate(atemp3->GetRectMo());
									atemp3->SetMoasMain();		
								}
								else
								{
									if (neari)
									{
										ftemp3->trusted = true;
										atemp3->SetRectMo(ftemp3->GetRect());
										atemp3->SetMtrack(true);
										//atemp3->CalculateRate(atemp3->GetRectMo());
										atemp3->SetMoasMain();		
									}
									else 
									{
										ftemp3->trusted = true;
										atemp3->SetMtrack(true);
										//atemp3->CalculateRate(atemp3->GetRectT() );
										atemp3->SetTasMain();
									}
								}
							}
							else
							{
								merge1 = Merge2Rect(MRinfo[0].rr, MRinfo[1].rr);
							    pro = SimilarSize(temptr,merge1);
								if ((pro>=0.7)&&(pro<=1.3))
								{//merge rect as 
									ftemp3->trusted=true;
									if (atemp3->GetID()<atemp4->GetID())
									{
										atemp3->SetMtrack(true);
										atemp3->SetRectMo(merge1);
										//atemp3->CalculateRate(merge1);
										atemp3->SetMoasMain();
										DelID.insertnode(atemp4->GetID());
									}
									else
									{
										atemp4->SetMtrack(true);
										atemp4->SetRectMo(merge1);
										//atemp4->CalculateRate(merge1);
										atemp4->SetMoasMain();
										DelID.insertnode(atemp3->GetID());
									}
								}
								else
								{//can not find match motion rect
									atemp3->SetMtrack(false);
									atemp3->SetRectMo(r0);
									//atemp3->CalculateRate(atemp3->GetRectT());
									atemp3->SetTasMain();
									atemp4->SetMtrack(false);
									atemp4->SetRectMo(r0);
									//atemp4->CalculateRate(atemp4->GetRectT());
									atemp4->SetTasMain();
								}
							}
						}
					}
				}
			}

			// track and lose update the counter
			atemp3=objects->head;
			bool mm1=false;
			bool mm2=false;
			while (atemp3->next!=NULL)
			{
				atemp3=atemp3->next;
				mm1=atemp3->GetMtrack();
				mm2=atemp3->GetTtrack();
				if (!(mm1))
				{
					if (atemp3->stopped==false)
					{atemp3->UpLostcnt();}
				}
				else
				{atemp3->UpMoTrackcnt();atemp3->ZeroLostcnt();}
			}

			// delete losing rects if lose for times
			int toreiniid=-1;
			// delete 
			atemp3 = objects->head;
			while(atemp3->next!=NULL)
			{
				atemp3=atemp3->next;
				if (atemp3->GetLostcnt()>=7)
				{
					if (NearOutter(atemp3->GetRectMain(),ssize)==true)
					{DelID.insertnode(atemp3->GetID());}
					else
					{
						if (atemp3->GetLostcnt()>=9) 
						{DelID.insertnode(atemp3->GetID());}
					}
				}
				
			}
			// do delete
			ListNode <int> * ptrdelid = DelID.head;
			if (ptrdelid->next!=NULL)
			{
				ptrdelid=ptrdelid->next;
				objects->DeleteNodeByID((long int)ptrdelid->Getdata());
			}
			
			// motion rect exist for less than 3 times, set show as false
			atemp3 = objects->head;
			bool like1=false,like2=false;
			while (atemp3->next!=NULL)
			{
				atemp3=atemp3->next;
				if (atemp3->GetMoTrackcnt()<3)
				{atemp3->SetShow(false);}
				else
				{
					if (atemp3->insertMiddle==true)
					{if (atemp3->GetMoTrackcnt()<5){atemp3->SetShow(false);}}//连续出现三帧，确认为真正目标，显示
				}
				// 丢失隐藏删除处理
				if (atemp3->GetLostcnt()>=5)
				{
					if (NearOutter(atemp3->GetRectMain(),ssize)==true)
					{atemp3->SetShow(false);}
					else
					{
						if (atemp3->GetLostcnt()>=7)
						{atemp3->SetShow(false);}
					}
				}
				//like1 = CarLikeWH(atemp3->GetRectMain(),dire);
				//like2 = CarLikeArea(atemp3->GetRectMain());
				//if ((like1&&like2)==false)
				//{atemp3->SetShow(false);}
			}

			// forcely set overlap rects not show
			double ar1=0,ar2=0,amin=0; int sindex=-1;
			CvRect rd1=r0,rd2=r0;
			atemp3=objects->head;
			while (atemp3->next!=NULL)
			{
				atemp3=atemp3->next;
				if (atemp3->GetShow()==true)
				{
					atemp4=atemp3;
					while(atemp4->next!=NULL)
					{
						atemp4=atemp4->next;
						if (atemp4->GetShow()==true)
						{
							rd1=atemp3->GetRectMain(); rd2=atemp4->GetRectMain();
							ar1=rd1.width*rd1.height; ar2 = rd2.width*rd2.height;
							if (ar1<ar2) {amin=ar1;sindex=1;}
							else {amin=ar2;sindex=2;}
							lapping=OverLapArea(rd1,rd2);
							if (lapping>=amin*0.5)
							{
								if (sindex==1){atemp3->SetShow(false);}
								else {atemp4->SetShow(false);}
							}
						}
					}
				}
			}

}

// Get Outter Rectangles
RectList* OutterRects(IplImage *frame, IplImage *tempf,int Thresh)
{
	RectList* rectL = new RectList();
	cvCopyImage(frame,tempf);
	//Get contour
	CvMemStorage *stor;
	stor = cvCreateMemStorage(0);
	int mark = 0,p=0;
	CvRect temp;
	double cntrArea = 0;
	int keepCntrCnt=0; //number of kept contours in current frame

	CvSeq* temp1, *temp2, *c,*head;
	CvContourScanner scanner = cvStartFindContours(tempf,stor,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);  
	while ((c=cvFindNextContour(scanner))!=NULL)
	{
		cntrArea = cvContourArea(c);
		if (cntrArea<CONTOUR_MIN_AERA)
		{cvSubstituteContour(scanner,NULL);}
		else keepCntrCnt++;
	}
	head=cvEndFindContours(&scanner);

	int i=0;
	CvRect *a = new CvRect[keepCntrCnt];
	CvRect *r = new CvRect[keepCntrCnt];

	temp1=head;
	for (i=0,temp1=head;temp1!=NULL;i++,temp1=temp1->h_next)
	{
		CvRect rect = ((CvContour*)temp1)->rect;
		a[i]=rect;
		temp.x = rect.x;
		temp.y = rect.y;
		temp.height = rect.height;
		temp.width = rect.width;
		//remove small rect
		if(temp.width*temp.height>Thresh)
		{r[p]=temp;p++;}
	}

	int *flag = new int[p];
	for (int cc=0;cc<p;cc++)
	{flag[cc]=0;}

	int smallerarea = 0; int smallerindex = 0;
	int area1 = 0; int area2 = 0;
	for ( int cc=0;cc<p;cc++)  //find overlapping rect
	{
		for ( int j=cc+1; j<p; j++)
		{
			// overlapping area: determine factor
			
			area1 = (r[cc].width)*(r[cc].height);
			area2 = (r[j].width)*(r[j].height);
			if (area1<area2) 
			{smallerarea=area1; smallerindex=cc;} 
			else 
			{smallerarea= area2; smallerindex = j;}
			int largerX = 0, smallerXW = 0, largerY = 0, smallerYH = 0;
			if (r[cc].x>=r[j].x) largerX=r[cc].x;
			else largerX = r[j].x;
			if ((r[cc].x+r[cc].width)<=(r[j].x+r[j].width)) smallerXW = (r[cc].x)+(r[cc].width);
			else smallerXW=(r[j].x)+(r[j].width);
			if (r[cc].y>=r[j].y) largerY=r[j].y;
			else largerY=r[cc].y;
			if ((r[cc].y+r[cc].height)<=(r[j].y+r[j].height)) smallerYH = (r[cc].y)+(r[cc].height);
			else smallerYH = (r[j].y)+(r[j].height);
			int overlaparea=0; double percentage=0;
			if ((largerX<smallerXW)&&(largerY<=smallerYH))
			{
				overlaparea=(smallerXW-largerX)*(smallerYH-largerY);
				percentage = overlaparea/(smallerarea);
				if (percentage>=0.75) flag[smallerindex]=1;
			}
		}
		/*int areat = 0;
		areat=(r[cc].width)*(r[cc].height);
		if ((areat<1000)||(areat>CONTOUR_MAX_AERA))
		{flag[cc]=1;}*/
	}
	// scan the contour list, get AvgColors
	//temp1=head;
	//cvZero(maskTemp);
	//int id = 0;
	//CvScalar CVX_WHITE=CV_RGB(255,255,255);
	//for (id=0,temp1=head;temp1!=NULL;temp1=temp1->h_next,id++)
	//{
	//	//draw current contour solely
	//	cvDrawContours(maskTemp,temp1,CVX_WHITE,CVX_WHITE,-1,CV_FILLED,8);
	//	// draw filled contours to mask, one by one in each iteration
	//	//cvDrawContours(pFrImgcopy,temp1,CVX_WHITE,CVX_WHITE,-1,CV_FILLED,8);
	//	// get colors of each contour
	//	AvgColors[id]=cvAvg(pFrameColor,maskTemp);
	//	// reset maskTemp, prepare to draw next contour solely 
	//	cout<<"Contour "<<id<<" Color("<<AvgColors[id].val[0]<<","<<AvgColors[id].val[1]<<","<<AvgColors[id].val[2]<<")"<<endl;
	//	cvZero(maskTemp);
	//}
	int cnt=0;
	for (int q=0; q<p; q++)
	{
		cnt++;
		int dd=flag[q];
		if (dd==0)
		{rectL->Insertid(r[q],cnt);}
	}
	// release memory
	delete []a;
	delete []r;
	delete []flag;
	
	cvReleaseMemStorage(&stor);
	return rectL;
}

/* Get Specific Foreground */
void GetCleanFrGnd(IplImage *src, IplImage *dst,IplImage *tempf)
{
	cvCopyImage(src,tempf);
	cvZero(dst);
	//Get contour
	CvMemStorage *stor;
	stor = cvCreateMemStorage(0);
	int mark = 0,diffx = 0,diffy = 0,p = 0;
	CvRect temp;
	double cntrArea = 0;
	int keepCntrCnt=0; //number of kept contours in current frame

	CvSeq* temp1, *c, *head;
	CvContourScanner scanner = cvStartFindContours(tempf,stor,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);  
	while ((c=cvFindNextContour(scanner))!=NULL)
	{
		cntrArea = cvContourArea(c);
		if ((cntrArea<CONTOUR_MIN_AERA)||(cntrArea>CONTOUR_MAX_AERA))
		{cvSubstituteContour(scanner,NULL);}
		else keepCntrCnt++;
	}
	head=cvEndFindContours(&scanner);

	int i=0;
	CvScalar CVX_WHITE=CV_RGB(255,255,255);

	temp1=head;
	for (i=0,temp1=head;temp1!=NULL;i++,temp1=temp1->h_next)
	{
		CvRect rect = ((CvContour*)temp1)->rect;
		temp.height = rect.height;
		temp.width = rect.width;
		//remove small rect
		if(temp.width*temp.height>800)
		{//draw current contour solely
			cvDrawContours(dst,temp1,CVX_WHITE,CVX_WHITE,-1,CV_FILLED,8);
		}
	}
	cvReleaseMemStorage(&stor);
}

/* Get Specific Foreground and Rectangles*/
RectList* GetCleanFrAndRects(IplImage *src, IplImage *dst,IplImage *tempf)
{
	RectList* rectL = new RectList();
	cvCopyImage(src,tempf);
	cvZero(dst);
	//Get contour
	CvMemStorage *stor;
	stor = cvCreateMemStorage(0);
	int mark = 0,diffx = 0,diffy = 0,p = 0;
	CvRect temp;
	double cntrArea = 0;
	int keepCntrCnt=0; //number of kept contours in current frame

	CvSeq* temp1, *c, *head;
	CvContourScanner scanner = cvStartFindContours(tempf,stor,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);  
	while ((c=cvFindNextContour(scanner))!=NULL)
	{
		cntrArea = cvContourArea(c);
		if ((cntrArea<CONTOUR_MIN_AERA)||(cntrArea>CONTOUR_MAX_AERA))
		{cvSubstituteContour(scanner,NULL);}
		else keepCntrCnt++;
	}
	head=cvEndFindContours(&scanner);
	CvRect *a = new CvRect[keepCntrCnt];
	CvRect *r = new CvRect[keepCntrCnt];
	int i=0;
	CvScalar CVX_WHITE=CV_RGB(255,255,255);

	temp1=head;
	for (i=0,temp1=head;temp1!=NULL;i++,temp1=temp1->h_next)
	{
		CvRect rect = ((CvContour*)temp1)->rect;
		a[i]=rect;
		temp.x = rect.x;
		temp.y = rect.y;
		temp.height = rect.height;
		temp.width = rect.width;
		//remove small rect
		if(temp.width*temp.height>2400)
		{
			r[p]=temp;
			p++;
		}
		//remove small rect
		if(temp.width*temp.height>900)
		{//draw current contour solely
			cvDrawContours(dst,temp1,CVX_WHITE,CVX_WHITE,-1,CV_FILLED,8);
		}
	}

	int *flag = new int[p];
	for (int cc=0;cc<p;cc++)
	{flag[cc]=0;}

	for ( int cc=0;cc<p;cc++)  //find overlapping rect
	{
		for ( int j=cc+1; j<p; j++)
		{
			// overlapping area: determine factor
			int smallerarea = 0; int smallerindex = 0;
			int area1 = 0; int area2 = 0;
			area1 = (r[cc].width)*(r[cc].height);
			area2 = (r[j].width)*(r[j].height);
			if (area1<area2) 
			{smallerarea=area1; smallerindex=cc;} 
			else 
			{smallerarea= area2; smallerindex = j;}
			int largerX = 0, smallerXW = 0, largerY = 0, smallerYH = 0;
			if (r[cc].x>=r[j].x) largerX=r[cc].x;
			else largerX = r[j].x;
			if ((r[cc].x+r[cc].width)<=(r[j].x+r[j].width)) smallerXW = (r[cc].x)+(r[cc].width);
			else smallerXW=(r[j].x)+(r[j].width);
			if (r[cc].y>=r[j].y) largerY=r[j].y;
			else largerY=r[cc].y;
			if ((r[cc].y+r[cc].height)<=(r[j].y+r[j].height)) smallerYH = (r[cc].y)+(r[cc].height);
			else smallerYH = (r[j].y)+(r[j].height);
			int overlaparea=0; double percentage=0;
			if ((largerX<smallerXW)&&(largerY<=smallerYH))
			{
				overlaparea=(smallerXW-largerX)*(smallerYH-largerY);
				percentage = overlaparea/(smallerarea);
				if (percentage>=0.75) flag[smallerindex]=1;
			}
		}
		int areat = 0;
		areat=(r[cc].width)*(r[cc].height);
		if ((areat<2600)||(areat>CONTOUR_MAX_AERA))
		{flag[cc]=1;}
	}
	int cnt=0;
	for (int q=0; q<p; q++)
	{
		cnt++;
		int dd=flag[q];
		if (dd==0)
		{rectL->Insertid(r[q],cnt);}
	}
	// release memory
	delete []a;
	delete []r;
	delete []flag;
	cvReleaseMemStorage(&stor);
	return rectL;
}

//-----------------------------Draw Rect-------------------------------------------
void DrawRectID(IplImage* Img, RectList *rect, CvScalar color)
{
	RectNode *temp=rect->head->next;
	CvPoint pt1,pt2;
	CvRect r1=cvRect(0,0,0,0);
	//CvScalar colorBlue = CV_RGB(0,0,255);
	
	//the font variable    
	CvFont font;    
	double hScale=0.5; double vScale=0.5;    
	int lineWidth=2;// character line width   
	//TCHAR showMsg[]; 
	char *buffer=new char[10];
	const char *Msg;
	//initialize font   
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth); 

	while (temp!=NULL)
	{
		r1=temp->GetRect();
		if (r1.height+r1.width!=0)
		{// draw rect
			pt1 = cvPoint(temp->GetRect().x,temp->GetRect().y);
			pt2 = cvPoint(temp->GetRect().x+temp->GetRect().width,temp->GetRect().y+temp->GetRect().height);
			cvRectangle (Img,pt1,pt2,color,2,CV_AA,0);
			// draw id
			sprintf( buffer, "%d", temp->GetID());
			Msg=buffer;
			cvPutText(Img,Msg,cvPoint(pt1.x+15,pt1.y+15),&font,color);
		}
		temp=temp->next;
	}
}

void DrawRect(IplImage* Img, RectList *rect, CvScalar color)
{
	RectNode *temp=rect->head->next;
	CvPoint pt1,pt2;
	CvRect r1=cvRect(0,0,0,0);
	//CvScalar colorBlue = CV_RGB(0,0,255);
	while (temp!=NULL)
	{
		r1=temp->GetRect();
		if (r1.width+r1.height!=0)
		{
			pt1 = cvPoint(temp->GetRect().x,temp->GetRect().y);
			pt2 = cvPoint(temp->GetRect().x+temp->GetRect().width,temp->GetRect().y+temp->GetRect().height);
			cvRectangle (Img,pt1,pt2,color,2,CV_AA,0);
		}
		temp=temp->next;
	}
}
//----------------- Conditions for Match and Merge --------------------------------
int Match2Rect(CvRect r1, CvRect r2)
{
	int trust=0;
	// input are valid rect? Y continue: N return -1000
	int check10=0,check20=0;
	check10=r1.height+r1.width;
	check20=r2.height+r2.width;
	if ((check10*check20)<=0)
	{return -1000;}

	int area1=0,area2=0;
	area1=(r1.height)*(r1.width);
	area2=(r2.height)*(r2.width);
	double ratio1 = (double)(area1/area2); 
	if ((ratio1>0.75)||(ratio1<1.25)) 
		trust=trust+2;
	else 
	{
		if((ratio1>0.65)||(ratio1<1.35)) trust=trust+1;
		if ((ratio1<0.55)||(ratio1>1.55)) trust=trust-1;
	}
	double largerarea = 1;
	if (area1>area2) largerarea=area1;
	else largerarea = area2;

	// overlapping area: determine factor
	/*int largerX = 0, smallerXW = 0, largerY = 0, smallerYH = 0;
	if (r1.x>=r2.x) largerX=r1.x;
	else largerX = r2.x;
	if ((r1.x+r1.width)<=(r2.x+r2.width)) smallerXW = (r1.x)+(r1.width);
	else smallerXW=(r2.x)+(r2.width);
	if (r1.y>=r2.y) largerY=r2.y;
	else largerY=r1.y;
	if ((r1.y+r1.height)<=(r2.y+r2.height)) smallerYH = (r1.y)+(r1.height);
	else smallerYH = (r2.y)+(r2.height);
	int overlaparea=0; double percentage=0;
	if ((largerX<smallerXW)&&(largerY<=smallerYH))
	{
		overlaparea=(smallerXW-largerX)*(smallerYH-largerY);
		percentage = overlaparea/(largerarea);
		if (percentage>=0.8) trust=trust+3;
		if ((percentage<0.8)&&(percentage>=0.7)) trust = trust+2;
		if ((percentage<0.7)&&(percentage>=0.6)) trust =trust+1;
		if (percentage<0.4) trust = trust-1;
	}*/


	//w/l
	double ratio2=1;
	ratio2 = ((double)(r1.width/r1.height))/((double)(r2.width/r2.height));
	if (abs(ratio2-1)<=0.2) trust=trust+2;
	else 
	{
		if (abs(ratio2-1)<=0.35) trust=trust+1;
		if (abs(ratio2-1)>0.45) trust=trust-1;
	}

	//center distance
	CvPoint center1, center2;
	center1.x = (int)(r1.x+(r1.width)/2);
	center1.y = (int)(r1.y+(r1.height)/2);
	center2.x = (int)(r2.x+(r2.width)/2);
	center2.y = (int)(r2.y+(r2.height)/2);
	double dist1 = 0;
	dist1 = sqrt((double)(center1.x-center2.x)*(center1.x-center2.x)+(center1.y-center2.y)*(center1.y-center2.y));
	if (dist1<20) trust=trust+2;
	else 
	{
		if (dist1<40) trust=trust+1;
		if (dist1>55) trust=trust-1;
	}

	return trust;
}

int OverLapArea(CvRect r1, CvRect r2)
{
	// input are valid rect? Y continue: N return -1000
	int check10=0,check20=0;
	check10=r1.height+r1.width;
	check20=r2.height+r2.width;
	if ((check10*check20)<=0)
	{return -1000;}

	int area1=0,area2=0;
	area1=(r1.height)*(r1.width);
	area2=(r2.height)*(r2.width);
	// overlapping area: determine factor
	int largerX = 0, smallerXW = 0, largerY = 0, smallerYH = 0;
	if (r1.x>=r2.x) largerX=r1.x;
	else largerX = r2.x;
	if ((r1.x+r1.width)<=(r2.x+r2.width)) smallerXW = (r1.x)+(r1.width);
	else smallerXW=(r2.x)+(r2.width);
	if (r1.y>=r2.y) largerY=r1.y;
	else largerY=r2.y;
	if ((r1.y+r1.height)<=(r2.y+r2.height)) smallerYH = (r1.y)+(r1.height);
	else smallerYH = (r2.y)+(r2.height);
	int overlaparea=0; double percentage=0;
	if ((largerX<smallerXW)&&(largerY<=smallerYH))
	{
		overlaparea=(smallerXW-largerX)*(smallerYH-largerY);
		/*percentage = overlaparea/(largerarea);
		if (percentage>=0.8) trust=trust+3;
		if ((percentage<0.8)&&(percentage>=0.7)) trust = trust+2;
		if ((percentage<0.7)&&(percentage>=0.6)) trust =trust+1;
		if (percentage<0.4) trust = trust-1;*/
	}
	else {overlaparea=-1000;}
	return overlaparea;
}

double SimilarSize(CvRect r1, CvRect r2)
{
	int check10=0,check20=0;
	check10=r1.height+r1.width;
	check20=r2.height+r2.width;
	if ((check10*check20)<=0) {return -1000;}

	int area1=0,area2=0;
	area1=(r1.height)*(r1.width);
	area2=(r2.height)*(r2.width);
	double ratio1 = 0;
	ratio1=(double)((double)area2/(double)area1); 
	double ratio2 = abs((double)(1-ratio1));
	return ratio1;
}

/* Enter Inner Center or not */
bool EnterInner(CvRect rc,CvSize size)
{
	// Get Center
	CvPoint pt = cvPoint(0,0);
	pt.x=rc.x+(rc.width/2);
	pt.y=rc.y+(rc.height/2);

	CvRect inner = cvRect(OUTTER_MARGIN, OUTTER_MARGIN ,size.width-2*OUTTER_MARGIN ,size.height-2*OUTTER_MARGIN);

	bool isinner = ((pt.x>=inner.x)&&(pt.y>=inner.y))&&(pt.x<=(inner.x+inner.width)&&(pt.y<=inner.y+inner.height));
	return isinner;
}

bool NearInner(CvRect rc, CvSize size)
{
	// Get Center
	CvPoint pt = cvPoint(0,0);
	pt.x=rc.x+(rc.width/2);
	pt.y=rc.y+(rc.height/2);

	CvRect inner = cvRect(OUTTER_MARGIN, OUTTER_MARGIN ,size.width-2*OUTTER_MARGIN ,size.height-2*OUTTER_MARGIN);
	int x1=0, x2=0, y1=0, y2=0;
	x1 = pt.x-inner.x; x2 = inner.x+inner.width-pt.x;
	y1 = pt.y-inner.y; y2 = inner.y+inner.height-pt.y;
	int min =1000;
	if (x1<min) min=x1;
	if (x2<min) min=x2;
	if (y1<min) min=y1;
	if (y2<min) min=y2;
	bool near1=0;
	if (min<30) near1 = true;
	return near1;
}

bool NearOutter(CvRect rc, CvSize size)
{
	int x1=0, x2=0, y1=0, y2=0;
	x1 = rc.x-0;
	x2 = size.width-rc.x-rc.width;
	y1 = rc.y-0;
	y2 = size.height-rc.y-rc.height;
	
	int minxy = 50000;
	if (x1<minxy) minxy=x1;
	if (x2<minxy) minxy=x2;
	if (y1<minxy) minxy=y1;
	if (y2<minxy) minxy=y2;
	
	if ((minxy<8)&&(minxy>=0))
	{return true;}
	else {return false;}
}
/* Likelihood of car */
bool CarLikeWH(CvRect rc, int direction)
{
	bool islike=false;
	if (direction == 1)
	{//horizontal
		islike = (rc.width>(0.9*rc.height))&&(rc.width<=3*(rc.height));
	}
	if (direction == 2)
	{//vertical
		islike = (rc.height > (0.9*rc.width))&&(rc.height<=3*(rc.width));
	}
	return islike;
}

bool CarLikeArea(CvRect rc)
{
	int area = (rc.width)*(rc.height);
	if ((area>=SMALL_AREA_THRESH-200)&&(area<=BIG_AREA_THRESH+1500)) return true;
	else return false;
}


bool SameObject(CvRect r1, CvRect r2, int direction)
{
	bool a = false, b = false, c = false, d = false, e = false, f = false;
	// a side difference
	int sidediff1 = 0, sidediff2 = 0, sidediffall = 0;
	if (direction==1)
	{//horizontal
		sidediff1 = abs(r1.y-r2.y);
		sidediff2 = abs((r1.y+r1.height)-(r2.y+r2.height));
	}
	else
	{// vertical
		sidediff1 = abs(r1.x-r2.x);
		sidediff2 = abs((r1.x+r1.width)-(r2.x+r2.width));
	}
	sidediffall = sidediff1+sidediff2;
	if (sidediffall<=SIDE_DIFF_MAX) {a = true;}
	else {a = false;}

	// b nearer side distance
	int dist1 = 0,dist2=0,dist=0;
	if (direction==1)
	{
		dist1 = abs(r1.x-r2.x-r2.width);
		dist2 = abs(r2.x-r1.x-r1.width);
	}
	else
	{
		dist1 = abs(r1.y-r2.y-r2.height);
		dist2 = abs(r2.y-r1.y-r1.height);
	}
	if (dist1<=dist2) {dist=dist1;}
	else {dist=dist2;}
	if (dist<=BORDER_DISTANCE_MAX) {b = true;}
	else {b = false;}

	//c d merge more like a car
	bool r1like = false, r2like = false, r1r2like = false;
	
	r1like = CarLikeWH(r1,direction);
	r2like = CarLikeWH(r2,direction);
	CvRect merge = Merge2Rect(r1,r2);

	if (r1like || r2like)
	{// check merge
		c = true;
		r1r2like = CarLikeWH(merge,direction);
		d = r1r2like;
	}
	else
	{c = false; d = false;}

	//e color 
	e = true;
	// f area
	int r1area = 0, r2area = 0, r1r2area = 0;
	r1area = (r1.width)*(r1.height);
	r2area = (r2.width)*(r2.height);
	r1r2area = (merge.height)*(merge.width);
	int r1areadiff = 0, r2areadiff = 0, r1r2areadiff = 0;
	r1areadiff = abs((double)(SMALL_AREA_THRESH-r1area));
	r2areadiff = abs((double)(SMALL_AREA_THRESH-r2area));
	r1r2areadiff = BIG_AREA_THRESH-r1r2area;
	if ((r1areadiff+r2areadiff<=1500)&&(r1r2areadiff<=3000))
	{f=true;}
	else {f=false;}

	bool tomerge = a&&b&&c&&d&&e&&f;
	return tomerge;
}

CvRect Merge2Rect(CvRect r1, CvRect r2)
{
	CvRect merge = cvRect(0,0,0,0);
	// set merge rect
	if (r1.x<r2.x) {merge.x = r1.x;}
	else {merge.x = r2.x;}
	if (r1.y<r2.y) {merge.y = r1.y;}
	else {merge.y = r2.y; }
	if ((r1.x+r1.width)>(r2.x+r2.width))  {merge.width = r1.x+r1.width - merge.x;}
	else {merge.width = r2.x+r2.width - merge.x;}
	if ((r1.y+r1.height)>(r2.y+r2.height)) {merge.height = r1.y+r1.height - merge.y;}
	else {merge.height = r2.y+r2.height - merge.y;}
	
	return merge;
}

