#pragma once
#ifndef ALLRECT2_H
#define ALLRECT2_H
#include "defs.h"
#include "ParticalItem.h"

const int CONTINUE_TRACK_THRESH = 5;

class AllRectList;

class TrackItem
{
private:
	int updatecnt;
	CvRect lstr,curRect;
	bool toupdate;
public:
	
	ParticalItem *trackitem;

	TrackItem(IplImage* curhsv, CvRect &region)
	{
		trackitem = new ParticalItem();
		updatecnt=0;
		toupdate=false;
		trackitem->Init(curhsv,region);
		lstr=region; 
		curRect=lstr;
	}
	//void Upupdatecnt() {updatecnt;}
	void DoUpdate(IplImage* curhsv,IplImage* lsthsv, CvRect &region, gsl_rng* rng)
	{
		delete trackitem;
		trackitem = new ParticalItem();
		trackitem->Init(lsthsv,region);
		curRect = trackitem->tracking(curhsv,rng);
		updatecnt = 0; toupdate=false;
	}

	void Track(IplImage* curhsv,IplImage* lsthsv, gsl_rng* rng)
	{
		if (updatecnt==CONTINUE_TRACK_THRESH)
		{ DoUpdate(curhsv,lsthsv,lstr,rng);}
		else
		{
			curRect = trackitem->tracking(curhsv,rng);
			updatecnt++;
		}
	}

	void Track_valid()
	{
		if (updatecnt>0)
		{

		}

	}

	void SetLstR(CvRect r1)
	{lstr=r1;}

	CvRect GetCurR()
	{return curRect;}
	CvRect GetLstR()
	{return lstr;}
};

class AllRectNode 
{
private:
	CvRect mainr;
	CvRect motionr;
	CvRect trackr;
	
	long int ID;
	bool tldtracked;
	bool motracked;
	int lostcnt;
	int motionexitcnt;
	bool tldtoupdate;
	bool todel;
	bool MatchMT;
	bool showmain;

public:
	TrackItem* trackit;
	AllRectNode *next;
	int resembleM;
	int resembleT;
	bool stopped;

	AllRectNode()
	{
		ID=0;
		mainr=cvRect(0,0,0,0);
		motionr=cvRect(0,0,0,0);
		trackr=cvRect(0,0,0,0);
		trackit = NULL;
		tldtracked=false;
		motracked = false;
		lostcnt=0;
		motionexitcnt=0;
		tldtoupdate=false;
		todel=false;
		MatchMT=false;
		showmain=true;
		stopped = false;
		resembleM=0;
		resembleT=0;
		next=NULL;
	}
	AllRectNode(IplImage* curhsv, CvRect &mr)
	{
		ID=0;
		mainr=cvRect(0,0,0,0);
		motionr=mr;
		trackr=cvRect(0,0,0,0);
		trackit = new TrackItem(curhsv, mr);
		tldtracked= false;
		motracked = true;
		lostcnt=0;
		motionexitcnt=0;
		tldtoupdate=true;
		todel=false;
		MatchMT=false;
		showmain=true;
		stopped = false;
		resembleM=0;
		resembleT=0;
		next=NULL;
	}


	~AllRectNode(){delete trackit;}
	//Get private
	CvRect GetRectT()
	{CvRect rtnrect = trackr; return rtnrect;}
	CvRect GetRectMo()
	{CvRect rtnrect = motionr; return rtnrect;}
	CvRect GetRectMain()
	{CvRect rtnrect = mainr; return rtnrect;}
	bool GetTtrack()
	{return tldtracked;}
	bool GetMtrack()
	{return motracked;}
	int GetLostcnt()
	{return lostcnt;}
	bool GetTUpdate()
	{return tldtoupdate;}
	long int GetID()
	{return ID;}
	bool GettoDel()
	{return todel;}
	bool GetMatchMoT()
	{return MatchMT;}
	bool GetShow()
	{return showmain;}
	int GetMoTrackcnt()
	{return motionexitcnt;}

