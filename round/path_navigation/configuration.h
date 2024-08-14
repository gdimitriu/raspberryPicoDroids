/*
 * moving robot raspberry pico and BLE
 * 
 * configuration.h (configuration of the robot and share data)
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
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

	#include "string_list.h"
	
	#define SERIAL_DEBUG_MODE true
	
	
	#define BUFFER 256
	#define BUFFER_SEND 512
	
	#define HIGH 1
	#define LOW 0

	extern char bufferReceive[BUFFER];
	extern char bufferSend[BUFFER_SEND];

	extern int bufferIndex;
	extern char inChar;
	
	extern int bufferIndex;
	extern int absoluteMaxPower;
	extern int maxPower;
	extern int currentPower;
	extern int minPower;
	/*
	 * engine settings
	 */
	extern float whellRadius;
	extern const float pi;
	//left engine
	extern unsigned int leftMotorPin1;
	extern unsigned int leftMotorPin2;
	extern unsigned int leftMotorEncoder;
	extern long leftResolutionCodor;
	extern float leftPPI;
	//right engine
	extern unsigned int rightMotorPin1;
	extern unsigned int rightMotorPin2;
	extern unsigned int rightMotorEncoder;
	extern long rightResolutionCodor;
	extern float rightPPI;
	extern long countRotate90Left;
	extern long countRotate90Right;
	//sensors
	extern unsigned int frontSensorPin;
	extern unsigned int backSensorPin;
	
	//commands
	extern string_list_node *commandsStartPoint;
	extern string_list_node *commandsCurrentPoint;
#endif
