#!/usr/bin/python

import time
import os, sys
import logging
import RPi.GPIO as GPIO
from powerpi import Powerpi

logging.basicConfig(level=logging.DEBUG)

disconnectflag = False
SHUTDOWNCMD = 'sudo shutdown -H '
CANCEL_SHUTDOWNCMD = 'sudo shutdown -c'
SLEEPDELAY = 5
ppi = Powerpi()


def read_status():
        global disconnectflag
        err, status = ppi.read_status()
        if err:
            return
            
        if status["PowerInputStatus"] == "Not Connected" and disconnectflag == False :
            disconnectflag = True
            message = "echo Power Disconnected, system will shutdown in %d minutes! | wall" % (status['TimeRemaining'])
            os.system(message)
        
        if status["PowerInputStatus"] == "Connected" and disconnectflag == True :
            disconnectflag = False
            message = "echo Power Restored, battery at %d percent | wall" % (status['BatteryPercentage'])
            os.system(message)
        
        logging.debug(status)

        if(status['BatteryVoltage'] < 3.2):
                ppi.bat_disconnect()
                os.system('sudo shutdown -H  now')

def interrupt_handler(channel):
    read_status()        

if __name__=="__main__":
    
    if ppi.initialize():
        sys.exit(1)

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(4, GPIO.FALLING, callback=interrupt_handler, bouncetime=200)

    while (True):
        read_status()
                