	//Set private
	void SetID(long int id)
	{ID=id;}
	void SetRectT(CvRect rr)
	{trackr=rr;}
	void SetRectMo(CvRect rr)
	{motionr = rr; }
	void SetRectMain (CvRect rr)
	{mainr = rr;}
	void SetMtrack(bool tracked)
	{motracked=tracked;}
	void SetTtrack(bool tracked)
	{tldtracked=tracked;}
	void SetTupdate(bool update)
	{tldtoupdate=update;}
	void SetMatchMoT(bool match)
	{MatchMT=match;}
	void SetShow(bool show)
	{showmain=show;}
	void UpLostcnt()
	{lostcnt++;}
	void ZeroLostcnt()
	{lostcnt=0;}
	void UpMoTrackcnt()
	{motionexitcnt++;}
	void ZeroMoTrackcnt()
	{motionexitcnt=0;}

	void SetTasMain()
	{mainr=trackr;}
	void SetMoasMain()
	{mainr=motionr;}

};


class AllRectList
{
private:
	int Length;
public:
	AllRectNode *head;
	AllRectNode *ptr;
	// constructor
	AllRectList()
	{Length=0;head=new AllRectNode();ptr=head;head->next=NULL;}
	// destructor
	~AllRectList()
	{DelList();delete head;head=NULL;ptr=NULL;}

	void DelList()
	{
		AllRectNode *temp=NULL;
		while(head->next!=NULL)
		{
			temp=head->next;
			head->next=temp->next;
			delete temp;
		}
	}

	// Get private
	int GetLength(){return Length;}

	// Insert a node/ motion rect
	void Insert(IplImage* curhsv,CvRect item)
	{
		AllRectNode *node=new AllRectNode(curhsv,item);
		node->SetMtrack(true);
		node->next=head->next;
		head->next=node;
		Length++;
	}
	void Insertid(IplImage* curhsv,CvRect item,long int id)
	{
		AllRectNode *node=new AllRectNode(curhsv,item);
		node->SetID(id); 
		node->SetMtrack(true);
		node->SetMoasMain();
		//node->UpMoTrackcnt();
		node->next=head->next;
		head->next=node;
		Length++;
	}

	// Delete a node
	bool DeleteNodeByID( long int id1)
	{
		AllRectNode *temp=head->next;
		AllRectNode *tempformer=head;
		while (temp!=NULL)
		{
			if (temp->GetID()==id1)
			{
				tempformer->next = temp->next;
				delete temp;
				Length--;
				return true;
				break;
			}
			tempformer = temp;
			temp = temp->next;
		}
		return false;
	}

	AllRectNode *GetByID(long int id1)
	{
		AllRectNode *temp=head->next;
		while (temp!=NULL)
		{
			if (temp->GetID()==id1)
			{
				return temp;
			}
			temp = temp->next;
		}
		return NULL;
	}

	CvRect GetTRectByID(long int id1)
	{
		AllRectNode *temp = head;
		CvRect rtnrect; 
		rtnrect.x=0; rtnrect.y=0; rtnrect.width=0; rtnrect.height=0;
		while (temp->next!=NULL)
		{
			temp=temp->next;
			if (temp->GetID()==id1)
			{rtnrect=temp->GetRectT();}
		}
		return rtnrect;
	}

	CvRect GetMoRectByID(long int id1)
	{
		AllRectNode *temp = head;
		CvRect rtnrect; 
		rtnrect.x=0; rtnrect.y=0; rtnrect.width=0; rtnrect.height=0;
		while (temp->next!=NULL)
		{
			temp=temp->next;
			if (temp->GetID()==id1)
			{rtnrect=temp->GetRectMo();}
		}
		return rtnrect;
	}

	CvRect GetMainRectByID(long int id1)
	{
		AllRectNode *temp = head;
		CvRect rtnrect; 
		rtnrect.x=0; rtnrect.y=0; rtnrect.width=0; rtnrect.height=0;
		while (temp->next!=NULL)
		{
			temp=temp->next;
			if (temp->GetID()==id1)
			{rtnrect=temp->GetRectMain();}
		}
		return rtnrect;
	}

};

#endif