/*
 * moving robot raspberry pico and BLE
 * 
 * communication.c (special communication)
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
#include <string.h>
#include <stdlib.h>
#include <hardware/uart.h>

#include "communication.h"

int bufferIndex = 0;
char inChar = '\0';
char bufferReceive[BUFFER];
char bufferSend[BUFFER_SEND];

/*
 * clean the input buffers
 */
void makeCleanup() {
	for (bufferIndex = 0; bufferIndex < BUFFER; bufferIndex++) {
		bufferReceive[bufferIndex] = '\0';
	}
	bufferIndex = 0;
	inChar ='\0';
}

void sendData(char *buffer) {
	uart_puts(uart1,buffer);
}
