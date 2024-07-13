#include <Arduino.h>
#include "Wallbox.h"

bool gIsCharging = false;

bool Wallbox::GetIsVehiclePluggedIn()
{
    // Todo
    return true;
}

void Wallbox::SetChargingLimits(float limitWatts, float limitAmps, int numPhases)
{
    // Todo
    Serial.println("Setting charging limits:");
    Serial.printf(" > Power (Watt): %f\n", limitWatts);
    Serial.printf(" > Current (Ampere): %f\n", limitAmps);
    Serial.printf(" > Phases: %d\n", numPhases);
}

void Wallbox::StartCharging()
{
    // Todo
    Serial.println("Received request to START charging");
    gIsCharging = true;
}

void Wallbox::StopCharging()
{
    // Todo
    Serial.println("Received request to STOP charging");
    gIsCharging = false;
}

float Wallbox::GetEnergyMeterWh()
{
    // Todo
    return gIsCharging ? 123.f : 0.f;
}

float Wallbox::GetPowerMeterW()
{
    // Todo
    return 5000.f;
}