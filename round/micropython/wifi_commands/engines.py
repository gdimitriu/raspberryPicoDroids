"""
 moving robot raspberry pico and wifi

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
from machine import Pin
import configuration

direction = 0


def callback(pin):
    """callback for sensors"""
    global direction
    if configuration.DEBUG_MODE:
        print("Callback(%s)" % pin)
    if pin is configuration.front_sensor:
        if direction == 1:
            go(0, 0)
    elif pin == configuration.rear_sensor:
        if direction == -1:
            go(0, 0)


def init_engines():
    """
    initialize the engines and sensors
    this should be called only once
    """
    if configuration.DEBUG_MODE:
        print("InitEngines")
    configuration.front_sensor.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    configuration.rear_sensor.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    configuration.leftMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.leftMotorPin2.freq(configuration.PWM_FREQUENCY)
    configuration.rightMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.rightMotorPin2.freq(configuration.PWM_FREQUENCY)


def go(leftSpeed=0, rightSpeed=0):
    global direction
    if configuration.DEBUG_MODE:
        print("Go(%g,%g)" % (leftSpeed, rightSpeed))

    if leftSpeed == 0 and rightSpeed == 0:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(0)
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("coasting")
        direction = 0
        return

    if leftSpeed > 0 and rightSpeed > 0 and configuration.front_sensor.value() == 0:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(0)
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("All on zero because of front collision")
        return

    if leftSpeed < 0 and rightSpeed < 0 and configuration.rear_sensor.value() == 0:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(0)
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("All on zero because of rear collision")
        return
    if leftSpeed < 0 and rightSpeed < 0:
        direction = -1
    elif rightSpeed > 0 and leftSpeed > 0:
        direction = 1
    if leftSpeed > 0:
        configuration.leftMotorPin1.duty_u16(leftSpeed)
        configuration.leftMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("Left %g:0" % leftSpeed)
    else:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(-leftSpeed)
        if configuration.DEBUG_MODE:
            print("Left 0:%g" % -leftSpeed)
    if rightSpeed > 0:
        configuration.rightMotorPin1.duty_u16(rightSpeed)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("Right %g:0" % rightSpeed)
    else:
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(-rightSpeed)
        if configuration.DEBUG_MODE:
            print("Right 0:%g" % -rightSpeed)


def break_engines():
    if configuration.DEBUG_MODE:
        print("BreakEngines")
    configuration.leftMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.leftMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
