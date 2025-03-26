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

![Hardware connections](/docs/img/hardware_connections.png)
