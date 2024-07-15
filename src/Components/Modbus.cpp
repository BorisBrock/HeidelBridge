#include <Arduino.h>
#include <WiFi.h>
#include "HardwareSerial.h"
#include "ModbusClientRTU.h"
#include "../Configuration/Pins.h"
#include "Modbus.h"

ModbusClientRTU gModbusRTU(PIN_RTS); // Create a ModbusRTU client instance
HardwareSerial gRs485Serial(1);      // define a Serial for UART1
constexpr uint8_t gServerId = 1;

void Modbus::Init()
{
    // Init serial conneted to the RTU Modbus
    Serial.println("Starting RS485 hardware serial");
    RTUutils::prepareHardwareSerial(gRs485Serial);
    gRs485Serial.begin(19200, SERIAL_8E1, PIN_RX, PIN_TX);

    // Start Modbus RTU
    Serial.println("Creating Modbus RTU instance");
    gModbusRTU.setTimeout(2000);    // Set RTU Modbus message timeout to 2000ms
    gModbusRTU.begin(gRs485Serial); // Start ModbusRTU background task
}

template <typename T>
bool Modbus::ReadInputRegister(T &value)
{
    ModbusMessage response = gModbusRTU.syncRequest(0, gServerId, READ_INPUT_REGISTER, sizeof(T));

    if (response.getError() != SUCCESS)
        return false;

    value = *reinterpret_cast<const T*>(response.data());
    return true;
}

template <typename T>
bool Modbus::ReadHoldRegister(T &value)
{
    ModbusMessage response = gModbusRTU.syncRequest(0, gServerId, READ_HOLD_REGISTER, sizeof(T));

    if (response.getError() != SUCCESS)
        return false;

    value = *reinterpret_cast<const T*>(response.data());
    return true;
}

template <typename T>
bool Modbus::WriteHoldRegister(T value)
{
    ModbusMessage response = gModbusRTU.syncRequest(0, gServerId, WRITE_HOLD_REGISTER, value);

    if (response.getError() != SUCCESS)
        return false;

    value = *reinterpret_cast<const T*>(response.data());
    return true;
}