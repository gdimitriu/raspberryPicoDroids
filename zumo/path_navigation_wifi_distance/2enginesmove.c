/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * 2enginesmove.c (actual driving the engines)
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
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/printf.h>
#include "2enginesmove.h"
#include "configuration.h"


/*
 * engine settings
 */

//left engine
unsigned int leftMotorPin1 = 15;
unsigned int leftMotorPin2 = 14;
unsigned int leftMotorEncoder = 16;

//right engine
unsigned int rightMotorPin1 = 10;
unsigned int rightMotorPin2 = 11;
unsigned int rightMotorEncoder = 17;

void breakEngines() {
	pwm_set_gpio_level(leftMotorPin1,absoluteMaxPower);
    pwm_set_gpio_level(leftMotorPin2,absoluteMaxPower);
    pwm_set_gpio_level(rightMotorPin1,absoluteMaxPower);
    pwm_set_gpio_level(rightMotorPin2,absoluteMaxPower);
}

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {

#ifdef TEST_COMMANDS
	printf("Move with left=%d right=%d\n", speedLeft, speedRight);
	fflush(stdout);
	return;
#endif
	if (speedLeft == 0 && speedRight == 0 ) {
		pwm_set_gpio_level(leftMotorPin1,LOW);
		pwm_set_gpio_level(leftMotorPin2,LOW);
		pwm_set_gpio_level(rightMotorPin1,LOW);
		pwm_set_gpio_level(rightMotorPin2,LOW);
#ifdef SERIAL_DEBUG_MODE    
		printf("All on zero\n");
#endif
		return;
	}

	if (speedLeft > 0) {
		pwm_set_gpio_level(leftMotorPin1, speedLeft);
		pwm_set_gpio_level(leftMotorPin2, 0);
#ifdef SERIAL_DEBUG_MODE
		printf("Left %d,0\n",speedLeft);
#endif
	} 
	else {
		pwm_set_gpio_level(leftMotorPin1, 0);
		pwm_set_gpio_level(leftMotorPin2, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
		printf("Left 0,%d\n",-speedLeft);
#endif
	}
 
	if (speedRight > 0) {
		pwm_set_gpio_level(rightMotorPin1, speedRight);
		pwm_set_gpio_level(rightMotorPin2, 0);
#ifdef SERIAL_DEBUG_MODE
		printf("Right %d,0\n",speedRight);
#endif
	}
	else {
		pwm_set_gpio_level(rightMotorPin1, 0);		
		pwm_set_gpio_level(rightMotorPin2, -speedRight);
#ifdef SERIAL_DEBUG_MODE
		printf("Right 0,%d\n",-speedRight);
#endif
	}
}

void stopLeftEngine() {
	pwm_set_gpio_level(leftMotorPin1,LOW);
	pwm_set_gpio_level(leftMotorPin2,LOW);
}

void stopRightEngine() {
	pwm_set_gpio_level(rightMotorPin1,LOW);
	pwm_set_gpio_level(rightMotorPin2,LOW);
}

