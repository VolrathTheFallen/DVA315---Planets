/* Doubly Linked List implementation */
#include <stdio.h>
#include <stdlib.h>
#include "doublylinkedlist.h"
#include "wrapper.h"


//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(planet_type x) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Inserts a Node at head of doubly linked list
void InsertAtHead(planet_type x) {
	struct Node* newNode = GetNewNode(x);
	if (head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head;
	head = newNode;
}

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(planet_type x) {
	struct Node* temp = head;
	struct Node* newNode = GetNewNode(x);
	if (head == NULL) {
		head = newNode;
		return;
	}
	while (temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
}

//Removes a node containing a planet, returns 1 if planet removed successfully, 0 if it failed to remove the planet
int removeNode(planet_type *planet) {

	struct Node* current = head;
	struct Node* del = NULL;
	int count = 0; //the index of the node we're currently looking at

	// Base case
	if (head == NULL || planet == NULL)
		return 0;

	// Loop through ll to find which planet to delete and get the pointer to it
	while (current != NULL)
	{
		if (strcmp(current->data.name, planet->name) == 0)
			del = current;
		count++;
		current = current->next;
	}
 
	// If node to be deleted is head node
	if (strcmp(head->data.name, del->data.name) == 0)
	{
		del = head;

		if (head->next != NULL)
		{
			head = head->next;
		}
		else
		{
			head = NULL;
		}
	}
	else
	{
		// Change next only if node to be deleted is NOT the last node
		if (del->next != NULL)
			del->next->prev = del->prev;

		// Change prev only if node to be deleted is NOT the first node
		if (del->prev != NULL)
			del->prev->next = del->next;
	}
 
	// Finally, free the memory occupied by de
	free(del);

	// If function reaches here it has succeeded the deletion
	return 1;

}