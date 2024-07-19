<div align="center">
  <a href="#">
    <img src="https://raw.githubusercontent.com/BorisBrock/Heidelbridge/main/img/blueberry.svg" height="100px" />
 </a>
</div>
<h1 align="center">Heidel<span style="color:#646bafff">Bridge</span></h1>

Heidelbridge is a firmware for ESP32 microcontrollers. It allows you to bring your [Heidelberg wallbox](https://www.heidelberg-wallbox.eu) into you local WiFi network. This is done by turning your wallbox into a [OCPP](https://en.wikipedia.org/wiki/Open_Charge_Point_Protocol) compatible device. This way the wallbox can easily be integrated into home energy management systems, e.g. [evcc](https://github.com/evcc-io/evcc).

![PlatformIO CI Build](https://github.com/BorisBrock/Heidelbridge/actions/workflows/build.yml/badge.svg)
![Flawfinder Code Analysis](https://github.com/BorisBrock/Heidelbridge/actions/workflows/flawfinder.yml/badge.svg)
![Cppcheck Code Analysis](https://github.com/BorisBrock/Heidelbridge/actions/workflows/cppcheck.yml/badge.svg)
![GitHub all releases](https://img.shields.io/github/downloads/BorisBrock/Heidelbridge/total?color=blue&style=flat-square)  

---

# Project Status

:warning: This project is in a very early development stage. It is not yet ready for productive use.

---

# Required Hardware

You only need two components for this project: an ESP32 microcontroller and a MAX485 module. Both are available in large quantities and at reasonable prices on the Internet. You will also need a breadboard and a few jumper wires. All in all, it shouldn't cost you more than 10$.

Parts list:
- ESP32 microcontroller (ESP classic, ESP32 C3, C6, S2 etc.)*
- MAX485 breakout board
- 6 jumper wires
- A breadboard

This should be enough for quickly putting together a fully functioning prototype.
Of course a well designed PCB would be much nicer, but this is still work in progress. Once the design is ready, the schematics will be available *right here*.

> *\* This project is currently based on the ESP32 classic. It has not been built/tested for newer models, lice the S2 and C3. Hopefully this will change soon.*

---

# Setting Up The ESP32

## Initial Programming

For the initial programming, clone or download this repository. Then compile the project using PlatformIO.
Now connect your ESP32 via USB and upload the firmware.

## WiFi Setup

On first boot, the ESP32 will open up a Wifi access point. Connect to that network with any smartphone or laptop.

- **SSID**: `HeidelBridge`
- **Password**: `heidelbridge`

From the landing page, you can now connect to your home network. This connection will be stored permanenetly.

> [!IMPORTANT]
> In addition to the WiFi SSID/password, you also need to enter the address of your OCPP server, e.g. evcc. This address must include the protocol (ws://), the server's IP address and the port (usually :8887). Here's an example: `ws://192.168.178.90:8887`.

## Updating The Firmware

Once the firmware has been uploaded to the ESP32 via USB, it can be updated via OTA (Over The Air). This makes updating the software very convenient because no more physical access to the device is required.

For this purpose, the platformio.ini contains two environments: OTA and USB.

## Resetting Connection Settings

If you need to reset/change the settings of your HeidelBridge ESP32, simply erase its flash memory. With the next reboot you can access the WiFi setup page again and make the required changes.

---

# Hardware Installation

## Configuring The Heidelberg Wallbox

1. Disconnect the wallbox from the power supply.
2. Open the housing.
3. Set DIP switch S4 pin 4 to 1 (this sets the Modbus ID to 1).
4. Set DIP switch S6 pin 2 to 1 (this enables the 120 Ohm termination).

## Connecting the Hardware Components

The hardware connections are very simple:
- ESP32's GND to MAX485-board GND
- ESP32's 3.3 V to MAX485-board VCC
- ESP32 pin 18 to MAX485-board RO pin
- ESP32 pin 19 to MAX485-board DI pin
- ESP32 pin 21 to MAX485-board DE+RE pins
- MAX485-board A terminal to Heidelberg wallbox A terminal
- MAX485-board B terminal to Heidelberg wallbox B terminal

![Hardware connections](/img/hardware_connections.png)

---

# Setting Up evcc

> [!IMPORTANT]
> Please make sure that you set up your HeidelBridge **before** adding it to evcc. Evcc won't start up, if it can't find your HeidelBridge! 

Adding your HeidelBerry to evcc is very straight forward.
Start by defining a new charger:

```
chargers:
  - name: heidelberg_ec
    type: template
    template: ocpp
    timeout: 2m # optional
```

Next, add a loadpoint:

```
loadpoints:
  - title: Heidelberg EC
    charger: heidelberg_ec
    mode: off
    guardduration: 5m
```

Aaaaaand you're done!
Start evcc with your new configuration and the HeidelBridge should be there.

---

# Contribution

:heart: Help is welcome! Do you own a Heidelberg Energy Control wallbox? Are you a Modbus expert? Do you have ideas for improvements? Did you find a bug? Feel free to review the code, create pull requests, open issues or contact me directly.

---

# Used Assets and Libraries

The following assets and libraries are used by this project:

- [Blueberry logo](https://www.vecteezy.com/free-vector/blueberry)
- [eModbus Arduino library](https://github.com/eModbus/eModbus)
- [Async TCP Arduino library](https://github.com/mathieucarbou/AsyncTCP)
- [ESP Async Webserver Arduino library](https://github.com/HenkHoldijk/mathieucarbou_ESPAsyncWebServer)
- [Websockets Arduino library](https://github.com/Links2004/arduinoWebSockets)
- [MicroOCPP Arduino library](https://github.com/matth-x/MicroOcpp)