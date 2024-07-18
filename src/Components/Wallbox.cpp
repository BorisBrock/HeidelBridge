#include <Arduino.h>
#include "Components/Modbus.h"
#include "Wallbox.h"

namespace Wallbox
{
    // Heidelberg EC Modbus registers
    constexpr uint16_t RegisterChargingState = 5;
    constexpr uint16_t RegisterMaximalCurrent = 261;
    constexpr uint16_t RegisterPower = 14;
    constexpr uint16_t RegisterEnergyLow = 16;
    constexpr uint16_t RegisterEnergyHigh = 15;

    // Last wallbox states
    bool gIsVehiclePluggedIn = false;
    bool gIsChargingAllowed = false;
    uint16_t gLastCurrentLimit = 160; // 16 Ampere
    float gLastPowerMeterValueW = 0.0f;
    float gLastEnergyMeterValueWh = 0.0f;

    bool GetIsVehiclePluggedIn()
    {
        uint16_t registerValue{};
        if (Modbus::ReadInputRegister16(RegisterChargingState, registerValue))
        {
            gIsVehiclePluggedIn = registerValue >= 4 && registerValue <= 7;
        }
        else
        {
            // Error reading modbus register
            Serial.println("ERROR: Could not read charging state via Modbus");
        }

        return gIsVehiclePluggedIn;
    }

    void SetChargingLimits(float limitWatts, float limitAmps, int numPhases)
    {
        Serial.println("Setting charging limits:");
        Serial.printf(" > Power (Watt): %f\n", limitWatts);
        Serial.printf(" > Current (Ampere): %f\n", limitAmps);
        Serial.printf(" > Phases: %d\n", numPhases);

        gLastCurrentLimit = static_cast<uint16_t>(limitAmps * 10);

        if (!Modbus::WriteHoldRegister16(RegisterMaximalCurrent, gLastCurrentLimit))
        {
            // Error reading modbus register
            Serial.println("ERROR: Could not set maximum charging current via Modbus");
        }
    }

    void StartCharging()
    {
        Serial.println("Starting charging");
        gIsChargingAllowed = true;

        if (!Modbus::WriteHoldRegister16(RegisterMaximalCurrent, gLastCurrentLimit))
        {
            // Error reading modbus register
            Serial.println("ERROR: Could not set maximum charging current via Modbus");
        }
    }

    void StopCharging()
    {
        Serial.println("Stopping charging");
        gIsChargingAllowed = false;

        // Set max. charging current to 0 (= off)
        if (!Modbus::WriteHoldRegister16(RegisterMaximalCurrent, 0))
        {
            // Error reading modbus register
            Serial.println("ERROR: Could not set maximum charging current to 0 via Modbus");
        }
    }

    float GetEnergyMeterWh()
    {
        uint16_t registerValueLow = 0;
        if (Modbus::ReadInputRegister16(RegisterEnergyLow, registerValueLow))
        {
            uint16_t registerValueHigh = 0;
            if (Modbus::ReadInputRegister16(RegisterEnergyHigh, registerValueHigh))
            {
                uint32_t totalEnergyWh = static_cast<uint32_t>(registerValueHigh) << 16 | static_cast<uint32_t>(registerValueLow);
                gLastEnergyMeterValueWh = static_cast<float>(totalEnergyWh);
            }
            else
            {
                // Error reading modbus register
                Serial.println("ERROR: Could not read energy meter value (high word) via Modbus");
            }
        }
        else
        {
            // Error reading modbus register
            Serial.println("ERROR: Could not read energy meter value (low word) via Modbus");
        }

        return gLastEnergyMeterValueWh;
    }

    float GetPowerMeterW()
    {
        uint16_t registerValue = 0;
        if (Modbus::ReadInputRegister16(RegisterPower, registerValue))
        {
            gLastPowerMeterValueW = static_cast<float>(registerValue);
        }
        else
        {
            // Error reading modbus register
            Serial.println("ERROR: Could not read last power meter value via Modbus");
        }

        return gLastPowerMeterValueW;
    }
};