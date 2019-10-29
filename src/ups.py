#!/usr/bin/python

import smbus
import time
import os, sys
import logging
import RPi.GPIO as GPIO

logging.basicConfig(level=logging.DEBUG)

bus = smbus.SMBus(1)    # 1 = /dev/i2c-1 (port I2C1)
ADDRESS = 0x6a      #I2C address of the ups
#Refer to http://www.ti.com/lit/ds/symlink/bq25895.pdf for register maps

### Initialization ###############################################
REG_WATCHDOG = 0x07
BYTE_WATCHDOG_STOP =  0b10001101 #Stop Watchdog timer
REG_ILIM = 0x00 #ILIM register
BYTE_ILIM =  0b01101000 #2A input current limit
REG_ICHG = 0x04 
BYTE_ICHG =  0b00001000 #.5A charging current limit
REG_CONV_ADC = 0x02
REG_BATFET = 0x09
BYTE_BATFET = 0b01001000 #delay before battery is disconnected

try:
    bus.write_byte_data(ADDRESS, REG_WATCHDOG, BYTE_WATCHDOG_STOP)
    bus.write_byte_data(ADDRESS, REG_ILIM, BYTE_ILIM)
    bus.write_byte_data(ADDRESS, REG_ICHG, BYTE_ICHG)
    bus.write_byte_data(ADDRESS, REG_BATFET, BYTE_BATFET)
    logging.info("UPS initialized")
except:
    logging.error("Initialization failed, check connection to the UPS!")
    sys.exit(1) 
###################################################################


BAT_CAPACITY = 2900 #Battery capacity in mah
CURRENT_DRAW = 2000 #Current draw in mah
REG_CONV_ADC = 0x02
BYTE_CONV_ADC_START = 0b10011101
BYTE_CONV_ADC_STOP = 0b00011101
REG_BATFET_DIS = 0x09
BYTE_BATFET_DIS = 0b01101000
REG_STATUS = 0x0B #address of status register
REG_BATV = 0x0e
REG_FAULT = 0x0c

disconnectflag = False
shutdowncmd = 'sudo shutdown -H '
cancelshutdowncmd = 'sudo shutdown -c'
batpercentprev = 0
SLEEPDELAY = 10


def int_to_bool_list(num):
    return [bool(num & (1<<n)) for n in range(8)]


def translate(val, in_from, in_to, out_from, out_to):
    out_range = out_to - out_from
    in_range = in_to - in_from
    in_val = val - in_from
    val=(float(in_val)/in_range)*out_range
    out_val = out_from+val
    return out_val

def read_status():
    global SLEEPDELAY
    global disconnectflag
    global batpercentprev
    try:
        bus.write_byte_data(ADDRESS, REG_CONV_ADC, BYTE_CONV_ADC_START)
        sample = bus.read_byte_data(ADDRESS, REG_STATUS)
        status = int_to_bool_list(sample)
        time.sleep(1.2)
        sample = bus.read_byte_data(ADDRESS, REG_BATV)
        batvbool = int_to_bool_list(sample)
        bus.write_byte_data(ADDRESS, REG_CONV_ADC, BYTE_CONV_ADC_STOP)
        
    except :
        logging.error("An exception occured while reading values from the UPS!")

    else :   
        if status[2]:
            power = "Connected"
        else:
            power = "Not Connected"

        if status[3] and status[4]:
            charge = "Charging done"
        elif status[4] and  not status[3]:
            charge = "Charging"
        elif not status[4] and status[3]:
            charge = "Pre-Charge"
        else:
            charge = "Not Charging"
     
        #convert batv register to volts
        batv = 2.304
        batv += batvbool[6] * 1.280
        batv += batvbool[5] * 0.640
        batv += batvbool[4] * 0.320
        batv += batvbool[3] * 0.160
        batv += batvbool[2] * 0.08
        batv += batvbool[1] * 0.04
        batv += batvbool[0] * 0.02   

        batpercent = translate(batv,3.4,4.184,0,1)
        if batpercent<0 :
            batpercent = 0
        elif batpercent >1 :
            batpercent = 1
        
        timeleftmin = int( batpercent * 60* BAT_CAPACITY / CURRENT_DRAW)
        if timeleftmin < 0 :
            timeleftmin = 0
        
        if power == "Connected" :
            timeleftmin = -1        
        
        if power == "Not Connected" and disconnectflag == False :
            disconnectflag = True
            message = "echo Power Disconnected, system will shutdown in %d minutes! | wall" % (timeleftmin)
            os.system(message)
        
        if power == "Connected" and disconnectflag == True :
            disconnectflag = False
            message = "echo Power Restored, battery at %d percent | wall" % (batpercentprev * 100)
            os.system(message)

        batpercentprev = batpercent

        data = { 
            'PowerInput': power,
            'ChargeStatus' : charge,
            'BatteryVoltage' : '%.2f'%batv,
            "BatteryPercentage" : int(batpercent*100),
            'TimeRemaining' : int(timeleftmin)
        }
        
        logging.debug(data)

        if(batv < 3.4):
                bus.write_byte_data(ADDRESS, REG_BATFET_DIS, BYTE_BATFET_DIS)
                os.system('sudo shutdown -H  now')

def interrupt_handler(channel):
    read_status()        

read_status()
GPIO.setmode(GPIO.BCM)
GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(4, GPIO.FALLING, callback=interrupt_handler, bouncetime=200)

while (True):
    time.sleep(SLEEPDELAY)
    read_status()
                
