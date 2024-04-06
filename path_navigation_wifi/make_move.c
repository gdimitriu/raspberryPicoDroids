/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * make_move.c (receive and interpretate commands)
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
#include <pico/printf.h>
#include <string.h>
#include <stdlib.h>
#include "2enginesmove.h"
#include "configuration.h"
#include "string_list.h"
#include "server_wifi.h"
#include "make_move.h"

/*
 * engine settings
 */
float whellRadius = 3.25;
const float pi = 3.14f;
//left engine
unsigned int leftMotorPin1 = 15;
unsigned int leftMotorPin2 = 14;
unsigned int leftMotorEncoder = 16;
long leftResolutionCodor = 20;
float leftPPI;
//right engine
unsigned int rightMotorPin1 = 11;
unsigned int rightMotorPin2 = 10;
unsigned int rightMotorEncoder = 17;
long rightResolutionCodor = 20;
float rightPPI;
long countRotate90Left= 8;
long countRotate90Right= 8;

//sensors
unsigned int frontSensorPin = 13;
unsigned int backSensorPin = 12;

string_list_node *commandsStartPoint;
string_list_node *commandsEndPoint;
string_list_node *commandsCurrentPoint;

bool isValidInput = false;

/*
 * validate if the data is a number
 */
static bool isValidNumber(char *data, int size) {

#ifdef SERIAL_DEBUG_MODE
	printf("%s size=%d\n",data, size);
#endif	
	if (size == 0 )
		return false;
	for (int i =0 ;i < size; i++) {
		if (!(data[i] < 58 && data[i] > 47 ))
			return false;
	}
	return true;
}

/*
 * send unsupported
 */
static void sendUnsupported() {
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"unsupported\r\n");
	sendData(bufferSend);
}

/*
 * send maximum power setting
 */
static void sendMaxPower() {
	
#ifdef SERIAL_DEBUG_MODE
	printf("%d\n\r",maxPower);
#endif
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"%d\r\n",maxPower);
	sendData(bufferSend);
}

/*
 * send minimum power setting
 */
static void sendMinPower() {

#ifdef SERIAL_DEBUG_MODE
	printf("%d\n",minPower);
#endif
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"%d\r\n",minPower);
	sendData(bufferSend);
}

/*
 * send current power setting
 */
static void sendCurrentPower() {
	
#ifdef SERIAL_DEBUG_MODE
	printf("%d\n",currentPower);
#endif
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"%d\r\n",currentPower);
	sendData(bufferSend);
}

/*
 * send encoder values
 */
static void sendEncoderValues() {
	//print the encoders leftCounts
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"left: %lu right: %lu\r\n",getLeftEngineCounts(),getRightEngineCounts());
	sendData(bufferSend);
}

/*
 * reset encoders and send encoder values
 */
static void resetEncoders() {
	//print the encoders leftCounts
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"left: %lu right: %lu\r\n",getLeftEngineCounts(),getRightEngineCounts());
	sendData(bufferSend);
	//reset counters
	clearEncoders();
}

static void moveWithCommandsInDirectOrder() {
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	commandsCurrentPoint = commandsStartPoint;
	while ( commandsCurrentPoint != NULL ) {
		memset(bufferReceive, '\0', sizeof(char)*WIFI_BUFFER);
		bufferIndex = strlen(commandsCurrentPoint->data) + 1;
		strncpy(bufferReceive, commandsCurrentPoint->data, strlen(commandsCurrentPoint->data));
		makeMove(false, 0);
		commandsCurrentPoint = getNext(commandsCurrentPoint);				
	}
}

