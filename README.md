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

Here's what the hardware setup looks like:

---

# Installation

## Programming the ESP32

todo

## Connecting the Hardware

todo

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