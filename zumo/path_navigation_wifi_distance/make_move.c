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

#include "configuration.h"
#include "server_wifi.h"
#include "make_move.h"
#include "2enginesmove.h"
#include "path_navigation.h"
#include "move_commands.h"
#include "move_encoder.h"

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
 * send low power distance
 */

static void sendLowPowerDistance() {
#ifdef SERIAL_DEBUG_MODE
	printf("%d\n\r",lowPowerDistance);
#endif
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"%d\r\n",lowPowerDistance);
	sendData(bufferSend);
}

/*
 * send stop distance
 */
static void sendStopDistance() {
#ifdef SERIAL_DEBUG_MODE
	printf("%d\n\r",stopDistance);
#endif
	memset(bufferSend,'\0',sizeof(char)*WIFI_BUFFER_SEND);
	sprintf(bufferSend,"%d\r\n",stopDistance);
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
		sendLowPowerDistance();
		isValidInput = true;
	}
	/*
	 * return stop distance
	 */
	else if (bufferReceive[0] == 's') {
		sendStopDistance();
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

/*
 * set lowPowerDistance
 */
static bool setLowPowerDistance() {
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
	printf("LowPowerDistance=%s\n",bufferReceive);
#endif                
	lowPowerDistance = atol(bufferReceive);
	isValidInput = true;
	return true;
}

/*
 * set stopDistance
 */
static bool setStopDistance() {
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
	printf("StopDistance=%s\n",bufferReceive);
#endif                
	stopDistance = atol(bufferReceive);
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
		moveOrRotateWithDistance(moveData,rotateData, true);
	}
	isValidInput = true;
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
		setLowPowerDistance();
		isValidInput = true;
		return true;
	}
	/*
	 * set the stop distance value
	 */
	else if (bufferReceive[0] == 's') {
		setStopDistance();
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
		return putCommand();
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