static void moveWithCommandsInReverseOrder() {
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
		commandsCurrentPoint = getPrevious(commandsCurrentPoint);
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

static bool clearCommandList() {
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

static bool commandWithoutData() {
	if (bufferReceive[0] == 'I')  {
		sendUnsupported();
		isValidInput = true;
	}
	/*
	 * 
	 * return maximum power setting
	 */
	else if (bufferReceive[0] == 'V') { 
		sendMaxPower();
	}
	/*
	 * return minimum power setting
	 */ 
	else if (bufferReceive[0] =='v') {
		sendMinPower();
	}
	/*
	 * return current power value
	 */
	else if (bufferReceive[0] =='c') {
		sendCurrentPower();
	}
	/*
	 * return minimum low power distance
	 */
	else if (bufferReceive[0] == 'd') {
		sendUnsupported();
		isValidInput = true;
	}
	/*
	 * return stop distance
	 */
	else if (bufferReceive[0] == 's') {
		sendUnsupported();
		isValidInput = true;
	}
	/*
	 * break the engines
	 */
	else if (bufferReceive[0] == 'b') {
		breakEngines();
		isValidInput = true;
		return true;
	}
	/*
	 * return encoder values
	 */
	else if (bufferReceive[0] == 'C') {		
		sendEncoderValues();
		isValidInput = true;
		return true;
	}
	/*
	 * reset encoder values
	 */
	else if (bufferReceive[0] == 'R') {
		resetEncoders();
		isValidInput = true;
		return true;
	}
	/*
	 * start with data from path in direct order
	 */
	else if ( bufferReceive[0] == 'D') {
		moveWithCommandsInDirectOrder();
	}
	/*
	 * start with data from path in reverse order
	 */
	else if ( bufferReceive[0] == 'B' ) {
		moveWithCommandsInReverseOrder();
	}
	/*
	 * clear the command list
	 */
	else if ( bufferReceive[0] == 'n' ) {
		return clearCommandList();
	}
	/*
	 * unknown command
	 */
	else {
		//sendUnsupported();
		isValidInput = false;
	}
	return true;
}

/*
 * set maximum power
 */
static bool setMaxPower() {
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	//remove V from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	if (!isValidNumber(bufferReceive, bufferIndex - 2)) {
		isValidInput = false;
		return false;
	}
	if (atol(bufferReceive) > absoluteMaxPower || atol(bufferReceive) < 0) {
		isValidInput = false;
		return false;
	}
#ifdef SERIAL_DEBUG_MODE
	printf("MaxPower=%s\n",bufferReceive);
#endif        
	maxPower = atol(bufferReceive);
	isValidInput = true;
	return true;
}

/*
 * set minimum power
 */
static bool setMinimumPower() {
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"OK\r\n");
	sendData(bufferSend);
	//remove v from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	if (!isValidNumber(bufferReceive, bufferIndex - 2)) {
		isValidInput = false;
		//do not makeCleanup();
		return false;
	}
	if (atol(bufferReceive) > maxPower || atol(bufferReceive) < 0) {
		isValidInput = false;
		//do not makeCleanup();
		return false;
	}
#ifdef SERIAL_DEBUG_MODE
	printf("MinPower=%s\n",bufferReceive);
#endif                
	minPower = atol(bufferReceive);
	isValidInput = true;
	return true;
}

static bool setCurrentPower(bool isHuman) {
	if ( isHuman ) {
		sprintf(bufferSend,"OK\r\n");
		sendData(bufferSend);
	}	
	//remove c from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	if (!isValidNumber(bufferReceive, bufferIndex - 2)) {
		isValidInput = false;
		//do not makeCleanup();
		return false;
	}
	if (atol(bufferReceive) > maxPower || atol(bufferReceive) < 0) {
		isValidInput = false;
		//do not makeCleanup();
		return false;
	}
#ifdef SERIAL_DEBUG_MODE
	printf("CurrentPower=%s\n",bufferReceive);
#endif                
	currentPower = atol(bufferReceive);
	isValidInput = true;
	return true;
}

/*
 * move until stop command
 * the robot is controlled by human
 */
static bool moveOrRotateUtilStopCommand() {
	//remove M from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	bufferReceive[strlen(bufferReceive)] = '\0';
	int position;
	for (uint8_t i = 0; i < strlen(bufferReceive); i++) {
		if (bufferReceive[i] == ',') {
			position = i;
			break;
		}
	}
	char buf[10];
	for (int i = 0; i < 10; i++) {
		buf[i] = '\0';
	}
	for (int i = 0 ; i < position; i++) {
		buf[i] = bufferReceive[i];
	}
	int moveData = atoi(buf);
	for (int i = 0; i < 10; i++) {
		buf[i] = '\0';
	}
	int idx = 0;
	for (int i = position + 1; i < strlen(bufferReceive); i++) {
		buf[idx] = bufferReceive[i];
		idx++;
	}
	int rotateData = atoi(buf);
#ifdef SERIAL_DEBUG_MODE
	printf("%s\n%d:%d\n",bufferReceive,moveData,rotateData);
#endif        
	setHumanCommand(true);
	setHumanDirection(moveData);
	if (moveData == 0 && rotateData == 0) {
		go(0,0);
	} else if (rotateData == 0) {
		if (moveData < 0) {
			go(-currentPower,-currentPower);
		} else {
			go(currentPower, currentPower);
		}
	} else {
		if (rotateData < 0) {
			go(-currentPower,currentPower);
		} else {
			go(currentPower, -currentPower);
		}
	}
	isValidInput = true;
	return true;
}

