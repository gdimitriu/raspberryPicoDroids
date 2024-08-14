/*
 * moving-sensor_ioe_sr05.c
 *
 *  Created on: August 2, 2024
 *      Author: Gabriel Dimitriu 2024
 */

#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <inttypes.h>
#include <pico/mutex.h>
#include "uart_rx.pio.h"
#include <hardware/pio.h>
#include "moving-sensor_ultrasonics.h"

#define SERVO_180 3200
#define SERVO_135 2600
#define SERVO_90 1850
#define SERVO_45 1100
#define SERVO_0 500

static mutex_t distanceMutex;
static volatile long currentDistance = MAX_RANGE_SENSOR;
int stopDistance = 5;
int lowPowerDistance = 25; 
static PIO pio = pio0;
static uint sm = 0;

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
	gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_put(TRIG_PIN, true);
}

void initServoUltrasonics() {
	setServoMillis(0);
	gpio_init(TRIG_PIN);
	gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_init(ECHO_PIN);
	gpio_set_dir(ECHO_PIN, GPIO_IN);
	gpio_pull_up(ECHO_PIN);
	mutex_init(&distanceMutex);
	// Set up the state machine we're going to use to receive them.
    uint offset = pio_add_program(pio, &uart_rx_program);
    uart_rx_program_init(pio, sm, offset, ECHO_PIN, 9600);
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
	long distance;
	mutex_enter_blocking(&distanceMutex);
	distance = currentDistance;
	mutex_exit(&distanceMutex);
	return distance;
}

void updateDistance() {
	uint8_t S_DATA = 0;
	uint8_t H_DATA = 0;
	uint8_t L_DATA = 0;
	uint8_t SUM = 0;

	uint16_t distance = 0;
	gpio_put(TRIG_PIN, false);
	S_DATA = uart_rx_program_getc(pio, sm);
	if ( S_DATA == 0xFF ) {
		H_DATA = uart_rx_program_getc(pio, sm);
		L_DATA = uart_rx_program_getc(pio, sm);
		SUM = uart_rx_program_getc(pio, sm);
	}
	gpio_put(TRIG_PIN, true);
	if ( S_DATA == 0xFF && H_DATA == 0xAA && L_DATA == 0xAA && SUM == 53 ) {
		distance = MAX_RANGE_SENSOR;
	} else {		
		uint16_t sum = S_DATA + H_DATA + L_DATA;
		sum = sum << 8;
		sum = sum >> 8; 
		if ( SUM == sum ) {
			distance = H_DATA * 256  +  L_DATA;
		}
	}
	//divide by 10 to be cm
	mutex_enter_blocking(&distanceMutex);
	currentDistance = distance/10;
	mutex_exit(&distanceMutex);
}

bool hasCollision() {
	bool collision = false;
	mutex_enter_blocking(&distanceMutex);
	if ( stopDistance >= currentDistance )
		collision = true;
	mutex_exit(&distanceMutex);
	return collision;
}
