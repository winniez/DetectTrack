#pragma once
#ifndef LIST_H
#define LIST_H
#include <iostream>
using namespace std;

template <class Type> class LinkedList;
template <class Type> class ListNode;

 template <class T>
 class ListNode
 {
	 friend class LinkedList<T>;
 private:
	 T data;
	 
	 //int size;
 public:
	 ListNode<T> *next;
	 //constructor
	 ListNode():next(NULL){}
	 ListNode(T D,ListNode<T> *Next=NULL):data(D),next(Next){}
	 // copy constructor
	// ListNode(const ListNode& node)
	 //{data=node.data;next=node.next;}
	 T Getdata()
	 {return data;}
	 ~ListNode(){}
 };

 template <class T>
 class LinkedList
 {
 protected:
	 
	 int size;           //length of linked list
 public:
	 ListNode<T> *head;  //list head node
	 //constructor
	 LinkedList(int len):size(len){head=new ListNode<T>;}
	 LinkedList(){size=0;head=new ListNode<T>;}
	 //copy constructor
	 LinkedList(const LinkedList& list);
	 //delete the list
	 void Dellist();
	 //destructor
	 ~LinkedList()
	 {Dellist();
	 delete head;
	 }
	 //insert a node into the list
	 void insertnode(T value);
	 //delete a node from list 
	 bool del(T value);
	 //print out list on screen
	 void printlist();
	 //return size of the list
	 int getsize(){return size;}
	 

 };

 template <class T>
LinkedList<T> ::LinkedList(const LinkedList<T> & list)
{
	head=new ListNode<T>; size=list.size;
	ListNode<T> *q=list.head;
	//ListNode<T> *ptr=head;
	while (q->next!=NULL)
	{q=q->next;
	 ListNode<T> *temp=new ListNode<T>;
	 temp->data=q->data;
	 temp->next=head->next;
	 head->next=temp;
	}
}

// delete the list
template <class T>
void LinkedList<T>::Dellist()
{ ListNode<T> *p=head->next;
  ListNode<T> *temp;
while (p!=NULL)
{ temp=p->next;
  delete p;
  p=temp;
 }
  head->next=NULL;
  size=1;
}

//delete a node
template <class T>
bool LinkedList<T>::del(T value)
{ ListNode<T> *p=head;
  //ListNode<T> *temp;

  while ((p->next!=NULL) && (p->next->data!=value))
  {p=p->next;}
  if (p->next==NULL) 
	  {cout<<"ListNode to delete is not in this list."<<endl;
       return false;}
  else
       {ListNode<T> *temp=p->next;
	    p->next=temp->next;
        delete temp;
		size--;
		return true;}
  }

//insert a node
template <class T>
void LinkedList<T>::insertnode(T value)
{ ListNode<T> *p;
  p= new ListNode<T>;
  p->next=head->next;
  p->data=value;
  head->next=p;
  size++;
}

//print list
template <class T>
void LinkedList<T>::printlist()
{
	ListNode<T> *p=head->next;
	while (p!=NULL)
	{cout<<p->data<<" ";
	 p=p->next;}
	cout<<endl;
}
#endif