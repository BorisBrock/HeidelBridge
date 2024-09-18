#include <Arduino.h>
#include <WiFi.h>
#include "HardwareSerial.h"
#include "ModbusClientRTU.h"
#include "../../Configuration/Pins.h"
#include "../../Configuration/Constants.h"
#include "ModbusRTU.h"

ModbusClientRTU gModbusRTU(Pins::PinRTS); // Create a ModbusRTU client instance
HardwareSerial gRs485Serial(1);           // Define a Serial for UART1
SemaphoreHandle_t gMutex = nullptr;       // A mutex object for buss access

ModbusRTU *ModbusRTU::Instance()
{
    static ModbusRTU instance;
    return &instance;
}

void ModbusRTU::Init()
{
    // Create the mutex
    gMutex = xSemaphoreCreateMutex();

    // Init serial conneted to the RTU Modbus
    Serial.println("Starting RS485 hardware serial");
    RTUutils::prepareHardwareSerial(gRs485Serial);
    gRs485Serial.begin(
        Constants::HeidelbergWallbox::ModbusBaudrate,
        SERIAL_8E1,
        Pins::PinRX,
        Pins::PinTX);

    // Start Modbus RTU
    Serial.println("Creating Modbus RTU instance");
    gModbusRTU.setTimeout(Constants::HeidelbergWallbox::ModbusTimeoutMs);
    gModbusRTU.begin(gRs485Serial); // Start ModbusRTU background task
}

bool ModbusRTU::ReadRegisters(uint16_t startAddress, uint8_t numValues, uint8_t fc, uint16_t *values)
{
    // Try to get the mutex
    if (xSemaphoreTake(gMutex, portMAX_DELAY))
    {
        ModbusMessage response = gModbusRTU.syncRequest(
            0,
            Constants::HeidelbergWallbox::ModbusServerId,
            (FunctionCode)fc,
            startAddress,
            numValues);

        // Free mutex
        xSemaphoreGive(gMutex);

        if (response.getError() == SUCCESS)
        {
            constexpr uint16_t startIndex = 3;
            for (uint8_t wordIndex = 0; wordIndex < numValues; ++wordIndex)
            {
                response.get(startIndex + wordIndex * Constants::ModbusRTU::RegisterSize, values[wordIndex]);
            }
            return true;
        }
        else
        {
            Serial.print("ModbusRTU read error: ");
            Serial.println(response.getError());
            for (uint8_t wordIndex = 0; wordIndex < numValues; ++wordIndex)
            {
                values[wordIndex] = 0;
            }
            return false;
        }
    }

    return false;
}

bool ModbusRTU::WriteHoldRegister16(uint16_t address, uint16_t value)
{
    // Try to get the mutex
    if (xSemaphoreTake(gMutex, portMAX_DELAY))
    {
        ModbusMessage response = gModbusRTU.syncRequest(
            0,
            Constants::HeidelbergWallbox::ModbusServerId,
            WRITE_HOLD_REGISTER,
            address,
            value);

        // Free mutex
        xSemaphoreGive(gMutex);

        if (response.getError() == SUCCESS)
        {
            return true;
        }
        else
        {
            Serial.print("ModbusRTU write error: ");
            Serial.println(response.getError());
            return false;
        }
    }

    return false;
}