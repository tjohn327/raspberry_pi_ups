# UPS for Raspberry Pi

![UPS Image](https://github.com/tjohn327/raspberry_pi_ups/raw/master/Assests/ups_3view.png)

## Description

An uninterruptible power supply for Raspberry Pi that can provide more than an hour of backup power and can shutdown the Pi safely.
This UPS can be used to power any 5V device with up to 3A continuous current. It is based on Texas Instruments [BQ25895](http://www.ti.com/product/BQ25895) power management IC and [TPS61236P](http://www.ti.com/product/TPS61236P) boost converter IC.

This UPS can power a Raspberry Pi through the GPIO header by using it as a hat. When used as a hat, the GPIO pins 2, 3 and 4 will be utilized for I2C and interrupt signals.

![UPS Working](https://github.com/tjohn327/raspberry_pi_ups/blob/master/Assests/UPSPi.png "UPS powering a Raspberry Pi 3B+ model")

Note:  

* Do not connect two input sources together!

* The device does not have reverse polarity protection, be sure to observe the polarity marked in the battery holder.

## Specifications

* Input:  3.9V - 14V DC, 2A - 3A

* Input Ports: Screw Terminal, micro USB

* Output: 5V, up to 3A

* Output Ports: Screw Terminal, USB A, 40 pin GPIO header for Raspberry Pi

* Battery: Samsung INR18650-29E (Other 18650 size batteries can be used)

* Communication: I2C

## Status LEDs

* IN: Input connected or not

* STATUS: ON- Charging, OFF- Charging done, Blinking- Error

* OUT: Output on or off

## Using Power Pi with a Raspberry Pi

### 1. Connect Power Pi to Raspberry pi

* Make sure the switch of Power Pi is turned off and power input is plugged into the Raspberry Pi.

* Insert the battery into the battery holder following the correct polarity.

* Connect Power Pi to Raspberry Pi by inserting it into the GPIO pins.

* Connect power to the Raspberry Pi's USB input to turn it on. (After the setup this power will be connected to Power Pi)

### 2. Set up Raspberry Pi for use with Power Pi

#### Enable I2C and install smbus

To enable I2C:
Open a terminal and run the command

```shell
sudo raspi-config
```

Choose Interfacing Options, then I2C and then select enable to enable I2C in the Raspberry Pi.

Install smbus by running the following command:

```shell
sudo apt-get install -y python-smbus
```

For more information, checkout the [link](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c)

#### Install Node-Red and its extensions (Optional)

Follow the [link](https://nodered.org/docs/getting-started/raspberrypi) and install Node-Red on the Raspberry Pi.

It may take a while to install. After the installation is done, install the following extensions for Node-Red.

```shell
npm install node-red-dashboard
```

Enable Node-Red to run on startup:

```shell
sudo systemctl enable nodered.service
sudo systemctl start nodered.service
```

### 3. Install the ups service

Clone the Power Pi repository:

```shell
cd ~
git clone https://github.com/tjohn327/raspberry_pi_ups.git
cd raspberry_pi_ups/src/
git checkout R3_1
```

Edit the file powerpi.py between lines 16 and 24 if you are not using Samsung INR18650-29E battery. It is recommended to keep the VBAT_LOW at 3.2V for Li-Ion batteries.

Run the install.sh script to install a service for the ups.

```shell
chmod +x install.sh
./install.sh
```

This creates a service named ups.service that will run on startup.
If there were no error you will see this as the output:

```shell
Checking Python
Python found
Initializing Power Pi
INFO:root:UPS initialized
Creating ups service
Enabling ups service to run on startup
ups service enabled
Power Pi configured successfully
```

Now turn off the Raspberry Pi and remove the power cable form it. Connect the power cable to Power Pi and turn the switch to ON position. This will power up the Raspberry Pi through Power Pi.

When the Pi is powered back up, check the status of the ups service by running:

```shell
sudo systemctl status ups.service
```

If everything is running correctly, you will see a status similar to this:

```shell
● ups.service - UPS Service
   Loaded: loaded (/lib/systemd/system/ups.service; enabled; vendor preset: enabled)
   Active: active (running) since Tue 2020-06-09 06:44:15 UTC; 34s ago
 Main PID: 542 (python)
    Tasks: 2 (limit: 2200)
   Memory: 6.9M
   CGroup: /system.slice/ups.service
           └─542 /usr/bin/python /home/pi/code/raspberry_pi_ups/src/ups.py

Jun 09 06:44:15 raspberrypi systemd[1]: Started UPS Service.
Jun 09 06:44:17 raspberrypi python[542]: INFO:root:UPS initialized
```

Your Power Pi is now ready.

### 4. Setup Node-Red dashboard for visualization (optional)

![Dashboard](https://github.com/tjohn327/raspberry_pi_ups/blob/master/Assests/dashboards.png "UPS Monitoring Dashboard")

Open the Node-Red link in a browser. The link is usually:

http://[IP of Raspberry Pi]:1880/

Import the flow, src/ups_flow.json into the Node-Red environment.

Edit one of the UI nodes (e.g BAT) by double clicking on it. In its setup menu click on the edit button next to "Group' and select 'Home' in the Tab drop down. Click Update and then Done.

Deploy the flow and open the dashboard link of Node-Red to see the status of the UPS.

http://[IP of Raspberry Pi]:1880/ui

More [info](https://nodered.org/docs/user-guide/editor/workspace/import-export) about importing flows and setting up [dashboards](https://flows.nodered.org/node/node-red-dashboard).
