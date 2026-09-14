#include <unordered_map>

#include "MockModbusRTU.h"
#include "Components/Modbus/ModbusRTU.h"

namespace
{
    std::unordered_map<uint16_t, uint16_t> gRegisters;
    std::unordered_map<uint16_t, uint32_t> gWriteCounts;
    bool gWriteSuccess = true;
    bool gReadSuccess = true;
}

ModbusRTU *ModbusRTU::Instance()
{
    static ModbusRTU instance;
    return &instance;
}

bool ModbusRTU::ReadRegisters(uint16_t startAddress, uint8_t numValues, uint8_t fc, uint16_t *values)
{
    (void)fc;
    if (!gReadSuccess)
    {
        return false;
    }

    for (uint8_t i = 0; i < numValues; ++i)
    {
        auto it = gRegisters.find(startAddress + i);
        values[i] = (it != gRegisters.end()) ? it->second : 0;
    }
    return true;
}

bool ModbusRTU::WriteHoldRegister16(uint16_t address, uint16_t value)
{
    if (!gWriteSuccess)
    {
        return false;
    }

    gRegisters[address] = value;
    gWriteCounts[address]++;
    return true;
}

namespace MockModbus
{
    void Reset()
    {
        gRegisters.clear();
        gWriteCounts.clear();
        gWriteSuccess = true;
        gReadSuccess = true;
    }

    void SetRegister(uint16_t address, uint16_t value)
    {
        gRegisters[address] = value;
    }

    uint16_t GetRegister(uint16_t address)
    {
        auto it = gRegisters.find(address);
        return (it != gRegisters.end()) ? it->second : 0;
    }

    uint32_t GetWriteCount(uint16_t address)
    {
        auto it = gWriteCounts.find(address);
        return (it != gWriteCounts.end()) ? it->second : 0;
    }

    void SetWriteSuccess(bool success)
    {
        gWriteSuccess = success;
    }

    void SetReadSuccess(bool success)
    {
        gReadSuccess = success;
    }
}
