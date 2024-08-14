/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * move_list.h (a move list that is used to store path commands)
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
#ifndef _MOVE_LIST_H_
#define _MOVE_LIST_H_

#include <pico/stdlib.h>

struct move_data {
	long distance;
	long rotate;
};
typedef struct move_data move_data_list;

struct move_node {
	struct move_node *previous;
	struct move_node *next;
	move_data_list data;
};

typedef struct move_node move_list_node;
move_list_node * allocateMoveNode(void);
move_list_node *getNextMoveNode(move_list_node *theNode);
move_list_node *getPreviousMoveNode(move_list_node *theNode);
bool isAtBeginigMoveList(move_list_node *theNode);
bool isAtEndMoveList(move_list_node *theNode);

#endif
