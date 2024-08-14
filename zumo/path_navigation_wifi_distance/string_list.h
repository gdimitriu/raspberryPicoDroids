/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * string_list.h (a string list that is used to store path commands)
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
#ifndef _STRING_LIST_H_
#define _STRING_LIST_H_

#include <pico/stdlib.h>

struct string_node {
	struct string_node *previous;
	struct string_node *next;
	char *data;
};
typedef struct string_node string_list_node;
string_list_node * allocateStringNode(void);
string_list_node *getNextStringNode(string_list_node *theNode);
string_list_node *getPreviousStringNode(string_list_node *theNode);
bool isAtBeginigStringList(string_list_node *theNode);
bool isAtEndStringList(string_list_node *theNode);

#endif
