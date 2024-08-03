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
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/printf.h>
#include "2enginesmove.h"
#include "configuration.h"
#include "moving-sensor_ultrasonics.h"

static volatile unsigned long leftCounts;
static volatile float leftCurrentDistance = 0.0f; 
static volatile unsigned long rightCounts;
static volatile float rightCurrentDistance = 0.0f;
static int humanCommandDirection = 0;
static bool humanCommand = false;

void setHumanCommand(bool isHuman) {
	humanCommand = isHuman;
}

void setHumanDirection(int value) {
	humanCommandDirection = value;
}

unsigned long getLeftEngineCounts() {
	return leftCounts;
}

unsigned long getRightEngineCounts() {
	return rightCounts;
}

void gpio_callback(uint gpio, uint32_t events) {
	if (!humanCommand) {
		if (GPIO_IRQ_EDGE_RISE == events) {
			if (gpio == leftMotorEncoder) {
				leftCounts++;
			} else if (gpio == rightMotorEncoder) {
				rightCounts++;
			}
		}
	}
}

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

static void stopLeftEngine() {
	pwm_set_gpio_level(leftMotorPin1,LOW);
	pwm_set_gpio_level(leftMotorPin2,LOW);
}

static void stopRightEngine() {
	pwm_set_gpio_level(rightMotorPin1,LOW);
	pwm_set_gpio_level(rightMotorPin2,LOW);
}

static void moveWithDistance(float moveData) {
	moveServo(90);
	if ( moveData > 0 && hasCollision() ) {
#ifdef SERIAL_DEBUG_MODE
		printf("Stop as distance in front %ld is less than %d\n", getDistance(), stopDistance);
#endif		
		return;
	}
	bool stopLeft = false;
	bool stopRight = false;
	float distance;
	if (moveData > 0) {
		distance = moveData;
		go(currentPower,currentPower);
	} else if (moveData < 0) {
		distance = - moveData;
		go(-currentPower,-currentPower);
	} else {
		go(0,0);
		return;
	}
	while ( !stopLeft || !stopRight) {
		if ( moveData > 0 && hasCollision() ) {
			stopLeftEngine();
			stopRightEngine();
			stopLeft = true;
			stopRight = true;
		}
		if (!stopLeft) {
			leftCurrentDistance = leftCounts / leftPPI;
			if ((distance - leftCurrentDistance) <= 0.02) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			rightCurrentDistance = rightCounts / rightPPI;
			if ((distance - rightCurrentDistance) <= 0.02) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
	leftCurrentDistance = 0;
	rightCurrentDistance = 0;
}

static void rotate90Left() {
	moveServo(180);
	bool stopLeft = false;
	bool stopRight = false;	
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate 90 left with left=%ld and right=%ld\r\n", countRotate90Left, countRotate90Right);
#endif	
	go(-currentPower,currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= countRotate90Left) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= countRotate90Right) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
	moveServo(90);
}

static void rotate90Right() {
	moveServo(0);
	bool stopLeft = false;
	bool stopRight = false;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate 90 right with left=%ld and right=%ld\r\n", countRotate90Left, countRotate90Right);
#endif	
	
	go(currentPower,-currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= countRotate90Left) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= countRotate90Right) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
	moveServo(90);
}

static void rotateLeftDegree(int nr) {
	if ( (nr >= 0 && nr < 180) || ( nr <= 0 && nr > -180) )
		moveServo(nr);
	bool stopLeft = false;
	bool stopRight = false;
	long int leftTargetCounts = countRotate90Left*nr/90;
	long int rightTargetCounts = countRotate90Right*nr/90;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate %d left with left=%ld and right=%ld\r\n", nr, leftTargetCounts, rightTargetCounts);
#endif		
	go(-currentPower,currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= leftTargetCounts) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= rightTargetCounts) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
	moveServo(90);
}

static void rotateRightDegree(int nr) {
	if ( (nr >= 0 && nr < 180) || ( nr <= 0 && nr > -180) )
		moveServo(nr);
	bool stopLeft = false;
	bool stopRight = false;
	long int leftTargetCounts = countRotate90Left*nr/90;
	long int rightTargetCounts = countRotate90Right*nr/90;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate %d right with left=%ld and right=%ld\r\n", nr, leftTargetCounts, rightTargetCounts);
#endif	
	go(currentPower,-currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= leftTargetCounts) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= rightTargetCounts) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
	moveServo(90);
}

void clearEncoders() {
	leftCounts = 0;
	rightCounts = 0;
}

/*
 * Move the platform with distance or rotate with encoders
 */
void moveOrRotateWithDistance(float moveData, int rotateData) {
	
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
		moveWithDistance(moveData);
	} else if (fabs(moveData) <= 0.01) {
		if (rotateData == -1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees left\r\n");
#endif
			rotate90Left();
		} else if (rotateData == 1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees right\r\n");
#endif
			rotate90Right();
		} else if (rotateData < 0) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees left\r\n",-rotateData);
#endif
			rotateLeftDegree(-rotateData);
		} else {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees right\r\n",rotateData);
#endif
			rotateRightDegree(rotateData);
		}
	}
}
