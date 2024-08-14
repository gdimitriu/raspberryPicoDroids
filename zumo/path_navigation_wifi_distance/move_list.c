/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * move_list.c (a move list that is used to store path commands)
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

#include "move_list.h"
#include <stdlib.h>
#include <string.h>

move_list_node *allocateMoveNode(void) {
	move_list_node * theNode;
	theNode = calloc(1, sizeof(move_list_node));
	theNode-> next = NULL;
	theNode->previous = NULL;
	return theNode;
}

move_list_node *getNextMoveNode(move_list_node *theNode) {
	return theNode->next;
}

move_list_node *getPreviousMoveNode(move_list_node *theNode) {
	return theNode->previous;
}

bool isAtBeginigMoveList(move_list_node *theNode) {
	if ( theNode->previous == NULL )
		return true;
	return false;
}

bool isAtEndMoveList(move_list_node *theNode) {
	if ( theNode->next == NULL ) 
		return true;
	return false;
}
