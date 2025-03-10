# HeidelBridge API Reference

## MQTT API

The following topics are published by HeidelBridge:

| Topic                                     | Unit | Data Type | Description                                                             |
| ----------------------------------------- | ---- | --------- | ----------------------------------------------------------------------- |
| {DeviceName}/version                      | -    | String    | The version of the HeidelBridge firmware (e.g. 1.0.0).                  |
| {DeviceName}/build_date                   | -    | String    | Build date of the HeidelBridge firmware.                                |
| {DeviceName}/ip_address                   | -    | String    | Currently assigned IP address of the HeidelBridge.                      |
| {DeviceName}/is_vehicle_connected         | -    | Integer   | Boolean (0 or 1) indicating if a vehicle is connected.                  |
| {DeviceName}/is_vehicle_charging          | -    | Integer   | Boolean (0 or 1) indicating if a vehicle is charging.                   |
| {DeviceName}/vehicle_state                | -    | String    | 'disconnected', 'connected' or 'charging'.                              |
| {DeviceName}/charging_current_limit       | A    | Float     | Charging current limit in Ampere.                                       |
| {DeviceName}/charging_power               | W    | Float     | Momentary charging power in Watt.                                       |
| {DeviceName}/failsafe_current             | A    | Float     | Current the wallbox will fall back to in case of a communication error. |
| {DeviceName}/energy_meter                 | kWh  | Float     | Total charged energy so far.                                            |
| {DeviceName}/temperature                  | °C   | Float     | Current wallbox PCB temperature.                                        |
| {DeviceName}/charging_current/phase1      | A    | Float     | Momentary charging current on phase 1.                                  |
| {DeviceName}/charging_current/phase2      | A    | Float     | Momentary charging current on phase 2.                                  |
| {DeviceName}/charging_current/phase3      | A    | Float     | Momentary charging current on phase 3.                                  |
| {DeviceName}/charging_voltage/phase1      | V    | Float     | Momentary charging voltage on phase 1.                                  |
| {DeviceName}/charging_voltage/phase2      | V    | Float     | Momentary charging voltage on phase 2.                                  |
| {DeviceName}/charging_voltage/phase3      | V    | Float     | Momentary charging voltage on phase 3.                                  |
| {DeviceName}/internal/uptime              | s    | Integer   | Total time this HeidelBridge has been up and running.                   |
| {DeviceName}/internal/wifi_disconnects    | -    | Integer   | Total number of WiFi connection losses since start.                     |
| {DeviceName}/internal/mqtt_disconnects    | -    | Integer   | Total number of MQTT connection losses since start.                     |
| {DeviceName}/internal/modbus_read_errors  | -    | Integer   | Total number of Modbus RTU read errors since start.                     |
| {DeviceName}/internal/modbus_write_errors | -    | Integer   | Total number of Modbus RTU write errors since start.                    |

The following topics are subscribed by HeidelBridge. Use these to control your wallbox:

| Topic                                        | Unit | Data Type | Description                                                             |
| -------------------------------------------- | ---- | --------- | ----------------------------------------------------------------------- |
| {DeviceName}/control/charging_current_limit  | A    | Float     | Charging current limit in Ampere.                                       |
