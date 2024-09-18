#include <Arduino.h>
#include "../Modbus/ModbusRTU.h"
#include "../../Configuration/Constants.h"
#include "HeidelbergWallbox.h"

HeidelbergWallbox *HeidelbergWallbox::Instance()
{
    static HeidelbergWallbox instance;
    return &instance;
}

void HeidelbergWallbox::Init()
{
    uint16_t rawCurrent = static_cast<uint16_t>(Constants::HeidelbergWallbox::FailSafeCurrentA / Constants::HeidelbergWallbox::CurrentFactor);
    Serial.printf("Heidelberg wallbox: Initializing fail safe current with %d (raw)\n", rawCurrent);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::FailsafeCurrent, rawCurrent))
    {
        // Error writing modbus register
        Serial.println("ERROR: Could not set fail safe current");
    }

    uint16_t standbyDisabled = 4;
    Serial.printf("Heidelberg wallbox: Initializing standby mode with %d (raw)\n", standbyDisabled);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::DisableStandby, standbyDisabled))
    {
        // Error writing modbus register
        Serial.println("ERROR: Could not disable standby");
    }
}

VehicleState HeidelbergWallbox::GetState()
{
    uint16_t registerValue[0];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::ChargingState, 1, 0x4, registerValue))
    {
        Serial.printf("Heidelberg wallbox: Read state: %d\n", registerValue[0]);
        if (registerValue[0] <= 3)
        {
            mState = VehicleState::Disconnected;
        }
        else if (registerValue[0] <= 5)
        {
            mState = VehicleState::Connected;
        }
        else if (registerValue[0] <= 7)
        {
            mState = VehicleState::Charging;
        }
    }
    else
    {
        // Error reading modbus register
        Serial.println("Heidelberg wallbox: ERROR: Could not read plugged state");
    }

    return mState;
}

bool HeidelbergWallbox::SetChargingCurrentLimit(float currentLimitA)
{
    mChargingCurrentLimitA = currentLimitA;
    Serial.printf("Heidelberg wallbox: setting charging current limit to %f A\n", mChargingCurrentLimitA);

    uint16_t rawCurrent = static_cast<uint16_t>(mChargingCurrentLimitA / Constants::HeidelbergWallbox::CurrentFactor);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::MaximalCurrent, rawCurrent))
    {
        // Error writing modbus register
        Serial.println("Heidelberg wallbox: ERROR: Could not set maximum charging current");
    }

    return true;
}

float HeidelbergWallbox::GetChargingCurrentLimit()
{
    uint16_t registerValue[0];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::MaximalCurrent, 1, 0x3, registerValue))
    {
        mChargingCurrentLimitA = static_cast<float>(registerValue[0] * Constants::HeidelbergWallbox::CurrentFactor);
        Serial.printf("Heidelberg wallbox: Read max. charging current: %d\n", mChargingCurrentLimitA);
        return mChargingCurrentLimitA;
    }
    else
    {
        // Error reading modbus register
        Serial.println("Heidelberg wallbox: ERROR: Could not read max. charging current");
        return mChargingCurrentLimitA; // Return last valid value
    }
}

float HeidelbergWallbox::GetEnergyMeterValue()
{
    uint16_t rawEnergy[2];

    if (ModbusRTU::Instance()->ReadRegisters(
            Constants::HeidelbergRegisters::Energy,
            2,
            0x4,
            rawEnergy))
    {
        uint32_t totalEnergyWh = static_cast<uint32_t>(rawEnergy[0]) << 16 | static_cast<uint32_t>(rawEnergy[1]);
        mLastEnergyMeterValueWh = static_cast<float>(totalEnergyWh);

        Serial.printf("Heidelberg wallbox: Read energy meter value: %f Wh\n", mLastEnergyMeterValueWh);
    }
    else
    {
        Serial.println("Heidelberg wallbox: ERROR: Could not read energy meter value");
    }

    return mLastEnergyMeterValueWh;
}

float HeidelbergWallbox::GetFailsafeCurrent()
{
    uint16_t registerValue[1];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::FailsafeCurrent, 1, 0x3, registerValue))
    {
        mFailsafeCurrentA = static_cast<float>(registerValue[0] * Constants::HeidelbergWallbox::CurrentFactor);
        Serial.printf("Read Heidelberg failsafe current: %f\n", mFailsafeCurrentA);
    }
    else
    {
        // Error reading modbus register
        Serial.println("Heidelberg wallbox: ERROR: Could not read failsafe current");
    }

    return mFailsafeCurrentA;
}

float HeidelbergWallbox::GetChargingPower()
{
    uint16_t registerValue[1];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::Power, 1, 0x4, registerValue))
    {
        mLastPowerMeterValueW = static_cast<float>(registerValue[0]);
        Serial.printf("Reading power meter value: %f W\n", mLastPowerMeterValueW);
    }
    else
    {
        // Error reading modbus register
        Serial.println("Heidelberg wallbox: ERROR: Could not read last power meter value");
    }

    return mLastPowerMeterValueW;
}

bool HeidelbergWallbox::GetChargingCurrents(float &c1A, float &c2A, float &c3A)
{
    uint16_t rawCurrents[3];

    if (ModbusRTU::Instance()->ReadRegisters(
            Constants::HeidelbergRegisters::Currents,
            3,
            0x4,
            rawCurrents))
    {
        c1A = static_cast<float>(rawCurrents[0] * Constants::HeidelbergWallbox::CurrentFactor);
        c2A = static_cast<float>(rawCurrents[1] * Constants::HeidelbergWallbox::CurrentFactor);
        c3A = static_cast<float>(rawCurrents[2] * Constants::HeidelbergWallbox::CurrentFactor);
        Serial.printf("Reading currents: %f %f %f A\n", c1A, c2A, c3A);
        return true;
    }
    else
    {
        Serial.println("Heidelberg wallbox: ERROR: Could not read currents");
        return false;
    }
}

bool HeidelbergWallbox::GetChargingVoltages(float &v1V, float &v2V, float &v3V)
{
    uint16_t rawVoltages[3];

    if (ModbusRTU::Instance()->ReadRegisters(
            Constants::HeidelbergRegisters::Voltages,
            3,
            0x4,
            rawVoltages))
    {
        v1V = static_cast<float>(rawVoltages[0] * Constants::HeidelbergWallbox::VoltageFactor);
        v2V = static_cast<float>(rawVoltages[1] * Constants::HeidelbergWallbox::VoltageFactor);
        v3V = static_cast<float>(rawVoltages[2] * Constants::HeidelbergWallbox::VoltageFactor);
        Serial.printf("Reading voltages: %f %f %f V\n", v1V, v2V, v3V);
        return true;
    }
    else
    {
        Serial.println("Heidelberg wallbox: ERROR: Could not read voltages");
        return false;
    }
}