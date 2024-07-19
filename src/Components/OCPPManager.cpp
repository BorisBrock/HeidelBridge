#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MicroOcpp.h>
#include "../Configuration/Constants.h"
#include "../Configuration/PersistentSettings.h"
#include "OCPPManager.h"
#include "Wallbox.h"

namespace OCPPManager
{
    bool gWasChargingAllowed = false;
    bool gIsOcppRunning = false;

    void Init()
    {
        // Get server address from persistent memory
        String ocppServer = PersistentSettings::ReadOCPPServer();

        Serial.print("Connecting to OCPP server '");
        Serial.print(ocppServer);
        Serial.println("'");

        mocpp_initialize(
            ocppServer.c_str(),
            Constants::OCPP::ChargeBoxId,
            Constants::OCPP::ChargePointModel,
            Constants::OCPP::ChargePointVendor);

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

    void Loop()
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

    void Shutdown()
    {
        gIsOcppRunning = false;
        mocpp_deinitialize();
    }
};