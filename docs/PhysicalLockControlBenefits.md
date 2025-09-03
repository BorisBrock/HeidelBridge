# Physical Lock Control with Relay Switch

## Overview

The HeidelBridge firmware supports an optional physical lock control feature using a relay switch. This provides a hardware-level safety mechanism that physically controls the wallbox's lock mechanism, working alongside the existing Modbus-based current control.

## What Does the Lock Control Do?

The physical lock control uses a relay to control the wallbox's **physical lock mechanism**. When activated, it prevents the charging cable from being inserted or removed, and the wallbox will report this lock state via Modbus register 259.

**Important**: The wallbox itself remains powered and operational - this control only affects the physical locking mechanism, not the power supply.

## Key Benefits

### 1. **Fail-Safe Operation** 
**This is the main advantage**: If the ESP32 loses power, crashes, or Modbus communication fails, the relay defaults to the **locked state**.

- **Modbus current limiting**: If communication fails, wallbox may continue with last known settings or default behavior
- **Physical lock relay**: Always defaults to "locked" when ESP32 is disconnected - **guaranteed safe state**

### 2. **Reliable Lock Control**
- **Hardware-level control**: Direct relay control is more reliable than software-only Modbus commands
- **Immediate response**: No communication delays or potential Modbus timeouts
- **Independent of wallbox firmware**: Works regardless of wallbox software state

### 3. **True Safety Backup**
When something goes wrong with your home automation system:
- ESP32 loses power → Relay opens → Wallbox locks → No new charging sessions can start
- Network failure → No communication needed for lock to engage
- Software crash → Hardware relay maintains safe state

## Comparison: Software vs Hardware Control

| Scenario | Modbus Current Control | Physical Lock Relay |
|----------|----------------------|-------------------|
| **Normal Operation** | ✅ Fast, flexible | ✅ Reliable |
| **ESP32 Power Loss** | ⚠️ Wallbox uses defaults | ✅ **Locks automatically** |
| **Network Failure** | ⚠️ No new commands possible | ✅ **Maintains safe state** |
| **Modbus Timeout** | ⚠️ May retry or timeout | ✅ **Lock remains active** |
| **Software Crash** | ⚠️ Unknown state | ✅ **Defaults to locked** |

## Use Cases

### **Primary Use Case: Fail-Safe Protection**
The main benefit is having a **guaranteed safe default**: if anything goes wrong with your automation system, the wallbox automatically locks and prevents new charging sessions.

### **Secondary Use Cases:**
- **Maintenance safety**: Physically prevent charging during wallbox maintenance
- **Emergency situations**: Immediate, reliable way to prevent charging
- **Simple on/off control**: Clear physical state that doesn't rely on communication

## Implementation Details

### Hardware Connection
- **ESP32**: Relay control on GPIO25
- **LILYGO T-CAN485**: Relay control on GPIO18
- **Relay Logic**: 
  - HIGH = Unlocked (charging allowed)
  - LOW = Locked (charging prevented) - **Default when ESP32 is off**

### Wiring
Connect the relay contacts to the wallbox's lock control input (refer to your wallbox manual for specific connection details).

## Home Assistant Integration

The physical lock appears as a standard lock entity:
- **Entity**: `lock.{device_name}_lock_control`
- **States**: 🔒 Locked / 🔓 Unlocked
- **Default**: Locked (when ESP32 is disconnected)

## Best Practice: Use Both Controls

For optimal safety and flexibility:

1. **Modbus current control** for normal operation:
   - Dynamic current adjustment
   - Fast response to changing conditions
   - Integration with smart home systems

2. **Physical lock relay** as safety backup:
   - Guaranteed safe state if communication fails
   - Simple emergency stop functionality
   - Peace of mind for unattended operation

## Conclusion

The physical lock control provides a simple but crucial safety feature: **it defaults to the safe state when the ESP32 is disconnected**. While Modbus control offers flexibility for normal operation, the hardware relay ensures that if your home automation system fails, the wallbox will automatically lock and prevent unexpected charging sessions.

This fail-safe behavior makes it an excellent complement to software-based controls, providing reliability where it matters most.
