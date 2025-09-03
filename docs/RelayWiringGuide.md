# Relay Wiring Guide for Physical Lock Control

## Overview

This guide explains how to wire a relay module to control the physical lock mechanism of your Heidelberg wallbox using the HeidelBridge system.

## Required Components

- **Relay Module**: 5V relay module with opto-isolation (recommended)
- **Jumper Wires**: For connections between ESP32/LILYGO and relay module
- **Multimeter**: For testing and verification

### Recommended Relay Module Specifications
- **Type**: Single channel 5V relay module with opto-isolation
- **Trigger**: 3.3V compatible (most modules work with ESP32's 3.3V output)
- **Contacts**: At least 5A @ 250VAC rating (though wallbox lock circuits are typically low voltage)
- **Form**: SPDT (Single Pole, Double Throw) or SPST (Single Pole, Single Throw)

## Pin Connections

### Control Side (ESP32/LILYGO to Relay Module)

| ESP32 Board | LILYGO Board | Relay Module | Function |
|-------------|--------------|--------------|----------|
| GPIO25      | GPIO18       | IN / Signal  | Control signal |
| 5V or 3.3V  | 5V or 3.3V   | VCC         | Power supply |
| GND         | GND          | GND         | Ground reference |

#### Wiring Diagram - Control Side
```
ESP32/LILYGO Board          Relay Module
┌─────────────────┐         ┌──────────────┐
│                 │         │              │
│     GPIO25/18 ──┼─────────┼── IN/Signal  │
│                 │         │              │
│     VCC (5V) ────┼─────────┼── VCC        │
│                 │         │              │
│     GND ─────────┼─────────┼── GND        │
│                 │         │              │
└─────────────────┘         └──────────────┘
```

### Contact Side (Relay to Wallbox)

The relay contacts connect to the wallbox's remote lock control connector.

#### Typical Wallbox Lock Connector Pinout
**⚠️ Important**: Check your specific wallbox manual for exact pinout!

Common configurations:
- **Pin 1**: Lock control signal (connect to relay COM)
- **Pin 2**: Ground/Common (connect to relay NC for "normally locked")
- **Pin 3**: +12V or +24V supply (may not be used for lock control)

#### Wiring Diagram - Contact Side
```
Wallbox Lock Connector       Relay Module Contacts
┌─────────────────────┐     ┌─────────────────────┐
│                     │     │                     │
│  Pin 1 (Lock Ctrl) ─┼─────┼── COM (Common)      │
│                     │     │                     │
│  Pin 2 (Ground)    ─┼─────┼── NC (Normally Closed) │
│                     │     │                     │
│  Pin 3 (Power)     ─┼─ ─ ─┼── NO (Normally Open) │
│    (if needed)      │     │     (not used)      │
└─────────────────────┘     └─────────────────────┘
```

## Complete Wiring Setup

### Full Connection Diagram
```
ESP32/LILYGO                Relay Module              Wallbox
┌─────────────────┐         ┌──────────────┐         ┌──────────────┐
│                 │         │   Control    │         │              │
│     GPIO25/18 ──┼─────────┼── IN         │         │              │
│     VCC (5V) ────┼─────────┼── VCC        │         │              │
│     GND ─────────┼─────────┼── GND        │         │              │
│                 │         │              │         │              │
│                 │         │   Contacts   │         │  Lock Conn   │
│                 │         │              │         │              │
│                 │         │   COM ───────┼─────────┼── Pin 1      │
│                 │         │   NC ────────┼─────────┼── Pin 2      │
│                 │         │   NO         │         │              │
│                 │         │   (unused)   │         │              │
└─────────────────┘         └──────────────┘         └──────────────┘
```

## Logic Operation

### Relay States
- **ESP32 GPIO LOW (0V)**: Relay OFF → Contacts connect COM to NC → **Wallbox LOCKED**
- **ESP32 GPIO HIGH (3.3V)**: Relay ON → Contacts connect COM to NO → **Wallbox UNLOCKED**

### Fail-Safe Behavior
When ESP32 loses power or crashes:
- GPIO pin goes to LOW (0V)
- Relay turns OFF
- Contacts default to NC position
- **Wallbox automatically locks** ✅

## Step-by-Step Installation

### Step 1: Power Down
1. Turn off power to the wallbox
2. Disconnect the ESP32/LILYGO from power
3. Ensure all components are de-energized

### Step 2: Connect Control Side
1. Connect GPIO25 (ESP32) or GPIO18 (LILYGO) to relay module IN pin
2. Connect VCC pin to relay module VCC (5V preferred, 3.3V usually works)
3. Connect GND pin to relay module GND

### Step 3: Connect Wallbox Side
1. Locate the wallbox remote lock connector
2. Identify lock control and ground pins (consult wallbox manual)
3. Connect relay COM to lock control pin
4. Connect relay NC to ground pin

### Step 4: Testing
1. Power up ESP32/LILYGO (wallbox still off)
2. Use multimeter to test relay operation:
   - Send unlock command → Should hear relay click, COM-NO connected
   - Send lock command → Should hear relay click, COM-NC connected
3. Power up wallbox and test lock operation

## Troubleshooting

### Relay Doesn't Click
- **Check power supply**: Ensure VCC is connected and at correct voltage
- **Check control signal**: Verify GPIO pin voltage changes (0V ↔ 3.3V)
- **Check relay module**: Some modules need 5V, won't work with 3.3V

### Lock Doesn't Respond
- **Check wallbox manual**: Verify lock connector pinout
- **Check connections**: Ensure solid connections to wallbox connector
- **Check polarity**: Some wallboxes are polarity sensitive

### Inverted Operation
If locked/unlocked states are reversed:
- Swap NC and NO connections on relay
- Or modify software logic (not recommended)

## Safety Notes

⚠️ **Important Safety Warnings**:
- Always consult your wallbox manual for specific lock connector details
- Use appropriately rated relay for your wallbox lock circuit
- Follow local electrical codes and regulations
- Test the fail-safe behavior before relying on it
- Consider using industrial-grade relays for critical applications

## Wallbox-Specific Notes

### Heidelberg Energy Control
- Typically uses low-voltage lock control signals
- Connector may be labeled "Remote" or "Lock"
- Check manual for exact pinout

### Other Wallbox Brands
- This wiring guide applies to most wallboxes with remote lock inputs
- Always verify connector pinout with manufacturer documentation
- Some wallboxes may use different voltage levels or logic

## Conclusion

This relay wiring setup provides a simple and reliable way to add physical lock control to your wallbox. The key benefit is the fail-safe operation: if the ESP32 loses power, the relay automatically defaults to the locked position, ensuring safe operation even during system failures.
