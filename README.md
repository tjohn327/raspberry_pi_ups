# UPS for Raspberry Pi

![UPS Image](Assests/ups_R3_1.png "UPS powering a Raspberry Pi 3B+ model")

## Description

An uninterruptible power supply for Raspberry Pi that can provide more than an hour of backup power and can shutdown the Pi safely.
This UPS can be used to power any 5V device with up to 3A continuous current. It is based on Texas Instruments [BQ25895](http://www.ti.com/product/BQ25895) power management IC and [TPS61236P](http://www.ti.com/product/TPS61236P) boost converter IC.

This UPS can power a Raspberry Pi through the GPIO header by using it as a hat. When used as a hat, the GPIO pins 2, 3 and 4 will be utilized for I2C and interrupt signals.

Note:  

* Do not connect two input sources together!

* The device does not have reverse polarity protection, be sure to observe the polarity marked in the battery holder while inserting the battery.

## Specifications

* Input:  4.5V - 14V DC, 2A - 5A

* Input Ports: Screw Terminal, micro USB

* Output: 5V, up to 3A

* Output Ports: Screw Terminal, USB A, 40 pin GPIO header for Raspberry Pi

* Battery: Samsung INR18650-29E Recommended (Other 18650 size batteries can be used)

* Communication: I2C

## Status LEDs

* IN: Input connected or not

* STATUS: ON- Charging, OFF- Charging done, Blinking- Error

* OUT: Output on or off

## Circuit Design

* [EasyEda Project](https://easyeda.com/tjohn327/ups-for-raspberry-pi)

## Setting up Raspberry Pi for use with the UPS

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

### Install Node-Red and its extensions

Follow the [link](https://nodered.org/docs/getting-started/raspberrypi) and install Node-Red on the Raspberry Pi.

It may take a while to install. After the installation is done, install the following extensions for Node-Red.

```shell
npm install node-red-dashboard
```

Enable Node-Red to run on startup:

```shell
sudo systemctl enable nodered.service
```

### Connect UPS to Raspberry

* Insert the battery into the UPS making sure to follow the correct polarity mentioned on the battery holder. Installing the battery in the wrong way can damage the UPS!

* Turn the power switch of Power Pi to off position.

* Place the UPS over the Raspberry Pi and insert the header of the UPS into the GPIO header of the Raspberry Pi.

### Setup the UPS script

Clone the the repository.

```shell
cd ~
git clone https://github.com/tjohn327/raspberry_pi_ups.git
cd raspberry_pi_ups/
git checkout R3_1
```

Edit src/powerpi.py to select the right input current limit and battery charge current.
Default input current limit is 3.25A and charge current is 500 mA.
To change these, comment and uncomment appropriate lines in powerpi.py.
Also enter the battery capacity and approximate current draw to be expected.
Edit VBAT_LOW to change the battery low threshold. It is recommended to keep it a 3.2V.

```python
#BYTE_ILIM =  0b01101000 #2A input current limit
BYTE_ILIM =  0b01111111 #3.25A input current limit
#BYTE_ICHG =  0b00001000 #.5A charging current limit
BYTE_ICHG =  0b00010000 #1A charging current limit
BAT_CAPACITY = 2900 #Battery capacity in mAh
CURRENT_DRAW = 2000 #Current draw in mAh approxiamtely
VBAT_LOW = 3.2
```

Setup ups service:

```shell
sudo nano /lib/systemd/system/ups.service
```

Copy and paste the following, make necessary changes and save the file:

```systemd
[Unit]
Description=UPS Service
After=multi-user.target

[Service]
Type=idle
User=pi
ExecStart=/usr/bin/python /home/pi/raspberry_pi_ups/src/ups.py
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

### Setup Node-Red dashboard for visualization

Open the Node-Red link in a browser. The link is usually:

http://[IP of Raspberry Pi]:1880/

Import the flow, src/ups_flow.json into the Node-Red environment.

Edit one of the UI nodes (e.g BAT) by double clicking on it. In its setup menu click on the edit button next to "Group' and select 'Home' in the Tab drop down. Click Update and then Done.

Deploy the flow and open the dashboard link of Node-Red to see the status of the UPS.

http://[IP of Raspberry Pi]:1880/ui

![Dashboard](Assests/dashboard_R3.PNG "UPS Monitoring Dashboard")

If everything went well till now, shutdown the Pi.

## Powering the Pi through the UPS

* Connect the power source to the micro usb input or the screw terminal input of the UPS.
* Make sure that nothing is plugged in to the power input of the Raspberry Pi and turn the power switch of the UPS to ON position.
* Once the Pi is booted up open the dashboard link to view the status of the UPS.
