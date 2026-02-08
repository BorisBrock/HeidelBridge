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

## Powering the ESP32

The ESP32 is **not** powered by the Modbus line. You need to power it externally, usually via an USB phone charger.

## Modbus Connection And Troubleshooting

To establish a stable and reliable Modbus connection between the Heidelberg wallbox and the HeidelBridge, please consider the following recommendations:

- Use a high-quality cable, preferably Ethernet CAT5 or CAT6.
- Use a twisted pair for the RS-485 A/B signal lines.
- Connect the ground (GND) of the wallbox and the ESP32, ideally using a dedicated conductor.
- Connect the cable shield on one side only (preferably at the wallbox) to protective earth (PE) or ground (GND).
- Terminate the RS-485 bus with 120 Ω resistors at both ends of the bus (verification with a multimeter is recommended).
- Avoid unnecessarily long cable runs and keep the cable away from sources of electromagnetic interference.