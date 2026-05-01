#ifndef Node_h
#define Node_h

template <class T>
struct Node
{
	T data;
	Node* next;
	Node<T>(T val) : data(val), next(nullptr) {}
};

#endif /* Node_h */
