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
import math
import ioe_sr05_pio
from servo import movecenter, move90left, move90right

direction = 0
_current_left_encoder = 0
_current_right_encoder = 0
_left_current_distance = 0.0
_right_current_distance = 0.0
_left_PPI = 0.0
_right_PPI = 0.0
_human_control = True
_stop_left = False
_stop_right = False


def set_human_control(value):
    global _human_control
    _human_control = value


def callback(pin):
    """callback for encoders"""
    global direction
    global _current_left_encoder
    global _current_right_encoder
    if configuration.DEBUG_MODE:
        print("Callback(%s)" % pin)
    if pin is configuration.left_motor_encoder:
        _current_left_encoder += 1
    elif pin is configuration.right_motor_encoder:
        _current_right_encoder += 1


def init_encoders():
    """
    initialize the encoders
    this should be called every time the move wth distance is called
    or when it is with path navigation
    """
    global _current_right_encoder
    global _current_left_encoder
    if configuration.DEBUG_MODE:
        print("InitEncoders")
    _current_left_encoder = 0
    _current_right_encoder = 0
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
    global _right_PPI
    global _left_PPI
    if configuration.DEBUG_MODE:
        print("InitEngines")
    _left_PPI = configuration.LEFT_RESOLUTION_ENCODER / (2 * math.pi * configuration.WHEEL_RADIUS)
    _right_PPI = configuration.RIGHT_RESOLUTION_ENCODER / (2 * math.pi * configuration.WHEEL_RADIUS)
    configuration.leftMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.leftMotorPin2.freq(configuration.PWM_FREQUENCY)
    configuration.rightMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.rightMotorPin2.freq(configuration.PWM_FREQUENCY)
    movecenter()


def go(left_speed=0, right_speed=0):
    global direction
    if configuration.DEBUG_MODE:
        print("Go(%g,%g)" % (left_speed, right_speed))

    if left_speed == 0 and right_speed == 0:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(0)
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("coasting")
        direction = 0
        return

    if left_speed > 0 and right_speed > 0 and ioe_sr05_pio.get_distance() <= configuration.STOP_DISTANCE:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(0)
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("All on zero because of front collision %g" % ioe_sr05_pio.get_distance())
        return

    if left_speed < 0 and right_speed < 0:
        direction = -1
    elif right_speed > 0 and left_speed > 0:
        direction = 1
    if left_speed > 0:
        configuration.leftMotorPin1.duty_u16(left_speed)
        configuration.leftMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("Left %g:0" % left_speed)
    else:
        configuration.leftMotorPin1.duty_u16(0)
        configuration.leftMotorPin2.duty_u16(-left_speed)
        if configuration.DEBUG_MODE:
            print("Left 0:%g" % -left_speed)
    if right_speed > 0:
        configuration.rightMotorPin1.duty_u16(right_speed)
        configuration.rightMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("Right %g:0" % right_speed)
    else:
        configuration.rightMotorPin1.duty_u16(0)
        configuration.rightMotorPin2.duty_u16(-right_speed)
        if configuration.DEBUG_MODE:
            print("Right 0:%g" % -right_speed)


def break_engines():
    global _stop_left
    global _stop_right
    if configuration.DEBUG_MODE:
        print("BreakEngines")
    configuration.leftMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.leftMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    _stop_left = True
    _stop_right = True


def stop_left_engine():
    configuration.leftMotorPin1.duty_u16(0)
    configuration.leftMotorPin2.duty_u16(0)


def stop_right_engine():
    configuration.rightMotorPin1.duty_u16(0)
    configuration.rightMotorPin2.duty_u16(0)


