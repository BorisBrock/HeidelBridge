# Remote Lock Control Implementation

## Overview
This document describes the implementation of a remote lock control feature that allows Home Assistant to control a relay connected to the wallbox's remote lock mechanism via MQTT.

## Hardware Configuration

### Pin Assignments

#### LILYGO T-CAN485 Board
- **Lock Relay Pin**: GPIO18
- **Logic**: HIGH = Unlocked, LOW = Locked
- **Default State**: Locked (LOW)

#### Regular ESP32 Board
- **Lock Relay Pin**: GPIO25
- **Logic**: HIGH = Unlocked, LOW = Locked
- **Default State**: Locked (LOW)

### Relay Connection
Connect a relay module to the designated GPIO pin:
- VCC → 3.3V or 5V (depending on relay module)
- GND → GND
- IN → GPIO pin (GPIO18 for LILYGO, GPIO25 for ESP32)
- NO/COM → Connected to wallbox remote lock terminals

## Software Implementation

### Components Added

#### 1. Board Configuration Updates
- **Board.h/Board.cpp**: Added `GetPinLockRelay()` method
- **BoardLilygo.cpp**: Configured GPIO18 as lock relay pin
- **BoardESP32.cpp**: Configured GPIO25 as lock relay pin

#### 2. LockController Component (`src/Components/LockController/`)
- **LockController.h**: Interface for lock control functionality
- **LockController.cpp**: Implementation with singleton pattern
- **Methods**:
  - `Init()`: Initialize the lock relay pin
  - `SetLockState(bool unlocked)`: Control relay state
  - `GetLockState()`: Get current lock state

#### 3. MQTT Integration Updates
- **Discovery Topic**: `homeassistant/lock/{device_name}/lock_control/config`
- **Command Topic**: `{device_name}/lock_control/command`
- **State Topic**: `{device_name}/lock_control/state`
- **Payloads**:
  - Commands: "LOCK" / "UNLOCK"
  - States: "LOCKED" / "UNLOCKED"

#### 4. Main Application Updates
- Added LockController initialization in `setup()`

## Home Assistant Integration

The lock control will appear in Home Assistant as:
- **Entity Type**: Lock
- **Entity Name**: "Lock Control"
- **Entity ID**: `lock.{device_name}_lock_control`
- **Device Class**: Lock
- **Commands**: Lock/Unlock buttons
- **State Feedback**: Shows current lock state

### MQTT Topics

#### Discovery Configuration Topic:
```
homeassistant/lock/{device_name}/lock_control/config
```

#### Command Topic (Home Assistant → HeidelBridge):
```
{device_name}/lock_control/command
```
Payloads: "LOCK" or "UNLOCK"

#### State Topic (HeidelBridge → Home Assistant):
```
{device_name}/lock_control/state
```
Payloads: "LOCKED" or "UNLOCKED"

## Usage Instructions

### Setup
1. Connect a relay module to the appropriate GPIO pin
2. Wire the relay output to the wallbox remote lock terminals
3. Deploy the firmware to the ESP32
4. The lock control will automatically appear in Home Assistant

### Operation
1. In Home Assistant, find the "Lock Control" entity
2. Use the lock/unlock buttons to control the relay
3. The state will be reflected in real-time
4. Default state is locked on boot

## Technical Details

### Relay Logic
- **Active HIGH**: GPIO HIGH = Relay ON = Lock UNLOCKED
- **Active LOW**: GPIO LOW = Relay OFF = Lock LOCKED
- **Default**: Locked state on startup for security

### Error Handling
- If MQTT connection fails, lock state remains unchanged
- Pin initialization occurs during board setup
- State is maintained in memory and published periodically

### Logging
- Lock state changes are logged at INFO level
- MQTT command reception logged at TRACE level
- Pin initialization logged at INFO level

## Safety Considerations

1. **Default Locked**: System defaults to locked state for security
2. **Power Loss**: On power loss/restart, lock returns to locked state
3. **GPIO Selection**: Chosen pins are safe and don't conflict with existing functionality
4. **Relay Type**: Use appropriate relay module rated for wallbox voltage/current

## Testing

To test the lock control feature:
1. Monitor serial output for lock controller initialization
2. Check Home Assistant for the new lock entity
3. Test lock/unlock commands from Home Assistant interface
4. Verify relay operation with multimeter
5. Check MQTT state topic updates

## Troubleshooting

### Lock not appearing in Home Assistant
- Check MQTT connection and discovery messages
- Verify Home Assistant MQTT integration is enabled
- Check device name configuration

### Relay not switching
- Verify GPIO pin wiring
- Check relay module power supply
- Monitor serial logs for state changes
- Test GPIO pin with multimeter

### Commands not working
- Check MQTT subscription topics
- Verify payload format (LOCK/UNLOCK)
- Monitor MQTT logs in Home Assistant
