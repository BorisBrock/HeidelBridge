#include <Arduino.h>
#include "../../Configuration/Constants.h"
#include "DummyWallbox.h"

DummyWallbox *DummyWallbox::Instance()
{
    static DummyWallbox instance;
    return &instance;
}

void DummyWallbox::Init()
{
    Serial.println("Dummy wallbox: initializing");
}

VehicleState DummyWallbox::GetState()
{
    if (mChargingCurrentLimitA > 0.0f)
    {
        Serial.println("Dummy wallbox: returning state CHARGING");
        return VehicleState::Charging;
    }
    else
    {
        Serial.println("Dummy wallbox: returning state CONNECTED");
        return VehicleState::Connected;
    }
}

bool DummyWallbox::SetChargingCurrentLimit(float currentLimitA)
{
    mChargingCurrentLimitA = currentLimitA;
    Serial.printf("Dummy wallbox: setting charging current limit to %f A\n", mChargingCurrentLimitA);
    return true;
}

float DummyWallbox::GetChargingCurrentLimit()
{
    Serial.printf("Dummy wallbox: returning charging current limit %f A\n", mChargingCurrentLimitA);
    return mChargingCurrentLimitA;
}

float DummyWallbox::GetEnergyMeterValue()
{
    if(mChargingCurrentLimitA > 0.0f)
    {
        mEnergyMeterKWh += 0.1f;
    }

    Serial.printf("Dummy wallbox: returning energy meter value %f kWh\n", mEnergyMeterKWh);
    return mEnergyMeterKWh;
}

float DummyWallbox::GetFailsafeCurrent()
{
    Serial.printf("Dummy wallbox: returning failsafe current %f A\n", mFailsafeCurrentA);
    return mFailsafeCurrentA;
}

float DummyWallbox::GetChargingPower()
{
    float chargingPowerW = mChargingCurrentLimitA * Constants::DummyWallbox::ChargingVoltageV * Constants::DummyWallbox::NumPhases;
    Serial.printf("Dummy wallbox: returning charging power %f W\n", chargingPowerW);
    return chargingPowerW;
}

bool DummyWallbox::GetChargingCurrents(float &c1A, float &c2A, float &c3A)
{
    c1A = mChargingCurrentLimitA;
    c2A = mChargingCurrentLimitA;
    c3A = mChargingCurrentLimitA;
    Serial.printf("Dummy wallbox: returning charging currents %f, %f and %f A\n", c1A, c2A, c3A);
    return true;
}

bool DummyWallbox::GetChargingVoltages(float &v1V, float &v2V, float &v3V)
{
    v1V = Constants::DummyWallbox::ChargingVoltageV;
    v2V = Constants::DummyWallbox::ChargingVoltageV;
    v3V = Constants::DummyWallbox::ChargingVoltageV;
    Serial.printf("Dummy wallbox: returning charging voltages %f, %f and %f A\n", v1V, v2V, v3V);
    return true;
}

float DummyWallbox::GetTemperature()
{
    return Constants::DummyWallbox::TemperatureDegCel;
}