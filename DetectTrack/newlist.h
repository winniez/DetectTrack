#pragma once
#ifndef NEWLIST_H
#define NEWLIST_H
#include <iostream>
#include "defs.h"
using namespace std;

class newlistnode;
class newlist;

typedef struct hogresult
{
	float hog[5];
	CvRect rects;
	float finalhog;
	int cnt;
}hogresult;

class newlistnode
{
private:
	int ID;
public:
	hogresult HOGR;
	newlistnode *next;

	newlistnode(int id)
	{
		ID=id;
		next=NULL;
		for (int i = 0; i<5; i++)
		{HOGR.hog[i]=0;}
		HOGR.cnt=0;
	}
	newlistnode()
	{
		ID=0;
		next=NULL;
		for (int i = 0; i<5; i++)
		{HOGR.hog[i]=0;}
		HOGR.cnt=0;
	}
	~newlistnode(){}
	int GetID()
	{return ID;}
	void SetID(long int setid)
	{ID=setid;}
};

class newlist
{
private:
	int length;
public:
	newlistnode *head;

	newlist()
	{length=0;head=new newlistnode();}
	// Get list length
	int GetLength(){return length;}
	// insert new node
	void Insertid(int id)
	{
		newlistnode *newnode = new newlistnode(id);
		newnode->next = head->next;
	}
	void Insert()
	{
		newlistnode *newnode = new newlistnode();
		newnode->next = head->next;
	}
	//delete whole list
	void DelList()
	{
		newlistnode *temp = NULL;
		while(head->next!=NULL)
		{
			temp=head->next;
			head->next=temp->next;
			delete temp;
		}
		Length=0;
	}

	~newlist()
	{DelList();delete head;}

	bool DeleteNodeByID(int id1)
	{
		newlistnode *temp=head->next;
		newlistnode *tempformer=head;
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

	newlistnode *GetByID(long int id1)
	{
		newlistnode *temp=head->next;
		while (temp!=NULL)
		{
			if (temp->GetID()==id1)
			{return temp;}
			temp = temp->next;
		}
		return NULL;
	}
	
};


#endif
