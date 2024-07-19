#include <Arduino.h>
#include <WiFi.h>
#include "HardwareSerial.h"
#include "../Configuration/Constants.h"
#include "Modbus.h"

namespace Modbus
{
    ModbusClientRTU gModbusRTU(Constants::Pins::PinRTS); // Create a ModbusRTU client instance
    HardwareSerial gRs485Serial(1);                       // define a Serial for UART1
    constexpr uint8_t gServerId = 1;

    void Init()
    {
        // Init serial conneted to the RTU Modbus
        Serial.println("Starting RS485 hardware serial");
        RTUutils::prepareHardwareSerial(gRs485Serial);
        gRs485Serial.begin(19200, SERIAL_8E1, Constants::Pins::PinRX, Constants::Pins::PinTX);

        // Start Modbus RTU
        Serial.println("Creating Modbus RTU instance");
        gModbusRTU.setTimeout(2000);    // Set RTU Modbus message timeout to 2000ms
        gModbusRTU.begin(gRs485Serial); // Start ModbusRTU background task
    }

    bool ReadRegisterInternal16(uint16_t address, FunctionCode fc, uint16_t &value)
    {
        ModbusMessage response = gModbusRTU.syncRequest(0, gServerId, fc, address, sizeof(uint16_t));
        if (response.getError() == SUCCESS)
        {
            value = response[3] << 8 | response[4]; // Skip byte 0, 1 and 2
            return true;
        }
        else
        {
            Serial.print("ModbusRTU read error: ");
            Serial.println(response.getError());
            value = 0;
            return false;
        }
    }

    bool ReadInputRegister16(uint16_t address, uint16_t &value)
    {
        return ReadRegisterInternal16(address, READ_INPUT_REGISTER, value);
    }

    bool ReadHoldRegister16(uint16_t address, uint16_t &value)
    {
        return ReadRegisterInternal16(address, READ_HOLD_REGISTER, value);
    }

    bool WriteHoldRegister16(uint16_t address, uint16_t value)
    {
        ModbusMessage response = gModbusRTU.syncRequest(0, gServerId, WRITE_HOLD_REGISTER, address, value);
        if (response.getError() != SUCCESS)
        {
            Serial.print("ModbusRTU write error: ");
            Serial.println(response.getError());
        }
        return response.getError() == SUCCESS;
    }
};