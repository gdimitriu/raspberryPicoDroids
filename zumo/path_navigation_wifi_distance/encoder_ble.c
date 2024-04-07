/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * encoder_ble.c (main entry point)
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
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <pico/printf.h>
#include <string.h>
#include <stdlib.h>
#include <pico/cyw43_arch.h>

#include "2enginesmove.h"
#include "configuration.h"
#include "string_list.h"
#include "make_move.h"
#include "server_wifi.h"


int absoluteMaxPower = 65025;
int maxPower = 65025;
int currentPower = 65025;
int minPower = 30000;


/*
 * compute the PPI from the new configuration
 */
void computePPI() {
	leftPPI = leftResolutionCodor/(2*pi*whellRadius);
	rightPPI = rightResolutionCodor/(2*pi*whellRadius);
}

int main() {
	stdio_usb_init();
	//initialize UART 1
	uart_init(uart1, 38400);
	// Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
	gpio_set_function(4, GPIO_FUNC_UART);
	gpio_set_function(5, GPIO_FUNC_UART);
	uart_set_translate_crlf(uart1, 1);
#ifdef SERIAL_DEBUG_MODE
	printf("Starting...\n");
	fflush(stdout);
#endif
	gpio_set_function(leftMotorPin1, GPIO_FUNC_PWM);
	// Figure out which slice we just connected
    uint slice_num = pwm_gpio_to_slice_num(leftMotorPin1);
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
	gpio_set_function(leftMotorPin2, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(leftMotorPin2);
	config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);	
	gpio_set_function(rightMotorPin1, GPIO_FUNC_PWM);	
	slice_num = pwm_gpio_to_slice_num(rightMotorPin1);
	config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);	
	gpio_set_function(rightMotorPin2, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(rightMotorPin2);
	config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
	gpio_set_irq_enabled_with_callback(leftMotorEncoder, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
	gpio_set_irq_enabled_with_callback(rightMotorEncoder, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
	gpio_set_irq_enabled_with_callback(frontSensorPin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
	gpio_set_irq_enabled_with_callback(backSensorPin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
	//read from external EEPROM configuration
	//update configuration
	computePPI();
	commandsStartPoint = commandsCurrentPoint = NULL;
	
#ifdef SERIAL_DEBUG_MODE	
	getchar();
	printf("Started\n");
	fflush(stdout);
#endif
	 if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    
    cyw43_arch_enable_sta_mode();
#ifdef SERIAL_DEBUG_MODE
	printf("Connecting to Wi-Fi...\n");
#endif	
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
#ifdef SERIAL_DEBUG_MODE	
        printf("failed to connect.\n");
#endif		
        return 1;
    } else {
#ifdef SERIAL_DEBUG_MODE
        printf("Connected.\n");
#endif		
    }
    while(true) {
		initWifi();
	}
	cyw43_arch_deinit();
}
