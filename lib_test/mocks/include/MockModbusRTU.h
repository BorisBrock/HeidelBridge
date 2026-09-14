#pragma once

#include <cstdint>

// Test hooks for the mock ModbusRTU implementation. The mock simulates a
// holding register map and can be put into a failure state to test error
// handling of the code under test.
namespace MockModbus
{
    // Clears the simulated register map and resets the failure flags
    void Reset();

    // Sets a holding register value
    void SetRegister(uint16_t address, uint16_t value);

    // Returns a holding register value, 0 if not set
    uint16_t GetRegister(uint16_t address);

    // Returns how often a holding register was written
    uint32_t GetWriteCount(uint16_t address);

    // Makes all subsequent writes succeed or fail
    void SetWriteSuccess(bool success);

    // Makes all subsequent reads succeed or fail
    void SetReadSuccess(bool success);
}
