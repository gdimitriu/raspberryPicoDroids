/*
 * string_list.c
 *
 *  Created on: 18 April, 2023
 *      Author: Gabriel Dimitriu 2023
 */
#include "string_list.h"
#include <stdlib.h>

string_list_node *allocate(void) {
	string_list_node * theNode;
	theNode = calloc(1, sizeof(string_list_node));
	theNode-> next = NULL;
	theNode->previous = NULL;
	theNode->data = NULL;
	return theNode;
}

string_list_node *getNext(string_list_node *theNode) {
	return theNode->next;
}

string_list_node *getPrevious(string_list_node *theNode) {
	return theNode->previous;
}

bool isAtBeginig(string_list_node *theNode) {
	if ( theNode->previous == NULL )
		return true;
	return false;
}

bool isAtEnd(string_list_node *theNode) {
	if ( theNode->next == NULL ) 
		return true;
	return false;
}
