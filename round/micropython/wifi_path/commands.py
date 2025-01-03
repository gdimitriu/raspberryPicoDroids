"""
 moving robot raspberry pico and wifi

 commands (the actual driver of the commands received from wifi)

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
from engines import break_engines, go, set_human_control, move_with_distance
from engines import rotate_90_left, rotate_90_right, rotate_degree
from engines import init_encoders, disable_encoders
import configuration
import _thread


class Command:
    def __init__(self, sock):
        self.current_power = configuration.MAX_POWER
        self.min_power = configuration.MIN_POWER
        self.max_power = configuration.MAX_POWER
        self.sock = sock

    def move_data(self, request):
        """
        move the robot according to the request
        :param request: socket
        """
        if configuration.DEBUG_MODE:
            print("MoveData(%s)" % request)
        if len(request) == 1:
            self.command_without_data(request)
        else:
            self.command_with_data(request.decode("ascii"))

    def command_without_data(self, request):
        if configuration.DEBUG_MODE:
            print("CommandWithoutData(%s)" % request)
        if request == b"v":
            self.sock.send("%g\r\n" % self.min_power)
        elif request == b'V':
            self.sock.send("%g\r\n" % self.max_power)
        elif request == b'c':
            self.sock.send("%g\r\n" % self.current_power)
        elif request == b'd':
            self.sock.send("unsupported\r\n")
        elif request == b's':
            self.sock.send("unsupported\r\n")
        elif request == b'b':
            break_engines()

    def command_with_data(self, request, is_auto=False):
        if configuration.DEBUG_MODE:
            print("CommandWithData(%s)" % request)
        if request[0] == 'V':
            value = int(request[1:])
            if value <= configuration.ABSOLUTE_MAX_POWER:
                self.max_power = value
            self.sock.send("OK\r\n")
        elif request[0] == 'v':
            self.min_power = int(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 'c':
            if self.max_power >= int(request[1:]) >= self.min_power:
                self.current_power = int(request[1:])
                self.sock.send("OK\r\n")
            else:
                self.sock.send("OK\r\n")
        elif request[0] == 'M':
            set_human_control(True)
            values = request[1:].split(",")
            if len(values) == 2:
                rotate_data = int(values[1])
                move_data = int(values[0])
                if move_data == 0 and rotate_data == 0:
                    go(0, 0)
                elif move_data == 0:
                    if rotate_data < 0:
                        go(-self.current_power, self.current_power)
                    else:
                        go(self.current_power, -self.current_power)
                elif rotate_data == 0:
                    if move_data > 0:
                        go(self.current_power, self.current_power)
                    else:
                        go(-self.current_power, -self.current_power)
        elif request[0] == 'm':
            set_human_control(False)
            init_encoders()
            values = request[1:].split(",")
            if len(values) == 2:
                rotate_data = int(values[1])
                move_data = float(values[0])
                if abs(move_data) <= 0.01 and rotate_data == 0:
                    go(0, 0)
                elif rotate_data == 0:
                    run_distance = {}
                    move_with_distance(move_data, self.current_power, run_distance)
                    if not is_auto:
                        self.sock.send("I%f,%f\r\n" % (run_distance[0], run_distance[1]))
                else:
                    if rotate_data == -90:  # left
                        rotate_90_left(self.current_power)
                    elif rotate_data == 90:  # right
                        rotate_90_right(self.current_power)
                    else:
                        rotate_degree(rotate_data, self.current_power)
            disable_encoders()
        else:  # unsupported command
            self.sock.send("OK\r\n")
