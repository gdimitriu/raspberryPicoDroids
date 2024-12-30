"""
 moving robot raspberry pico and BLE

 wifi (the main entry point for wifi communication)

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
import network
import socket
import configuration
import time
from commands import Command


def init_wifi():
    """
    initialize the wifi connection
    this should be called only once at startup
    :return: socket for communication
    """
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(configuration.SSID, configuration.PASSWORD)

    max_wait = 10
    while max_wait > 0:
        if wlan.status() < 0 or wlan.status() >= 3:
            break
        max_wait -= 1
        print('waiting for connection...')
        time.sleep(1)

    if wlan.status() != 3:
        raise RuntimeError('net connection failed')
    else:
        print('connected')
        status = wlan.ifconfig()
        print('ip = ' + status[0])

    addr = socket.getaddrinfo('0.0.0.0', configuration.PORT)[0][-1]

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(addr)
    sock.listen(1)
    if configuration.DEBUG_MODE:
        print('listening on', addr)
    return sock


def receive_data(sock):
    """
    this is the main entry point for receiving command from the robot control center
    this should be called in infinite loop on core 0
    :param sock:  the socket for communication received from init_wifi
    """
    # Listen for connections
    try:
        cl, addr = sock.accept()
        if configuration.DEBUG_MODE:
            print('client connected from', addr)
    except OSError as e:
        cl.close()
        if configuration.DEBUG_MODE:
            print('connection closed')
        return
    command = Command(cl)
    try:
        while True:
            request = cl.readline(1024)
            if configuration.DEBUG_MODE:
                print(request)
            if request != b'':
                command.move_data(request[:-2])
            else:
                cl.close()
                if configuration.DEBUG_MODE:
                    print('connection closed')
                return
    except OSError as e:
        cl.close()
        if configuration.DEBUG_MODE:
            print('connection closed')
        return