/*
 * move or rotate by value of the ecoders
 * could be by human or path
 * could be direct or reverse
 */
static bool moveOrRotateByValue(int isReverse) {
	//remove m from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	bufferReceive[strlen(bufferReceive)] = '\0';
	int position;
	for (uint8_t i = 0; i < strlen(bufferReceive); i++) {
		if (bufferReceive[i] == ',') {
			position = i;
			break;
		}
	}
	char buf[10];
	for (int i = 0; i < 10; i++) {
		buf[i] = '\0';
	}
	for (int i = 0 ; i < position; i++) {
		buf[i] = bufferReceive[i];
	}
	float moveData = atof(buf);
	for (int i = 0; i < 10; i++) {
		buf[i] = '\0';
	}
	int idx = 0;
	for (int i = position + 1; i < strlen(bufferReceive); i++) {
		buf[idx] = bufferReceive[i];
		idx++;
	}
	int rotateData = atoi(buf);
	if (isReverse == 1) {
		rotateData = -rotateData;
	} else if ( isReverse == 2) { //not implemented yet
		rotateData = -rotateData;
		moveData = -moveData;
	}
#ifdef SERIAL_DEBUG_MODE
	printf("%s\n%f:%d\n",bufferReceive,moveData,rotateData);
#endif        
	if (moveData == 0 && rotateData == 0) {
		go(0,0);
	} else {
		moveOrRotateWithDistance(moveData,rotateData);
	}
	isValidInput = true;
	return true;
}

/*
 * put path value
 */
static bool putPathValue() {
	//remove N from command
	for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
		bufferReceive[i]=bufferReceive[i+1];
	}
	bufferReceive[strlen(bufferReceive)] = '\0';
	if ( commandsCurrentPoint == NULL ) {
		commandsCurrentPoint = commandsStartPoint = allocate();
	} else {
		string_list_node *node = allocate();
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

static bool commandWithData(bool isHuman, int isReverse) {
	/*
	 * set the maximum power value
	 */
	if (bufferReceive[0] == 'V') {
		return setMaxPower();
	}
	/*
	 * set the minimum power
	 */
	else if (bufferReceive[0] == 'v') {
		return setMinimumPower();
	}
	/*
	 * set current power value
	*/
	else if (bufferReceive[0] == 'c') {
		return setCurrentPower(isHuman);
	}
	/*
	 * set the low power distance value
	 */
	else if (bufferReceive[0] == 'd') {
		sendUnsupported();
		isValidInput = true;
		return true;
	}
	/*
	 * set the stop distance value
	 */
	else if (bufferReceive[0] == 's') {
		sendUnsupported();
		isValidInput = true;
		return true;
	}
	/*
	 * move or rotate until break command
	 */
	else if (bufferReceive[0] == 'M') {
		return moveOrRotateUtilStopCommand();
	}
	/*
	 * move distance or rotate using degree or specific
	 */
	else if (bufferReceive[0] == 'm') {
		return moveOrRotateByValue(isReverse);
	}
	/*
	 * put values into the navigation path
	 */
	else if ( bufferReceive[0] == 'N' ) {
		return putPathValue();
	}
	/*
	 * command unknown
	 */
	else {
		//do not send
		//sendUnsupported();
		//do not makeCleanup();
		isValidInput = false;
		return false;
	}
    return true;
}

/*
 * make the move of the platform
 */
bool makeMove(bool isHuman, int isReverse) {
	
	if (bufferIndex > 0) {
		bufferReceive[bufferIndex] = '\0';
	}
	if (strlen(bufferReceive) == 1) {
#ifdef SERIAL_DEBUG_MODE
		printf("Received without data %s\n",bufferReceive);
#endif			
		return commandWithoutData();
	}
	/*
	 * Command with data
	 */
	else {
#ifdef SERIAL_DEBUG_MODE
		printf("Received with data %s\n",bufferReceive);
#endif		
		return commandWithData(isHuman, isReverse);
    }
}
