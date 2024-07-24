#pragma once

class ModbusRTU
{
private:
    ModbusRTU(){};

public:
    static ModbusRTU *Instance();

    void Init();
    bool WriteHoldRegister16(uint16_t address, uint16_t value);
    bool ReadRegisters(uint16_t startAddress, uint8_t numValues, uint8_t fc, uint16_t *values);
};