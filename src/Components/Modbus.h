#pragma once

#include "ModbusClientRTU.h"

namespace Modbus
{
    void Init();

    bool ReadInputRegister16(uint16_t address, uint16_t &value);

    bool ReadHoldRegister16(uint16_t address, uint16_t &value);

    bool WriteHoldRegister16(uint16_t address, uint16_t value);
};