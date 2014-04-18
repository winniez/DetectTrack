#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cv.h>
#include <core\types_c.h>
#include <core\core.hpp>
#include <highgui\highgui.hpp>
#include <highgui\highgui_c.h>
#include <cxcore.hpp>

using namespace std;
using namespace cv;

class ForeGndRectList;

class ForeGndRectNode 
{
	friend class ForeGndRectList;
private:
	CvRect data;
	long int ID;
	bool tracked;
	CvScalar contourcolor;
public:
	ForeGndRectNode* next;
	int trustlevel;
	bool trusted;
	
	//constructor
	ForeGndRectNode(CvRect item)
	{
		data.height=item.height; data.width=item.width;
		data.x=item.x;data.y=item.y;
		trustlevel=0; trusted=false; ID=0;
		tracked = true;
	    next=NULL;
	}

	ForeGndRectNode(CvRect& item, ForeGndRectNode* Nt)
	{
		data.height=item.height; data.width=item.width;
		data.x=item.x;data.y=item.y;
		tracked = true;
		next=Nt;trustlevel=0;trusted=false;ID=0;
	}
	ForeGndRectNode()
	{data.height=0;data.width=0;data.x=0;data.y=0;next=NULL;trustlevel=0;trusted=false;ID=0;tracked = true; }
	
	CvRect GetRect()
	{
		CvRect rtnrect = data;
		return rtnrect;
	}

	void SetRect(CvRect rr)
	{
		data.x = rr.x; data.y = rr.y;
		data.width = rr.width; data.height = rr.height;
	}

	long int GetID()
	{
		long int rtnid = ID;
		return rtnid;
	}

	void SetID(long int setid)
	{ID=setid;}

	bool GetTrack()
	{return tracked;}

	void SetTrack(bool iftrack)
	{tracked = iftrack;}

	CvScalar GetAvgColor()
	{return contourcolor;}

	void SetColor(CvScalar color)
	{contourcolor = color;}

	//destructor
	~ForeGndRectNode(){};
};


class ForeGndRectList 
{
private:
	int Length;
public:
	ForeGndRectNode *head;
	ForeGndRectNode *ptr;
	// constructor
	ForeGndRectList()
	{Length=0;head=new ForeGndRectNode();ptr=head;head->next=NULL;}
	// destructor
	~ForeGndRectList()
	{DelList();delete head;head=NULL;ptr=NULL;}

	void DelList()
	{
		ForeGndRectNode *temp=NULL;
		while(head->next!=NULL)
		{
			temp=head->next;
			head->next=temp->next;
			delete temp;
		}
		Length = 0;
	}

	void CopyList(ForeGndRectList *another)
	{
		Length=another->Length;
		ForeGndRectNode *temp=another->head;
		while(temp->next!=NULL)
		{
			temp=temp->next;
			Insert(temp->GetRect());
		}
	}

	// Get list length
	int GetLength(){return Length;}
	// Insert a node
	void Insert(CvRect item)
	{
		ForeGndRectNode *node=new ForeGndRectNode(item);
		node->next=head->next;
		head->next=node;
		Length++;
		node->SetID(Length);
	}
	void Insertid(CvRect item,long int id)
	{
		ForeGndRectNode *node=new ForeGndRectNode(item);
		node->SetID(id); 
		node->next=head->next;
		head->next=node;
		Length++;
	}
	// Delete a node
	bool DeleteNode(CvRect item)
	{
		ForeGndRectNode *temp=head->next;
		ForeGndRectNode *tempformer=head;
		bool equal = false;
		while(temp->next!=NULL)
		{
			tempformer=temp;temp=temp->next;
			equal = ((temp->GetRect().height==item.height) && (temp->GetRect().width==item.width)
			&& (temp->GetRect().x==item.x) && (temp->GetRect().y==item.y)) ;
		}
		if (equal) 
		{
			tempformer->next=temp->next;
			delete temp;
			Length--;
			return true;
		}
		else
		{
			cout<<"Rectangle "<<item.x<<" "<<item.y<<" "<<item.height<<" "<<item.width<<" "<<endl;
			return false;
		}
	}

	bool DeleteNodeByID( long int id1)
	{
		ForeGndRectNode *temp=head->next;
		ForeGndRectNode *tempformer=head;
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

	ForeGndRectNode *GetByID(long int id1)
	{
		ForeGndRectNode *temp=head->next;
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

	CvRect GetRectByID(long int id1)
	{
		ForeGndRectNode *temp = head;
		CvRect rtnrect; 
		rtnrect.x=0; rtnrect.y=0; rtnrect.width=0; rtnrect.height=0;
		while (temp->next!=NULL)
		{
			temp=temp->next;
			if (temp->GetID()==id1)
			{
				rtnrect=temp->GetRect();
			}
		}
		return rtnrect;
	}

};

