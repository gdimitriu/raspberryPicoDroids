/*
 * string_list.h
 *
 *  Created on: 18 April, 2023
 *      Author: Gabriel Dimitriu 2023
 */
#ifndef _STRING_LIST_H_
#define _STRING_LIST_H_

#include <pico/stdlib.h>

struct node {
	struct node *previous;
	struct node *next;
	char *data;
};
typedef struct node string_list_node;
string_list_node * allocate(void);
string_list_node *getNext(string_list_node *theNode);
string_list_node *getPrevious(string_list_node *theNode);
bool isAtBeginig(string_list_node *theNode);
bool isAtEnd(string_list_node *theNode);

#endif
