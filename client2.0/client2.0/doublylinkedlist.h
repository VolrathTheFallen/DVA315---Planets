#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

/* Doubly Linked List implementation */
#include <stdio.h>
#include <stdlib.h>
#include "wrapper.h"

// Definition of abstract node that holds planet data and pointers to previous and next planets.
struct Node {
	planet_type data;
	struct Node* next;
	struct Node* prev;
} node;

struct Node* head; // global variable - pointer to head node.

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(planet_type );

//Inserts a Node at head of doubly linked list
void InsertAtHead(planet_type );

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(planet_type );

//Removes a node containing a planet
int removeNode(planet_type *);

#endif /* DOUBLYLINKEDLIST_H */