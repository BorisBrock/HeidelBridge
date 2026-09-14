#pragma once

// Stub of the eModbus ModbusClientRTU interface so that the real ModbusRTU.h
// can be compiled in host-based unit tests. The ModbusRTU implementation is
// replaced by a mock (see MockModbusRTU.h), so no real modbus functionality is
// needed here.
class ModbusClientRTU
{
public:
    virtual ~ModbusClientRTU() = default;
};
