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
- ESP32 microcontroller (ESP classic, ESP32 C3, C6, S2 etc.)
- MAX485 breakout board
- 6 jumper wires
- A breadboard

This should be enough for quickly putting together a fully functioning prototype.
Of course a well designed PCB would be much nicer, but this is still work in progress. Once the design is ready, the schematics will be available *right here*.

---

# Installation

## Step 1: Program the ESP32

For the initial programming, clone or download this repository. Then compile the project using PlatformIO.
Now connect your ESP32 via USB and upload the firmware.

Hint: All further updates can now be uploaded via OTA.

On first boot, the ESP32 will open a Wifi network called "HeidelBridge". Connect to that network with any smartphone or laptop. The Wifi password is "heidelbridge". From the landing page, you can now connect to your home network. This connection will be stored permanenetly.

## Step 2: Prepare The Heidelberg Energy Control Wallbox

todo

## Connecting the Hardware Components

The hardware connection is very simple:
- Connect the ESP32's GND to the MAX485-board's GND
- Connect the ESP32's 3.3 V to the MAX485-board's VCC
- Connect ESP32 pin 18 to the MAX485-board's RO pin
- Connect ESP32 pin 19 to the MAX485-board's DI pin
- Connect ESP32 pin 21 to the MAX485-board's DE+RE pins
- Connect the MAX485-board's A terminal to the Heidelberg wallbox' A terminal
- Connect the MAX485-board's B terminal to the Heidelberg wallbox' B terminal

![Hardware connections](/img/hardware_connections.png)

## Setting Up evcc

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

# Attribution

- [Blueberry logo](https://www.vecteezy.com/free-vector/blueberry)
- [eModbus Arduino library](https://github.com/eModbus/eModbus)
- [Async TCP Arduino library](https://github.com/mathieucarbou/AsyncTCP)
- [ESP Async Webserver Arduino library](https://github.com/HenkHoldijk/mathieucarbou_ESPAsyncWebServer)
- [Websockets Arduino library](https://github.com/Links2004/arduinoWebSockets)
- [MicroOCPP Arduino library](https://github.com/matth-x/MicroOcpp)