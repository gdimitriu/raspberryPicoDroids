/*
 * moving-sensor_hc-sr04.c
 *
 *  Created on: Jan 30, 2024
 *      Author: Gabriel Dimitriu 2024
 */

#include <pico/time.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <inttypes.h>

#include "moving-sensor_hc-sr04.h"

#define SERVO_180 3200
#define SERVO_135 2600
#define SERVO_90 1850
#define SERVO_45 1100
#define SERVO_0 500

static void setMillis(float millis) {
	pwm_set_gpio_level(SERVO_PIN, (millis/20000.f)*29062.f);
}

static void setServoMillis(float startMillis) {
	gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
	pwm_config config = pwm_get_default_config();
	pwm_config_set_clkdiv(&config, 64.f);
	pwm_config_set_wrap(&config, 29062.f);
	pwm_init(slice_num, &config, true);
	setMillis(startMillis);
}

void initServoHCSR04() {
	setServoMillis(0);
	gpio_init(TRIG_PIN);
	gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_init(ECHO_PIN);
	gpio_set_dir(ECHO_PIN, GPIO_IN);
	gpio_pull_up(ECHO_PIN);
}


void moveServo(unsigned int value) {
	if ( value == 180 ) {
		setServoMillis(SERVO_180 );
	} else if ( value == 135 ) {
		setServoMillis(SERVO_135 );
	} else if ( value == 90 ) {
		setServoMillis(SERVO_90 );
	} else if ( value == 45 ) {
		setServoMillis(SERVO_45 );
	} else if ( value == 0 ) {
		setServoMillis(SERVO_0 );
	} else  if ( value < 180 && value > 135 ) { 
		setServoMillis(SERVO_135 + (value - 135)* 13.33);
	} else if ( value < 135 && value > 90) {
		setServoMillis(SERVO_90 + (value - 90) * 16.66);
	} else if ( value > 45 && value < 90 ) {
		setServoMillis(SERVO_45 + (value - 45) * 16.66);
	} else if ( value > 0 && value < 45 ) {
		setServoMillis(SERVO_0 + value * 13.33);
	}
}

long getDistance() {
	long currentDistance = 4500;
	bool timeoutOccured = false;
	uint64_t start;
	gpio_put(TRIG_PIN, false);
	busy_wait_us(2);
	gpio_put(TRIG_PIN, true);
	busy_wait_us(10);
	gpio_put(TRIG_PIN, false);			
	while(gpio_get(ECHO_PIN) == false);
	start = time_us_64();
	while(gpio_get(ECHO_PIN) == true) {
		if ((time_us_64() - start) > 26190) {
			timeoutOccured = true;
			break;
		}
	}
	if (timeoutOccured == false) {				
		int64_t microseconds = time_us_64() - start;
		currentDistance = ((microseconds/2)/29.1) * 10; //cm * 10 to be mm
	}
	return currentDistance;
}

bool hasCollision() {
	if ( STOP_DISTANCE >= getDistance() )
		return true;
	return false;
}
