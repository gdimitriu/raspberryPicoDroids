/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * string_list.c (a string list that is used to store path commands)
 * 
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of raspberryPicoDroids project.

 * raspberryPicoDroids is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * raspberryPicoDroids is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with raspberryPicoDroids; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
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
