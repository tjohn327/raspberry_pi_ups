# UPS for Raspberry Pi

![UPS Image](https://github.com/tjohn327/raspberry_pi_ups/raw/master/Assests/ups_3view.png)

## Description

An uninterruptible power supply for Raspberry Pi that can provide more than an hour of backup power and can shutdown the Pi safely.
This UPS can be used to power any 5V device with upto 3A continous current. It is based on Texas Instruments [BQ25895](http://www.ti.com/product/BQ25895) power management IC and [TPS61236P](http://www.ti.com/product/TPS61236P) boost converter IC.

This UPS can power a Raspberry Pi through the GPIO header by using it as a hat. When used as a hat, the GPIO pins 2, 3 and 4 will be utilised for I2C and interrupt signals.

![UPS Working](https://github.com/tjohn327/raspberry_pi_ups/raw/master/Assests/UPSpi.png "UPS powering a Raspberry Pi 3B+ model")

Note:  

* Do not connect two input sources together!

* The device does not have reverse polarity protection, be sure to observe the polarity marked in the battery holder.

## Specifications

* Input:  3.9V - 14V DC, 2A - 3A

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

## Setting up Raspberry Pi for use with the ups

### Enable I2C and install smbus

Run the command

```
sudo raspi-config
```

Choose Interfacing Options, then I2C and then enable.

Install smbus by running the following command:

```
sudo apt-get install -y python-smbus
```

For more information, checkout the [link](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c)

### Install Mosquitto broker (Optional)

```
sudo apt-get install -y mosquitto mosquitto-clients
sudo systemctl enable mosquitto.service
```

### Install Paho MQTT Client (Optional)

You can Install the MQTT client using PIP with the command:

```
 pip install paho-mqtt
```

More [info](http://www.steves-internet-guide.com/into-mqtt-python-client/)

## Using the UPS with Raspberry Pi

```shell
cd ~
git clone https://github.com/tjohn327/raspberry_pi_ups.git
```

* Insert the battery into the UPS making sure to follow the correct polarity mentioned on the battery holder. Installing the battery in the wrong way can damage the UPS!

* Place the UPS over the Raspberry Pi and insert the header of the UPS into the GPIO header of the Raspberry Pi.

* Connect the power source to the micro usb input of the UPS.

* Turn on the Switch on the UPS to start powering the Raspberry Pi.

* Run the script ups.py for normal ups operation.

* Use the upsmqtt.py script for sending the ups status through mqtt.

## Setup a service to run the ups script with MQTT messaging on start up

```shell
sudo nano /lib/systemd/system/ups.service
```

Copy and paste the following, make necessary changes and save the file:

```
[Unit]
 Description=UPS Service
 After=multi-user.target mosquitto.service
 Wants=mosquitto.service

 [Service]
 Type=idle
 User=pi 
 ExecStart=/usr/bin/python /home/pi/code/raspberry_pi_ups/src/upsmqtt.py 
 Restart=on-failure
 [Install]
 WantedBy=multi-user.target
```

Enable and start the service

```shell
sudo systemctl daemon-reload
sudo systemctl enable ups.service
sudo systemctl start ups.service
```

## Setup Node-Red dashboard for visualization

Install Node-Red and Node-red dashboard.
Import the flow, /src/ups_flow.json into the Node-Red environment. Deploy the flow and open the dashboard link of Node-Red.

![Dashboard](https://github.com/tjohn327/raspberry_pi_ups/raw/master/Assests/Dashboards.png "UPS Monitoring Dashboard")

