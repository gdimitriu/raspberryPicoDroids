/*
 * moving robot raspberry pico w (WIFI based)
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
	
	#define WIFI_BUFFER 512
	#define WIFI_BUFFER_SEND 512
	
	#define WIFI_TCP_PORT 4242
	
	#define DEBUG_printf printf
	
	#define HIGH 1
	#define LOW 0
	
	extern char *bufferReceive;
	extern char *bufferSend;

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
	
	//commands
	extern string_list_node *commandsStartPoint;
	extern string_list_node *commandsCurrentPoint;
	
	//servo
	#define SERVO_PIN 22
	//HC-SR-4
	#define ECHO_PIN 21
	#define TRIG_PIN 20
	#define STOP_DISTANCE 50
/*	
	 * structure for the TCP server
 */
#include <lwip/pbuf.h>
#include <lwip/tcp.h>
typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    struct tcp_pcb *client_pcb;
    bool complete;
    char buffer_sent[WIFI_BUFFER_SEND];
    char buffer_recv[WIFI_BUFFER];
    int sent_len;
    int recv_len;
} TCP_SERVER_T;
	
	#define SERIAL_DEBUG_MODE true

//	#define TEST_COMMANDS true
#ifdef TEST_COMMANDS
	#define SERIAL_DEBUG_MODE true
#endif
#endif
