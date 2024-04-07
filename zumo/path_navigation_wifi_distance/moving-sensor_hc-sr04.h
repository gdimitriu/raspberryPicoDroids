/*
 * moving-sensor_hc-sr04.h
 *
 *  Created on: Jan 30, 2024
 *      Author: Gabriel Dimitriu 2024
 */

#ifndef _MOVING_SENSOR_HC_SR04H_
#define _MOVING_SENSOR_HC_SR04H_

#include "configuration.h"

	void moveServo(unsigned int value);
	void initServoHCSR04();
	long getDistance();
	bool hasCollision();
#endif
