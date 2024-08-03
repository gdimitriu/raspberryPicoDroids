/*
 * moving-sensor_ultrasonics.h
 *
 *  Created on: Jan 30, 2024
 *      Author: Gabriel Dimitriu 2024
 */

#ifndef _MOVING_SENSOR_ULTRASONICSH_
#define _MOVING_SENSOR_ULTRASONICSH_

#include "configuration.h"

	void moveServo(unsigned int value);
	void initServoUltrasonics();
	long getDistance();
	bool hasCollision();
	void updateDistance();
#endif
