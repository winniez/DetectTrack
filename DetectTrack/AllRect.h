#pragma once
#ifndef ALLRECT_H
#define ALLRECT_H

#include "defs.h"

using namespace std;
using namespace cv;

typedef struct Rate
{
	int x_move;
	int y_move;
}Rate;

class AllRectList;

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
	AllRectNode *next;
	int resembleM;
	int resembleT;
	bool insertMiddle;
	bool stopped;
	//Rate *rate;

	AllRectNode()
	{
		ID=0;
		mainr=cvRect(0,0,0,0);
		motionr=cvRect(0,0,0,0);
		trackr=cvRect(0,0,0,0);
		//rate =new Rate[100];
		//for (int i =0 ;i<100;i++)
		//{rate[i].x_move=0; rate[i].y_move=0;}
		tldtracked=false;
		motracked = false;
		lostcnt=0;
		motionexitcnt=0;
		tldtoupdate=false;
		todel=false;
		MatchMT=false;
		showmain=true;
		resembleM=0;
		resembleT=0;
		insertMiddle=false;
		stopped = false;
		next=NULL;
	}
	AllRectNode(CvRect mr)
	{
		ID=0;
		mainr=cvRect(0,0,0,0);
		motionr=mr;
		trackr=cvRect(0,0,0,0);
		//rate =new Rate[100];
		//for (int i =0 ;i<100;i++)
		//{rate[i].x_move=0; rate[i].y_move=0;}
		tldtracked= false;
		motracked = true;
		lostcnt=0;
		motionexitcnt=1;
		tldtoupdate=true;
		todel=false;
		MatchMT=false;
		showmain=true;
		resembleM=0;
		resembleT=0;
		insertMiddle=false;
		stopped = false;
		next=NULL;
	}

	~AllRectNode() 
	{}//delete []rate;}
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
	{mainr = rr;motionexitcnt++;}
	void SetMtrack(bool tracked)
	{
		motracked=tracked;
	}
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
	void DownLostcnt()
	{lostcnt--;}
	void ZeroLostcnt()
	{lostcnt=0;}
	void UpMoTrackcnt()
	{motionexitcnt++;}
	void ZeroMoTrackcnt()
	{motionexitcnt=0;}

	/*void CalculateRate(CvRect next)
	{
		if(motionexitcnt==100)
		{
			int d=79,i=0;
			for (i=0;i<20;i++)
			{rate[i]=rate[i+d];}
			motionexitcnt=20;
			for (i=motionexitcnt;i<100;i++)
			{rate[i].x_move=0;rate[i].y_move=0;}
		}
		double x1=(double)(mainr.x)+(double)(mainr.width/2);
		double y1=(double)(mainr.y)+(double)(mainr.height/2);
		double x2=(double)(next.x)+(double)(next.width/2);
		double y2=(double)(next.y)+(double)(next.height/2);
		rate[motionexitcnt].x_move=abs((double)(x1-x2));
		rate[motionexitcnt].y_move=abs((double)(y1-y2));
	}*/

	void SetTasMain()
	{mainr=trackr;motionexitcnt++;}
	void SetMoasMain()
	{mainr=motionr;motionexitcnt++;}

};


class AllRectList
{
private:
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
	void Insert(CvRect item)
	{
		AllRectNode *node=new AllRectNode(item);
		node->SetMtrack(true);
		node->next=head->next;
		head->next=node;
		Length++;
	}
	void Insertid(CvRect item,long int id)
	{
		AllRectNode *node=new AllRectNode(item);
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