#ifndef RECTLIST_H
#define RECTLIST_H

#include "defs.h"

using namespace std;
using namespace cv;

class RectList;

class RectNode 
{
	friend class RectList;
private:
	CvRect data;
	long int ID;
	bool tracked;
public:
	RectNode* next;
	int trustlevel;
	bool trusted;
	
	//constructor
	RectNode(CvRect item)
	{
		data.height=item.height; data.width=item.width;
		data.x=item.x;data.y=item.y;
		trustlevel=0; trusted=false; ID=0;
		tracked = true;
	    next=NULL;
	}

	RectNode(CvRect& item, RectNode* Nt)
	{
		data.height=item.height; data.width=item.width;
		data.x=item.x;data.y=item.y;
		tracked = true;
		next=Nt;trustlevel=0;trusted=false;ID=0;
	}
	RectNode()
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

	//destructor
	~RectNode(){};
};


class RectList 
{
private:
	int Length;
public:
	RectNode *head;
	RectNode *ptr;
	// constructor
	RectList()
	{Length=0;head=new RectNode();ptr=head;head->next=NULL;}
	// destructor
	~RectList()
	{DelList();delete head;head=NULL;ptr=NULL;}

	void DelList()
	{
		RectNode *temp=NULL;
		while(head->next!=NULL)
		{
			temp=head->next;
			head->next=temp->next;
			delete temp;
		}
		Length=0;
	}

	void CopyList(RectList *another)
	{
		Length=another->Length;
		RectNode *temp=another->head;
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
		RectNode *node=new RectNode(item);
		node->next=head->next;
		head->next=node;
		Length++;
	}
	void Insertid(CvRect item,long int id)
	{
		RectNode *node=new RectNode(item);
		node->SetID(id); 
		node->next=head->next;
		head->next=node;
		Length++;
	}
	// Delete a node
	bool DeleteNode(CvRect item)
	{
		RectNode *temp=head->next;
		RectNode *tempformer=head;
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
		RectNode *temp=head->next;
		RectNode *tempformer=head;
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

	RectNode *GetByID(long int id1)
	{
		RectNode *temp=head->next;
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
		RectNode *temp = head;
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
#endif
