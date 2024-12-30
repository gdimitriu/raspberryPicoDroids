"""
 moving robot raspberry pico and BLE

 engines (actual driving the engines)

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
import configuration

front_sensor_pin_state = 1
back_sensor_pin_state = 1


def callback(pin):
    """callback for sensors and encoders"""
    if pin == configuration.front_sensor:
        global front_sensor_pin_state
        front_sensor_pin_state ^= 1
    elif pin == configuration.back_sensor:
        global back_sensor_pin_state
        back_sensor_pin_state ^= 1


def init_encoders():
    """
    initialize the encoders
    this should be called every time the move wth distance is called
    or when it is with path navigation
    """
    if configuration.DEBUG_MODE:
        print("InitEncoders")
    configuration.left_motor_encoder.irq(trigger=Pin.IRQ_RISING, handler=callback)
    configuration.right_motor_encoder.irq(trigger=Pin.IRQ_RISING, handler=callback)


def disable_encoders():
    """
    disable the encoders
    :return:
    """
    if configuration.DEBUG_MODE:
        print("DisableEncoders")
    configuration.left_motor_encoder.irq(handler=None)
    configuration.right_motor_encoder.irq(handler=None)


def init_engines():
    """
    initialize the engines and sensors
    this should be called only once
    """
    if configuration.DEBUG_MODE:
        print("InitEngines")
    configuration.front_sensor.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=callback)
    configuration.back_sensor.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=callback)


def go(leftSpeed=0, rightSpeed=0):
    if configuration.DEBUG_MODE:
        print("Go(%g,%g)" % (leftSpeed, rightSpeed))


def break_engines():
    if configuration.DEBUG_MODE:
        print("BreakEngines")
