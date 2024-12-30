"""
 moving robot raspberry pico and wifi

 configuration (this hold the configuration and main settings of the robot)

 Copyright 2024 Gabriel Dimitriu
  This file is part of raspberryPicoDroids project.

 raspberryPicoDroids is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 raspberryPicoDroids is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with raspberryPicoDroids; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
"""
from machine import Pin, PWM
SSID = 'YOUR_SSID'
PASSWORD = 'YOUR_PASSWORD'
PORT = 4242
ABSOLUTE_MAX_POWER = 65025
MAX_POWER = 65025
MIN_POWER = 30000
LEFT_RESOLUTION_ENCODER = 20
WHEEL_RADIUS = 3.25
RIGHT_RESOLUTION_ENCODER = 20
COUNT_ROTATE_90_LEFT = 8
COUNT_ROTATE_90_RIGHT = 8
# left engine
leftMotorPin1 = PWM(Pin(15))
leftMotorPin2 = PWM(Pin(14))
left_motor_encoder = Pin(16, Pin.IN, Pin.PULL_UP)

# right engine
rightMotorPin1 = PWM(Pin(10))
rightMotorPin2 = PWM(Pin(11))
right_motor_encoder = Pin(17, Pin.IN, Pin.PULL_UP)

# sensors
front_sensor = Pin(13, Pin.IN, Pin.PULL_UP)
rear_sensor = Pin(12, Pin.IN, Pin.PULL_UP)

DEBUG_MODE = True

PWM_FREQUENCY = 4000  # 4800000 4.8Mhz
