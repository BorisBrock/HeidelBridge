<div align="center">
  <a href="#">
    <img src="https://raw.githubusercontent.com/BorisBrock/Heidelbridge/main/docs/img/blueberry.svg" height="100px" />
 </a>
</div>
<h1 align="center">Heidel<span style="color:#646bafff">Bridge</span></h1>

HeidelBridge is a firmware for ESP32 microcontrollers. It allows you to bring your [Heidelberg wallbox](https://www.heidelberg-wallbox.eu) into your WiFi network. This is done by turning your wallbox into a [Daheimladen](https://www.daheimladen.de/) compatible device. This way the wallbox can easily be integrated into home energy management systems like [evcc](https://github.com/evcc-io/evcc).

![PlatformIO CI Build](https://github.com/BorisBrock/Heidelbridge/actions/workflows/build.yml/badge.svg)
![Flawfinder Code Analysis](https://github.com/BorisBrock/Heidelbridge/actions/workflows/flawfinder.yml/badge.svg)
![Cppcheck Code Analysis](https://github.com/BorisBrock/Heidelbridge/actions/workflows/cppcheck.yml/badge.svg)

---

![graph](/docs/img/graph.svg)

---

# Required Hardware

You only need two components for this project: an ESP32 microcontroller and a MAX485 module. Both are available in large quantities and at reasonable prices on the Internet. You will also need a breadboard and a few jumper wires. All in all, it shouldn't cost you more than 10€.

Parts list:
- ESP32 microcontroller*
- MAX485 breakout board
- 6 jumper wires
- A breadboard

This should be enough for quickly putting together a fully functioning prototype.
Of course a well designed PCB would be much nicer, but this is still work in progress. Once the design is ready, the schematics will be available *right here*.

> *\* This project is currently based on the classic ESP32. It has not been built/tested for newer models, like the S2 and C6.*

# Getting Started

To get your very own HeidelBridge up and running follow these guides:
- Step 1: [Programming the ESP32](/docs/SoftwareSetup.md)
- Step 2: [Hardware Setup](/docs/HardwareSetup.md)

# Keeping Your HeidelBridge Up-To-Date

Heidelbridge supports OTA (Over The Air) updates. That means that you can update the device firmware via WiFi without physical access to the device. This process is described here: [updating your HeidelBridge](/docs/SoftwareSetup.md#updating-your-heidelbridge).

# Using HeidelBridge With evcc

Adding your HeidelBridge to evcc is very straight forward.
See [these instructions](/docs/evcc.md) for details.

# Using HeidelBridge With Home Assistant

HeidelBridge can easily be integrated into Home Assistant. Check out [this guide](/docs/HomeAssistant.md) to get started.

GUI Example:

![graph](/docs/img/home_assistant.png)

---

# Available APIs

The HeidelBridge offers a MQTT API and a REST API. You can find more details about both APIs in the [HeidelBridge API Reference](/docs/APIReference.md).

# Web Endpoints

The following web endpoints are currently served by HeidelBridge:

| URL         | Function                    |
| ----------- | --------------------------- |
| {ip}        | Configuration web interface |
| {ip}/update | OTA update web interface    |

# Contribution

:heart: Help is welcome! Do you own a Heidelberg Energy Control wallbox? Are you a Modbus expert? Do you have ideas for improvements? Did you find a bug? Feel free to review the code, create pull requests, open issues or contact me directly.


# Used Assets and Libraries

The following assets and libraries are used by this project:

- [Blueberry logo](https://www.vecteezy.com/free-vector/blueberry)
- [eModbus Arduino library](https://github.com/eModbus/eModbus)
- [Async TCP Arduino library](https://github.com/mathieucarbou/AsyncTCP)
- [ESP Async Webserver Arduino library](https://github.com/HenkHoldijk/mathieucarbou_ESPAsyncWebServer)
- [Websockets Arduino library](https://github.com/Links2004/arduinoWebSockets)
- [Async MQTT Client](https://github.com/marvinroger/async-mqtt-client)
- [Daheimladen Modbus API](https://www.daheimladen.de/post/modbus-api)
