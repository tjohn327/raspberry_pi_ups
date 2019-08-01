# UPS for Raspberry Pi

## Description

An uninterruptible power supply for Raspberry Pi that can provide more than an hour of backup power and can shutdown the Pi safely.
This UPS can be used to power any 5V device with upto 3A continous current. It is based on Texas Instruments [BQ25895](http://www.ti.com/product/BQ25895) power management IC and [TPS61236P](http://www.ti.com/product/TPS61236P) boost converter IC.

This UPS can power a Raspberry Pi through the GPIO header by using it as a hat. When used as a hat, the GPIO pins 2, 3 and 4 will be utilised for I2C and interrupt signals.

Note: 

* Do not connect two input sources together!

* The device does not have reverse polarity protection, be sure to observe the polarity marked in the battery holder.



## Specifications

* Input:  5V - 14V DC, 2A - 3A

* Input Ports: Screw Terminal, micro USB

* Output: 5V, upto 3A

* Output Ports: Screw Terminal, USB A, 40 pin GPIO header for Raspberry Pi

* Battery: Samsung INR18650-29E (Other 18650 size batteries can be used)

* Communication: I2C

## Status LEDs

* IN: Input connected or not

* STATUS: ON- Charging, OFF- Charging done, Blinking- Error

* OUT: Output on or off

## Circuit Design

* [EasyEda Project](https://easyeda.com/tjohn327/ups-for-raspberry-pi)

## Using the UPS with Raspberry Pi

* [Enable I2C Communication in the raspberry pi and install smbus.](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c)

* If you need to send the UPS status through mqtt, [install](http://www.steves-internet-guide.com/into-mqtt-python-client/) paho mqtt client.

* Run the script ups.py for normal ups operation.

* Use the upsmqtt.py script for sending the ups status through mqtt.
