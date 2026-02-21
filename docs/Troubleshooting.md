# Troubleshooting

This page contains useful information in case things don't work as expected.

## RS485/Modbus connection

To establish a stable and reliable Modbus connection between the Heidelberg wallbox and the HeidelBridge, please consider the following recommendations:

- Use a high-quality cable, preferably Ethernet CAT5 or CAT6.
- Use a twisted pair for the RS-485 A/B signal lines.
- Connect the ground (GND) of the wallbox and the ESP32, ideally using a dedicated conductor.
- Connect the cable shield on one side only (preferably at the wallbox) to protective earth (PE) or ground (GND).
- Terminate the RS-485 bus with 120 Ω resistors at both ends of the bus (verification with a multimeter is recommended).
- Avoid unnecessarily long cable runs and keep the cable away from sources of electromagnetic interference.

## Heidelbridge Blinking Codes

| LED Signal | Status / Cause | Action |
| :--- | :--- | :--- |
| **White** (95% on, 5% off) | External release missing. Not charging. | Enable the external release/blocking device. |
| **White** (Solid/Continuous) | Vehicle connected, charging not yet requested. | The vehicle must request the charging process. |
| **White** (Pulsing) | Vehicle is charging. | None (Normal operation). |
| **White** (Pulsing with pause) | Charging with reduced power (load management active). | None (Normal operation). |
| **6x White**, Pause, **Blue** (3 s) | Residual Current Device (RCD) has tripped. | 1. Visual inspection (Box, Cable, Car).<br>2. Disconnect charging cable for approx. 4 s (Reset).<br>3. Reconnect. |
| **6x White**, Pause, **3x Blue** (50/50) | Overtemperature. | No action required. Wait for self-test/cooling down. |
| **6x White**, Pause, **3x Blue** (90/10) | A) Over- or Undervoltage<br>B) Communication error (in load management) | **A:** No action required (Wait).<br>**B:** Check the communication line. |
| **6x White**, Pause, **6x Blue** (fast) | Internal wallbox malfunction. | 1. Disconnect cable from vehicle.<br>2. Switch off the building's circuit breaker/fuse (wait 1 min).<br>3. Switch power back on & reconnect. |