"""
 moving robot raspberry pico and BLE

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
from machine import Pin
SSID = 'YOUR_SSID'
PASSWORD = 'YOUR_PASSWORD'
PORT = 4242
MAX_POWER = 65025
MIN_POWER = 30000
LEFT_RESOLUTION_ENCODER = 1452
WHEEL_RADIUS = 3.25
RIGHT_RESOLUTION_ENCODER = 20
COUNT_ROTATE_90_LEFT = 8
COUNT_ROTATE_90_RIGHT = 8
# left engine
leftMotorPin1 = Pin(15)
leftMotorPin2 = Pin(14)
left_motor_encoder = Pin(16, Pin.IN, Pin.PULL_UP)

# right engine
rightMotorPin1 = Pin(11)
rightMotorPin2 = Pin(10)
right_motor_encoder = Pin(18, Pin.IN, Pin.PULL_UP)

# sensors
front_sensor = Pin(13, Pin.IN, Pin.PULL_UP)
back_sensor = Pin(12, Pin.IN, Pin.PULL_UP)

DEBUG_MODE = True