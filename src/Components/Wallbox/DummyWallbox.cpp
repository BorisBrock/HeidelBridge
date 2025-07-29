#include <Arduino.h>
#include "../Logger/Logger.h"
#include "../../Configuration/Constants.h"
#include "DummyWallbox.h"

DummyWallbox *DummyWallbox::Instance()
{
    static DummyWallbox instance;
    return &instance;
}

void DummyWallbox::Init()
{
    Logger::Debug("Dummy wallbox: initializing");
}

VehicleState DummyWallbox::GetState()
{
    if (mChargingCurrentLimitA > 0.0f)
    {
        Logger::Debug("Dummy wallbox: returning state CHARGING");
        return VehicleState::Charging;
    }
    else
    {
        Logger::Debug("Dummy wallbox: returning state CONNECTED");
        return VehicleState::Connected;
    }
}

bool DummyWallbox::SetChargingCurrentLimit(float currentLimitA)
{
    if (mChargingEnabled)
    {
        mChargingCurrentLimitA = currentLimitA;
        Logger::Debug("Dummy wallbox: setting charging current limit to %f A", mChargingCurrentLimitA);
    }
    else
    {
        mPreviousChargingCurrentLimitA = currentLimitA;
        Logger::Info("Dummy wallbox: charging is disabled. current limit %f A is not applied", mChargingCurrentLimitA);
    }

    return true;
}

bool DummyWallbox::SetChargingEnabled(bool chargingEnabled)
{
    bool ok = true;

    if (!mChargingEnabled && chargingEnabled)
    {
        Logger::Info("Dummy wallbox: enabling charging");

        // Enable charging
        mChargingEnabled = true;
        ok = SetChargingCurrentLimit(mPreviousChargingCurrentLimitA);
    }
    else if (mChargingEnabled && !chargingEnabled)
    {
        Logger::Info("Dummy wallbox: disabling charging");

        // Disable charging
        mPreviousChargingCurrentLimitA = mChargingCurrentLimitA;
        ok = SetChargingCurrentLimit(0.0f);
        mChargingEnabled = false;
    }

    return ok;
}

bool DummyWallbox::IsChargingEnabled()
{
    Logger::Debug("Dummy wallbox: returning charging enabled %i", mChargingEnabled);
    return mChargingEnabled;
}

float DummyWallbox::GetChargingCurrentLimit()
{
    Logger::Debug("Dummy wallbox: returning charging current limit %f A", mChargingCurrentLimitA);
    return mChargingCurrentLimitA;
}

float DummyWallbox::GetEnergyMeterValue()
{
    if (mChargingCurrentLimitA > 0.0f)
    {
        mEnergyMeterWh += 100.0f;
    }

    Logger::Debug("Dummy wallbox: returning energy meter value %f kWh", mEnergyMeterWh);
    return mEnergyMeterWh;
}

float DummyWallbox::GetFailsafeCurrent()
{
    Logger::Debug("Dummy wallbox: returning failsafe current %f A", mFailsafeCurrentA);
    return mFailsafeCurrentA;
}

float DummyWallbox::GetChargingPower()
{
    float chargingPowerW = mChargingCurrentLimitA * Constants::DummyWallbox::ChargingVoltageV * Constants::DummyWallbox::NumPhases;
    Logger::Debug("Dummy wallbox: returning charging power %f W", chargingPowerW);
    return chargingPowerW;
}

bool DummyWallbox::GetChargingCurrents(float &c1A, float &c2A, float &c3A)
{
    c1A = mChargingCurrentLimitA;
    c2A = mChargingCurrentLimitA;
    c3A = mChargingCurrentLimitA;
    Logger::Debug("Dummy wallbox: returning charging currents %f, %f and %f A", c1A, c2A, c3A);
    return true;
}

bool DummyWallbox::GetChargingVoltages(float &v1V, float &v2V, float &v3V)
{
    v1V = Constants::DummyWallbox::ChargingVoltageV;
    v2V = Constants::DummyWallbox::ChargingVoltageV;
    v3V = Constants::DummyWallbox::ChargingVoltageV;
    Logger::Debug("Dummy wallbox: returning charging voltages %f, %f and %f A", v1V, v2V, v3V);
    return true;
}

float DummyWallbox::GetTemperature()
{
    return Constants::DummyWallbox::TemperatureDegCel;
}