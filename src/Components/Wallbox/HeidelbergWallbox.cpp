#include <Arduino.h>
#include "../Logger/Logger.h"
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
    Logger::Debug("Heidelberg wallbox: Initializing fail safe current with %d (raw)", rawCurrent);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::FailsafeCurrent, rawCurrent))
    {
        // Error writing modbus register
        Logger::Error("ERROR: Could not set fail safe current");
    }

    // Disable standby
    uint16_t standbyDisabled = Constants::HeidelbergWallbox::AllowStandby ? 0 : 4;
    Logger::Debug("Heidelberg wallbox: Initializing standby mode with %d", standbyDisabled);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::DisableStandby, standbyDisabled))
    {
        // Error writing modbus register
        Logger::Error("ERROR: Could not configure standby");
    }

    // Disable watchdog
    Logger::Debug("Heidelberg wallbox: Setting watch dog timeout to %d s", Constants::HeidelbergWallbox::WatchdogTimeoutS);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::WatchdogTimeout, Constants::HeidelbergWallbox::WatchdogTimeoutS))
    {
        // Error writing modbus register
        Logger::Error("ERROR: Could not set watchdog timeout");
    }
}

VehicleState HeidelbergWallbox::GetState()
{
    uint16_t registerValue[0];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::ChargingState, 1, 0x4, registerValue))
    {
        Logger::Debug("Heidelberg wallbox: Read state: %d", registerValue[0]);
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
        Logger::Error("Heidelberg wallbox: ERROR: Could not read plugged state");
    }

    return mState;
}

bool HeidelbergWallbox::SetChargingCurrentLimit(float currentLimitA)
{
    mChargingCurrentLimitA = currentLimitA;
    Logger::Info("Heidelberg wallbox: setting charging current limit to %f A", mChargingCurrentLimitA);

    uint16_t rawCurrent = static_cast<uint16_t>(mChargingCurrentLimitA / Constants::HeidelbergWallbox::CurrentFactor);
    if (!ModbusRTU::Instance()->WriteHoldRegister16(Constants::HeidelbergRegisters::MaximalCurrent, rawCurrent))
    {
        // Error writing modbus register
        Logger::Error("Heidelberg wallbox: ERROR: Could not set maximum charging current");
    }

    return true;
}

float HeidelbergWallbox::GetChargingCurrentLimit()
{
    uint16_t registerValue[0];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::MaximalCurrent, 1, 0x3, registerValue))
    {
        mChargingCurrentLimitA = static_cast<float>(registerValue[0] * Constants::HeidelbergWallbox::CurrentFactor);
        Logger::Debug("Heidelberg wallbox: Read max. charging current: %d", mChargingCurrentLimitA);
        return mChargingCurrentLimitA;
    }
    else
    {
        // Error reading modbus register
        Logger::Error("Heidelberg wallbox: ERROR: Could not read max. charging current");
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

        Logger::Debug("Heidelberg wallbox: Read energy meter value: %f Wh", mLastEnergyMeterValueWh);
    }
    else
    {
        Logger::Error("Heidelberg wallbox: ERROR: Could not read energy meter value");
    }

    return mLastEnergyMeterValueWh;
}

float HeidelbergWallbox::GetFailsafeCurrent()
{
    uint16_t registerValue[1];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::FailsafeCurrent, 1, 0x3, registerValue))
    {
        mFailsafeCurrentA = static_cast<float>(registerValue[0] * Constants::HeidelbergWallbox::CurrentFactor);
        Logger::Debug("Read Heidelberg failsafe current: %f", mFailsafeCurrentA);
    }
    else
    {
        // Error reading modbus register
        Logger::Error("Heidelberg wallbox: ERROR: Could not read failsafe current");
    }

    return mFailsafeCurrentA;
}

float HeidelbergWallbox::GetChargingPower()
{
    uint16_t registerValue[1];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::Power, 1, 0x4, registerValue))
    {
        mLastPowerMeterValueW = static_cast<float>(registerValue[0]);
        Logger::Debug("Reading power meter value: %f W", mLastPowerMeterValueW);
    }
    else
    {
        // Error reading modbus register
        Logger::Error("Heidelberg wallbox: ERROR: Could not read last power meter value");
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
        Logger::Debug("Reading currents: %f %f %f A", c1A, c2A, c3A);
        return true;
    }
    else
    {
        Logger::Error("Heidelberg wallbox: ERROR: Could not read currents");
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
        Logger::Debug("Reading voltages: %f %f %f V", v1V, v2V, v3V);
        return true;
    }
    else
    {
        Logger::Error("Heidelberg wallbox: ERROR: Could not read voltages");
        return false;
    }
}

float HeidelbergWallbox::GetTemperature()
{
    uint16_t registerValue[0];
    if (ModbusRTU::Instance()->ReadRegisters(Constants::HeidelbergRegisters::PcbTemperature, 1, 0x4, registerValue))
    {
        return static_cast<float>(registerValue[0]) * Constants::HeidelbergWallbox::TemperatureFactor;
    }
    else
    {
        // Error reading modbus register
        Logger::Error("Heidelberg wallbox: ERROR: Could not read PCB temperature");
        return 0.0f;
    }
}