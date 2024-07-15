#pragma once

namespace Modbus
{
    void Init();

    template <typename T>
    bool ReadInputRegister(T &value);

    template <typename T>
    bool ReadHoldRegister(T &value);

    template <typename T>
    bool WriteHoldRegister(T value);
};