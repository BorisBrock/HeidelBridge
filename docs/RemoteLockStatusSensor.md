# Remote Lock Status Sensor and Lock Control Implementation

## Overview
This document describes the implementation of:
1. A Home Assistant sensor that reads the remote lock status from Modbus register 259 (**always available**)
2. A Home Assistant lock control that controls a relay via GPIO pin for physical lock control (**optional, enabled with build flag**)

## Build Configuration

### Available Environments

**Without Lock Relay Control (Default):**
- `lilygo` - LILYGO T-CAN485 board, Modbus sensor only
- `esp32` - Regular ESP32 board, Modbus sensor only  
- `dummy` - Dummy wallbox simulation, Modbus sensor only

**With Lock Relay Control:**
- `lilygo_with_lock_relay` - LILYGO T-CAN485 + GPIO relay control
- `esp32_with_lock_relay` - Regular ESP32 + GPIO relay control
- `dummy_with_lock_relay` - Dummy wallbox + GPIO relay control

### Build Flag: `ENABLE_LOCK_RELAY`

When this flag is defined:
- GPIO pins are configured for relay control
- Lock control MQTT topics are enabled
- Home Assistant lock entity is created
- Physical pin control is active

When this flag is **not** defined:
- Only Modbus remote lock status sensor is available
- No GPIO pins are used for relay control
- No lock control MQTT topics
- Reduced memory usage

## Implementation Details

### Part 1: Remote Lock Status Sensor (Modbus-based) - **ALWAYS AVAILABLE**

#### Modbus Register
- **Register Address**: 259
- **Data Type**: 16-bit unsigned integer
- **Values**: 
  - 0 = Locked
  - 1 = Unlocked (default as per specifications)

### Part 2: Lock Control (GPIO-based Relay Control) - **OPTIONAL WITH BUILD FLAG**

#### Hardware Setup (Only with ENABLE_LOCK_RELAY)
**LILYGO T-CAN485 Board:**
- **Control Pin**: GPIO18
- **Relay Connection**: Connect relay control input to GPIO18
- **Relay Logic**: HIGH = Unlock, LOW = Lock

**Regular ESP32 Board:**
- **Control Pin**: GPIO25
- **Relay Connection**: Connect relay control input to GPIO25  
- **Relay Logic**: HIGH = Unlock, LOW = Lock

#### MQTT Topics for Lock Control (Only with ENABLE_LOCK_RELAY)
- **Command Topic**: `{device_name}/lock_control/command`
  - Payloads: "LOCK" or "UNLOCK"
- **State Topic**: `{device_name}/lock_control/state`
  - Payloads: "LOCKED" or "UNLOCKED"

## Usage Instructions

### Option 1: Modbus Remote Lock Status Only
```bash
# Build for LILYGO without relay control
pio run -e lilygo

# Build for ESP32 without relay control  
pio run -e esp32

# Build for dummy simulation without relay control
pio run -e dummy
```

**Features Available:**
- ✅ Remote lock status sensor (from Modbus register 259)
- ❌ Physical lock control via relay

### Option 2: Modbus Remote Lock Status + Physical Lock Control
```bash
# Build for LILYGO with relay control
pio run -e lilygo_with_lock_relay

# Build for ESP32 with relay control
pio run -e esp32_with_lock_relay

# Build for dummy simulation with relay control
pio run -e dummy_with_lock_relay
```

**Features Available:**
- ✅ Remote lock status sensor (from Modbus register 259)  
- ✅ Physical lock control via relay (GPIO pin)

## Home Assistant Integration

### Entities Created

#### 1. Remote Lock Status (Binary Sensor) - **ALWAYS AVAILABLE**
- **Entity Name**: Remote Lock Status
- **Entity ID**: `binary_sensor.{device_name}_remote_lock_status`
- **Device Class**: Lock
- **Data Source**: Modbus register 259

#### 2. Lock Control (Lock Entity) - **ONLY WITH ENABLE_LOCK_RELAY**
- **Entity Name**: Lock Control  
- **Entity ID**: `lock.{device_name}_lock_control`
- **Device Class**: Lock
- **Control Method**: MQTT commands to GPIO relay

## Debugging and Troubleshooting

### Build Flag Verification
Check serial output during startup:

**Without ENABLE_LOCK_RELAY:**
```
[INFO] Lock controller disabled (ENABLE_LOCK_RELAY not defined)
```

**With ENABLE_LOCK_RELAY:**
```
[INFO] Initializing lock controller on pin 18
[INFO] Lock controller initialized successfully
```

### Expected Behavior

**Without Relay Control:**
- Only the binary sensor appears in Home Assistant
- No lock control entity is created
- No GPIO pins are configured for relay
- Commands like "UNLOCK" are ignored

**With Relay Control:**
- Both binary sensor AND lock control entities appear
- GPIO pin voltage changes when commands are sent
- Lock control responds to Home Assistant commands

## Migration Guide

### From Relay Control to Modbus-Only
1. Change build environment from `*_with_lock_relay` to base environment
2. Rebuild and upload firmware
3. The lock control entity will disappear from Home Assistant
4. The binary sensor will continue working

### From Modbus-Only to Relay Control  
1. Connect relay hardware to appropriate GPIO pin
2. Change build environment to `*_with_lock_relay`
3. Rebuild and upload firmware
4. New lock control entity will appear in Home Assistant

## Safety Considerations

- The Modbus remote lock status sensor is read-only and always safe
- Physical relay control should only be enabled when appropriate hardware is connected
- Use the appropriate build flag for your hardware configuration
- Test thoroughly before permanent installation
