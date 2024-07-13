#pragma once

namespace Wallbox
{
    bool GetIsVehiclePluggedIn();
    void SetChargingLimits(float limitWatts, float limitAmps, int numPhases);
    void StartCharging();
    void StopCharging();
    float GetEnergyMeterWh();
    float GetPowerMeterW();
};