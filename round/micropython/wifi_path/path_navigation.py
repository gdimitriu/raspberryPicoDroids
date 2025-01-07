"""
 moving robot raspberry pico and wifi

 path navigation module

 Copyright 2025 Gabriel Dimitriu
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

import configuration
import os


class PathNavigation:
    def __init__(self, commands):
        self.commands = commands
        self.commands.set_path_navigation(self)
        self.current_path_data = []
        self.current_path_file = None

    def move_direct(self):
        for command in self.current_path_data:
            if configuration.DEBUG_MODE:
                print("MoveDirect(%s)" % command)
            command = command.encode("ascii")
            self.commands.move_data(command)

    def move_reverse(self):
        self.commands.move_data(b"m0,180")
        for command in reversed(self.current_path_data):
            if configuration.DEBUG_MODE:
                print("MoveReverse(%s)" % command)
            if command[0] == 'm':
                values = command[1:].split(",")
                if values[1] != 0:
                    values[1] = - int(values[1])
                    command_new = "m" + values[0] + "," + str(values[1])
                    command_new = command_new.encode("ascii")
                    if configuration.DEBUG_MODE:
                        print("RotateReverse(%s)" % command_new)
                    self.commands.move_data(command_new)
                    continue
            command = command.encode("ascii")
            self.commands.move_data(command)
        self.commands.move_data(b"m0,180")

    def move_direct_with_file(self, file):
        self.current_path_file = file

    def move_reverse_with_file(self, file):
        self.current_path_file = file

    def add_path_command(self, command):
        if configuration.DEBUG_MODE:
            print("AddPathCommand(%s)" % command)
        self.current_path_data.append(command + "\n")

    def load_path_file(self, file):
        pass

    def save_path_file(self, file):
        pass

    def remove_path_file(self, file):
        os.remove(file)

    def clear_navigation(self):
        self.current_path_data = []
