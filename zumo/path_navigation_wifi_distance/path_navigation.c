/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * path_navigation.c
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

#include <math.h>
#include <pico/stdlib.h>
#include "path_navigation.h"
#include "move_list.h"
#include "moving-sensor_ultrasonics.h"
#include "2enginesmove.h"
#include "move_encoder.h"

int humanCommandDirection = 0;
bool humanCommand = false;

static move_list_node *avoidCommandsStartPoint = NULL;
static move_list_node *avoidCommandsCurrentPoint = NULL;


void setHumanCommand(bool isHuman) {
	humanCommand = isHuman;
}

void setHumanDirection(int value) {
	humanCommandDirection = value;
}

static bool restorePath() {
	
	return true;
}

static bool avoidInFrontObject(float targetDistance, float actualLeftDistance, float actualRightDistance) {
	
	return true;
	if ( avoidCommandsStartPoint == NULL ) {
		avoidCommandsStartPoint = avoidCommandsCurrentPoint = allocateMoveNode();
	} else {
		move_list_node *node = allocateMoveNode();
		node->previous = avoidCommandsCurrentPoint;
		avoidCommandsCurrentPoint->next = node;
		avoidCommandsCurrentPoint = node;
	}
	avoidCommandsCurrentPoint->data.rotate = 0;
	avoidCommandsCurrentPoint->data.distance = targetDistance - ( actualLeftDistance < actualRightDistance ? actualRightDistance : actualLeftDistance );
	
	//look and get distance from left
	moveServo(180);
	sleep_ms(20);
	long leftDistance = getDistance();
	moveServo(0);
	sleep_ms(20);
	long rightDistance = getDistance();
	
	if ( leftDistance < rightDistance ) { //we go right
		move_list_node *node = allocateMoveNode();
		node->previous = avoidCommandsCurrentPoint;
		avoidCommandsCurrentPoint->next = node;
		avoidCommandsCurrentPoint = node;
		avoidCommandsCurrentPoint->data.distance = 0.0;
		avoidCommandsCurrentPoint->data.rotate = 1;
	} else if ( stopDistance > leftDistance ) { //we are in lock
		
	} else { //we go left
		move_list_node *node = allocateMoveNode();
		node->previous = avoidCommandsCurrentPoint;
		avoidCommandsCurrentPoint->next = node;
		avoidCommandsCurrentPoint = node;
		avoidCommandsCurrentPoint->data.distance = 0.0;
		avoidCommandsCurrentPoint->data.rotate = -1;
	}
	return restorePath();
}



/*
 * Move the platform with distance or rotate with encoders
 */
void moveOrRotateWithDistance(float moveData, int rotateData, bool autoServo) {
	
#ifdef TEST_COMMANDS
	printf("moveOrRotateWithDistance distance=%f rotate=%d\n", moveData, rotateData);
	fflush(stdout);
	return;
#endif
	humanCommand = false;
	go(0,0);
	clearEncoders();
	if (rotateData == 0) {
#ifdef SERIAL_DEBUG_MODE
	printf("Move linear to %f\r\n", moveData);
#endif
		if (autoServo ) {
			moveServo(90);
		}
		int isInterrupted = moveWithDistance(moveData);
		if ( isInterrupted == -1 ) {
			avoidInFrontObject(moveData, 0, 0);
		} else if ( isInterrupted == 1 ) {
			avoidInFrontObject(moveData, getLeftEngineDistance(), getRightEngineDistance());
		}
	} else if (fabs(moveData) <= 0.01) {
		if (rotateData == -1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees left\r\n");
#endif
			if ( autoServo ) {
				moveServo(180);
			}
			rotate90Left();
			if ( autoServo ) {
				moveServo(90);
			}
		} else if (rotateData == 1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees right\r\n");
#endif
			if ( autoServo ) {
				moveServo(0);
			}
			rotate90Right();
			if ( autoServo ) {
				moveServo(90);
			}
		} else if (rotateData < 0) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees left\r\n",-rotateData);
#endif
			int nr = -rotateData;
			if ( autoServo ) {
				if ( (nr >= 0 && nr < 180) || ( nr <= 0 && nr > -180) )
					moveServo(nr);
			}
			rotateLeftDegree(nr);
			if ( autoServo ) {
				moveServo(90);
			}
		} else {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees right\r\n",rotateData);
#endif
			if ( autoServo ) {
				if ( (rotateData >= 0 && rotateData < 180) || ( rotateData <= 0 && rotateData > -180) ) {
					moveServo(rotateData);
				}
			}
			rotateRightDegree(rotateData);
			if ( autoServo ) {
				moveServo(90);
			}
		}
	}
}
