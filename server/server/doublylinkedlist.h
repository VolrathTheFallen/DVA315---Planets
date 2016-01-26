/* Doubly Linked List implementation */
#include <stdio.h>
#include <stdlib.h>

struct Node  {
	int data;
	struct Node* next;
	struct Node* prev;
};

struct Node* head; // global variable - pointer to head node.

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(int );

//Inserts a Node at head of doubly linked list
void InsertAtHead(int );

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(int );

//Prints all the elements in linked list in forward traversal order
void Print();

//Prints all elements in linked list in reverse traversal order. 
void ReversePrint();

//
//int main() {
//
//	/*Driver code to test the implementation*/
//	head = NULL; // empty list. set head as NULL. 
//
//	// Calling an Insert and printing list both in forward as well as reverse direction. 
//	InsertAtTail(2); Print(); ReversePrint();
//	InsertAtTail(4); Print(); ReversePrint();
//	InsertAtHead(6); Print(); ReversePrint();
//	InsertAtTail(8); Print(); ReversePrint();
//
//}
