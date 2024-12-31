#include <Arduino.h>
#include "../Logger/Logger.h"
#include "../Statistics/Statistics.h"
#include "HardwareSerial.h"
#include "ModbusClientRTU.h"
#include "../../Configuration/Pins.h"
#include "../../Configuration/Constants.h"
#include "ModbusRTU.h"

ModbusClientRTU gModbusRTU(Pins::PinRTS); // Create a ModbusRTU client instance
HardwareSerial gRs485Serial(1);           // Define a Serial for UART1
SemaphoreHandle_t gMutex = nullptr;       // A mutex object for buss access

// Returns the singleton instance of ModbusRTU
ModbusRTU *ModbusRTU::Instance()
{
    static ModbusRTU instance;
    return &instance;
}

// Initializes the ModbusRTU instance
void ModbusRTU::Init()
{
    // Create the mutex
    gMutex = xSemaphoreCreateMutex();

    // Init serial conneted to the RTU Modbus
    Logger::Info("Starting RS485 hardware serial");
    RTUutils::prepareHardwareSerial(gRs485Serial);
    gRs485Serial.begin(
        Constants::HeidelbergWallbox::ModbusBaudrate,
        SERIAL_8E1,
        Pins::PinRX,
        Pins::PinTX);

    // Start Modbus RTU
    Logger::Trace("Creating Modbus RTU instance");
    gModbusRTU.setTimeout(Constants::HeidelbergWallbox::ModbusTimeoutMs);
    gModbusRTU.begin(gRs485Serial); // Start ModbusRTU background task
}

// Reads multiple registers starting from the specified address
bool ModbusRTU::ReadRegisters(uint16_t startAddress, uint8_t numValues, uint8_t fc, uint16_t *values)
{
    uint16_t numTries = 1 + Constants::ModbusRTU::NumReadRetries;
    uint8_t lastError = 0;

    while (numTries > 0)
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
                // Read failed
                lastError = response.getError();
                Logger::Warning("ModbusRTU read attempt failed: %d", lastError);
                delay(Constants::ModbusRTU::RetryDelayMs);
            }
        }
    }

    // All read attempts failed
    Logger::Error("ModbusRTU read error: %d", lastError);
    gStatistics.NumModbusReadErrors++;
    for (uint8_t wordIndex = 0; wordIndex < numValues; ++wordIndex)
    {
        values[wordIndex] = 0;
    }
    return false;
}

// Writes a single 16 bit holding register at the specified address
bool ModbusRTU::WriteHoldRegister16(uint16_t address, uint16_t value)
{
    uint16_t numTries = 1 + Constants::ModbusRTU::NumWriteRetries;
    uint8_t lastError = 0;

    while (numTries > 0)
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
                // Write failed
                lastError = response.getError();
                Logger::Warning("ModbusRTU write attempt failed: %d", lastError);
                delay(Constants::ModbusRTU::RetryDelayMs);
            }
        }

        numTries--;
    }

    // All write attempts failed
    Logger::Error("ModbusRTU write error: %d", lastError);
    gStatistics.NumModbusWriteErrors++;
    return false;
}