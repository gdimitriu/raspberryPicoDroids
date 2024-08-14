
#include <hardware/gpio.h>
#include "move_encoder.h"
#include "configuration.h"
#include "2enginesmove.h"
#include "moving-sensor_ultrasonics.h"

/*
 * encoder settings
 */
long leftResolutionCodor = 1452;
float leftPPI;
float whellRadius = 2.00;
const float pi = 3.14f;
long rightResolutionCodor = 1468;
float rightPPI;
unsigned long countRotate90Left= 841;
unsigned long countRotate90Right= 830;
static volatile unsigned long leftCounts;
static volatile float leftCurrentDistance = 0.0f; 
static volatile unsigned long rightCounts;
static volatile float rightCurrentDistance = 0.0f;


void clearEncoders() {
	leftCounts = 0;
	rightCounts = 0;
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

unsigned long getLeftEngineCounts() {
	return leftCounts;
}

unsigned long getRightEngineCounts() {
	return rightCounts;
}

float getLeftEngineDistance() {
	return leftCurrentDistance;
}

float getRightEngineDistance() {
	return rightCurrentDistance;
}


void rotate90Left() {
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
}

void rotate90Right() {
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
}

void rotateLeftDegree(int nr) {
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
	
}

void rotateRightDegree(int nr) {
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
}



int moveWithDistance(float moveData) {
	if ( moveData > 0 && hasCollision() ) {
#ifdef SERIAL_DEBUG_MODE
		printf("Stop as distance in front %ld is less than %d\n", getDistance(), stopDistance);
#endif
		return -1;
	}
	bool stopLeft = false;
	bool stopRight = false;
	leftCurrentDistance = 0;
	rightCurrentDistance = 0;
	float distance;
	if (moveData > 0) {
		distance = moveData;
		go(currentPower,currentPower);
	} else if (moveData < 0) {
		distance = - moveData;
		go(-currentPower,-currentPower);
	} else {
		go(0,0);
		return 0;
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
	if ( hasCollision() ) {
		return 1;
	}
}
