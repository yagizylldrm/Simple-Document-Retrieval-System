#ifndef LinkedList_h
#define LinkedList_h

#include "Node.h"

template <class T>
class LinkedList
{
private:
	Node<T>* head;

public:
	LinkedList() : head(nullptr) {}
	
	LinkedList(const LinkedList& other) : head(nullptr)
	{
		Node<T>* walk = other.head;
		while (walk != nullptr)
		{
			push_back(walk->data);
			walk = walk->next;
		}
	}
	
	LinkedList& operator=(const LinkedList& other)
	{
		if (this != &other)
		{
			while (head != nullptr)
			{
				pop_front();
			}
			
			Node<T>* walk = other.head;
			while (walk != nullptr)
			{
				push_back(walk->data);
				walk = walk->next;
			}
		}
		return *this;
	}
	
	~LinkedList()
	{
		while (head != nullptr)
		{
			pop_front();
		}
	}
	
	Node<T>* getHead() const { return head; }
	
	void push_back(T& val)
	{
		if (head == nullptr) head = new Node<T>(val);
		else
		{
			Node<T>* walk = head;
			while (walk->next != nullptr) { walk = walk->next; }
			walk->next = new Node<T>(val);
		}
	}
	
	void push_front(T& val)
	{
		Node<T>* temp = head;
		head = new Node<T>(val);
		head->next = temp;
	}
	
	void pop_back()
	{
		if (head == nullptr) return;
		if (head->next == nullptr)
		{
			delete head;
			head = nullptr;
		}
		else
		{
			Node<T>* walk = head;
			while (walk->next->next != nullptr) { walk = walk->next; }
			delete walk->next;
			walk->next = nullptr;
		}
	}

	void pop_front()
	{
		if (head == nullptr) return;
		Node<T>* temp = head;
		head = head->next;
		delete temp;
	}
};

#endif /* LinkedList_h */
