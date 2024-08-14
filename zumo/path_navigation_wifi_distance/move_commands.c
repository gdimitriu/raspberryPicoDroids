/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * move_commands.c (deal with path commands)
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
#include <pico/stdlib.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"
#include "server_wifi.h"
#include "string_list.h"
#include "make_move.h"

static string_list_node *commandsStartPoint = NULL;
static string_list_node *commandsEndPoint = NULL;
static string_list_node *commandsCurrentPoint = NULL;

void moveWithCommandsInDirectOrder() {
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	commandsCurrentPoint = commandsStartPoint;
	while ( commandsCurrentPoint != NULL ) {
		memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
		bufferIndex = strlen(commandsCurrentPoint->data) + 1;
		strncpy(bufferReceive, commandsCurrentPoint->data, strlen(commandsCurrentPoint->data));
		makeMove(false, 0);
		commandsCurrentPoint = getNextStringNode(commandsCurrentPoint);
	}
}

void moveWithCommandsInReverseOrder() {
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	//change the power
	int previousPower = currentPower;
	currentPower /= 2;
	if ( currentPower < minPower ) {
		currentPower = minPower;
	}
	//rotate 180 degree
	memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
	strncpy(bufferReceive, "m0,1", 4);
	bufferIndex = 5;
	makeMove(false, 0);
	memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
	strncpy(bufferReceive, "m0,1", 4);
	bufferIndex = 5;
	makeMove(false, 0);
	currentPower = previousPower;
	commandsCurrentPoint = commandsEndPoint;
	while ( commandsCurrentPoint != NULL ) {
		memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
		bufferIndex = strlen(commandsCurrentPoint->data) + 1;
		strncpy(bufferReceive, commandsCurrentPoint->data, strlen(commandsCurrentPoint->data));
		makeMove(false, 1);
		commandsCurrentPoint = getPreviousStringNode(commandsCurrentPoint);
	}
	//change the power
	previousPower = currentPower;
	currentPower /= 2;
	if ( currentPower < minPower ) {
		currentPower = minPower;
	}
	//rotate 180 degree
	memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
	strncpy(bufferReceive, "m0,1", 4);
	bufferIndex = 5;
	makeMove(false, 0);
	memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
	strncpy(bufferReceive, "m0,1", 4);
	bufferIndex = 5;
	makeMove(false, 0);
	currentPower = previousPower;
}

/*
 * put command into list
 */
bool putCommand() {
	//remove N from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	bufferReceive[strlen(bufferReceive)] = '\0';
	if ( commandsCurrentPoint == NULL ) {
		commandsCurrentPoint = commandsStartPoint = allocateStringNode();
	} else {
		string_list_node *node = allocateStringNode();
		node->previous = commandsCurrentPoint;
		commandsCurrentPoint->next = node;
		commandsCurrentPoint = node;
	}
	commandsCurrentPoint->data = calloc(strlen(bufferReceive) + 1, sizeof(char));
#ifdef SERIAL_DEBUG_MODE
	printf("Added command: %s\n",bufferReceive);
#endif			
	strncpy(commandsCurrentPoint->data, bufferReceive, strlen(bufferReceive));
	commandsEndPoint = commandsCurrentPoint;
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	return true;
}

bool clearCommandList() {
	if ( commandsCurrentPoint == NULL ) {
		memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
		sprintf(bufferSend,"OK\r\n");
		sendData(bufferSend);
		return true;
	}
	commandsCurrentPoint = commandsStartPoint;
	commandsStartPoint = commandsStartPoint -> next;
	while ( commandsCurrentPoint != NULL ) {				
		if ( commandsCurrentPoint -> data != NULL ) {
			free(commandsCurrentPoint->data);
		}
		free(commandsCurrentPoint);
		commandsCurrentPoint = commandsStartPoint;
		if ( commandsStartPoint != NULL ) {
			commandsStartPoint = commandsStartPoint -> next;
		}
	}
	commandsCurrentPoint = commandsStartPoint = commandsEndPoint = NULL;
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	return true;
}