def move_with_distance(move_data, current_power, run_distance):
    global _left_current_distance
    global _right_current_distance
    global _right_PPI
    global _left_PPI
    global _current_left_encoder
    global _current_right_encoder
    global _stop_left
    global _stop_right
    if configuration.DEBUG_MODE:
        print("Moving with distance %g" % move_data)
    _left_current_distance = 0.0
    _right_current_distance = 0.0
    _stop_left = False
    _stop_right = False
    if move_data > 0:
        distance = move_data
        go(current_power, current_power)
    elif move_data < 0:
        distance = -move_data
        go(-current_power, -current_power)
    else:
        go(0, 0)
        return
    while not _stop_left or not _stop_right:
        if not _stop_left:
            _left_current_distance = _current_left_encoder / _left_PPI
            if (distance - _left_current_distance) <= 0.02:
                stop_left_engine()
                _stop_left = True
        if not _stop_right:
            _right_current_distance = _current_right_encoder / _right_PPI
            if (distance - _right_current_distance) <= 0.02:
                stop_right_engine()
                _stop_right = True
    go(0, 0)
    run_distance[0] = _left_current_distance
    run_distance[1] = _right_current_distance


def rotate_90_left(current_power):
    global _stop_left
    global _stop_right
    global _current_left_encoder
    global _current_right_encoder
    _stop_left = False
    _stop_right = False
    if configuration.DEBUG_MODE:
        print("Rotate 90 left with left=%g and right=%g" % (configuration.COUNT_ROTATE_90_LEFT,
                                                            configuration.COUNT_ROTATE_90_RIGHT))
    go(-current_power, current_power)
    while not _stop_left or not _stop_right:
        if not _stop_left:
            if _current_left_encoder >= configuration.COUNT_ROTATE_90_LEFT:
                stop_left_engine()
                _stop_left = True
        if not _stop_right:
            if _current_right_encoder >= configuration.COUNT_ROTATE_90_RIGHT:
                stop_right_engine()
                _stop_right = True
    go(0, 0)


def rotate_90_right(current_power):
    global _stop_left
    global _stop_right
    global _current_left_encoder
    global _current_right_encoder
    _stop_left = False
    _stop_right = False
    if configuration.DEBUG_MODE:
        print("Rotate 90 right with left=%g and right=%g" % (configuration.COUNT_ROTATE_90_LEFT,
                                                             configuration.COUNT_ROTATE_90_RIGHT))
    go(current_power, -current_power)
    while not _stop_left or not _stop_right:
        if not _stop_left:
            if _current_left_encoder >= configuration.COUNT_ROTATE_90_LEFT:
                stop_left_engine()
                _stop_left = True
        if not _stop_right:
            if _current_right_encoder >= configuration.COUNT_ROTATE_90_RIGHT:
                stop_right_engine()
                _stop_right = True
    go(0, 0)


def rotate_degree(rotate_data, current_power):
    global _stop_left
    global _stop_right
    global _current_left_encoder
    global _current_right_encoder
    _stop_left = False
    _stop_right = False
    if rotate_data < 0:
        _left_target_counts = configuration.COUNT_ROTATE_90_LEFT * (-rotate_data) / 90
        _right_target_counts = configuration.COUNT_ROTATE_90_RIGHT * (-rotate_data) / 90
        if configuration.DEBUG_MODE:
            print("Rotate %g left with left=%g and right=%g" % (rotate_data, _left_target_counts,
                                                                _right_target_counts))
    else:
        _left_target_counts = configuration.COUNT_ROTATE_90_LEFT * rotate_data / 90
        _right_target_counts = configuration.COUNT_ROTATE_90_RIGHT * rotate_data / 90
        if configuration.DEBUG_MODE:
            print("Rotate %g right with left=%g and right=%g" % (rotate_data, _left_target_counts,
                                                                 _right_target_counts))
    if rotate_data < 0:
        go(-current_power, current_power)
    else:
        go(current_power, -current_power)
    while not _stop_left or not _stop_right:
        if not _stop_left:
            if _current_left_encoder >= _left_target_counts:
                stop_left_engine()
                _stop_left = True
        if not _stop_right:
            if _current_right_encoder >= _right_target_counts:
                stop_right_engine()
                _stop_right = True
    go(0, 0)
