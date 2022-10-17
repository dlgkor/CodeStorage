#include<iostream>
#include<string.h>
#include<windows.h>
using namespace std;

template <typename T>
class Node{
public:
	Node<T>* link;
	T data;
};

template <typename T>
class LinkedList{
private:
	Node<T>* head;
	Node<T>* cur;
	int size;
public:
	LinkedList(){
		head = new Node<T>;
		head->link = NULL;
		cur = head;
		size = 0;
	}
	void LFirst(){
		cur = head;
	}
	int LNext(T* Data){
		if(cur->link != NULL){
			cur = cur->link;
			*Data = cur->data;
			return 1;
		}
		return 0;		
	}
	int AddData(T Data){
		Node<T>* newNode = new Node<T>;
		newNode->data = Data;
		newNode->link = head->link;
		head->link = newNode;
		size++;
	}
	int GetSize(){
		return size;
	}
	~LinkedList(){
		Node<T>* nextNode;
		cur = head;
		for(int i=0;i<size;i++){
			nextNode = cur->link;
			delete cur;
		}
	}
};

class Tree{
	
};

class Item{
public:
	char* tag;
	int baseStat; //Basic status 
public:
	Item(const char* _tag){
		tag = (char*)malloc(sizeof(char)*20);
		strcpy(tag,_tag);
		
		baseStat = 0;
	}
	~Item(){
		free(tag);
	}	
};

class ItemCopy{
public:
	Item* content;
	int price; //Revised Value
public:
	ItemCopy(Item* _content):content(_content){}
};

int main(void){
	Item shield("shield");
	Item sword("sword");
	
	ItemCopy shield1(&shield); shield1.price = 1000;
	ItemCopy shield2(&shield); shield2.price = 2000;
	ItemCopy sword1(&sword); sword1.price = 3500;
	ItemCopy sword2(&sword); sword2.price = 6000;
	
	
	LinkedList<ItemCopy*> LL;
	
	LL.AddData(&shield1);
	LL.AddData(&shield2);
	LL.AddData(&sword1);
	LL.AddData(&sword2);
	
	ItemCopy* data=NULL;
	
	LL.LFirst();
	
	for(int i=0;i<LL.GetSize();i++){
		LL.LNext(&data);
		cout<<data->content->tag << ": "<< data->price<<endl;
	}
	
	return 0;
}
