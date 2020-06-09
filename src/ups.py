#!/usr/bin/python

import time
import os, sys
import logging
import socket
import json
import signal
import RPi.GPIO as GPIO
from powerpi import Powerpi


logging.basicConfig(level=logging.INFO)

GPIO4_AVAILABLE = True
ENABLE_UDP = True
UDP_PORT = 40001
serverAddressPort   = ("127.0.0.1", UDP_PORT)
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
disconnectflag = False
ppi = Powerpi()

def read_status(clear_fault=False):
        global disconnectflag, ENABLE_UDP
        err, status = ppi.read_status(clear_fault)
        
        if err:
            time.sleep(1)
            return

        if status["PowerInputStatus"] == "Not Connected" and disconnectflag == False :
            disconnectflag = True
            message = "echo Power Disconnected, system will shutdown in %d minutes! | wall" % (status['TimeRemaining'])
            os.system(message)
        
        if status["PowerInputStatus"] == "Connected" and disconnectflag == True :
            disconnectflag = False
            message = "echo Power Restored, battery at %d percent | wall" % (status['BatteryPercentage'])
            os.system(message)
        
        if ENABLE_UDP:
            try:
                UDPClientSocket.sendto(json.dumps(status), serverAddressPort)
            except Exception as ex:
                logging.error(ex)
        
        logging.debug(status)
        
        if(status['BatteryVoltage'] < 3.2):
                ppi.bat_disconnect()
                os.system('sudo shutdown now')

def interrupt_handler(channel):
    read_status(True) 

def main():
    if ppi.initialize():
        sys.exit(1)

    if GPIO4_AVAILABLE:
        try:
            GPIO.setmode(GPIO.BCM)
            GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            GPIO.add_event_detect(4, GPIO.FALLING, callback=interrupt_handler, bouncetime=200)
        except Exception as ex:
            logging.error("Error attaching interrupt to GPIO4")
            logging.error(ex)
        
    while (True):
        read_status()

if __name__=="__main__":
    main()
                
