#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MicroOcpp.h>
#include "../Configuration/Hems.h"
#include "OCPPManager.h"
#include "Wallbox.h"

bool gWasChargingAllowed = false;
bool gIsOcppRunning = false;

void OCPPManager::Init()
{
    mocpp_initialize(OcppServerUrl, "heidelberg-ec", "Heidelberg EC", "Heidelberg");

    // Input of the electricity meter register in Wh
    setEnergyMeterInput([]()
                        { return static_cast<int>(Wallbox::GetEnergyMeterWh()); });

    // Input of the power meter reading in W
    setPowerMeterInput([]()
                       { return Wallbox::GetPowerMeterW(); });

    // Set charging limits
    setSmartChargingOutput([](float limitWatts, float limitAmps, int numPhases)
                           { Wallbox::SetChargingLimits(limitWatts, limitAmps, numPhases); });

    // Return true if an EV is plugged to this EVSE
    setConnectorPluggedInput([]()
                             { return Wallbox::GetIsVehiclePluggedIn(); });

    gIsOcppRunning = true;
}

void OCPPManager::Loop()
{
    if (!gIsOcppRunning)
        return;

    // Do all OCPP stuff (process WebSocket input, send recorded meter values to Central System, etc.)
    mocpp_loop();

    // Energize EV plug if OCPP transaction is up and running
    if (ocppPermitsCharge())
    {
        if (!gWasChargingAllowed)
        {
            Wallbox::StartCharging();
            gWasChargingAllowed = true;
        }
    }
    else
    {
        if (gWasChargingAllowed)
        {
            Wallbox::StopCharging();
            gWasChargingAllowed = false;
        }
    }
}

void OCPPManager::Shutdown()
{
    gIsOcppRunning = false;
    mocpp_deinitialize();
}