#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MicroOcpp.h>
#include "OCPPManager.h"

#define OCPP_BACKEND_URL "ws://192.168.178.90:8887"
#define OCPP_CHARGE_BOX_ID "heidelberg-ec"

bool gTempIsCharging = false;
bool gWasChargingAllowed = false;
bool gIsOcppRunning = true;

void OCPPManager::Init()
{
    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, "My Charging Station", "My company name");

    /*
     * Integrate OCPP functionality. You can leave out the following part if your EVSE doesn't need it.
     */
    setEnergyMeterInput([]()
                        {
                            Serial.println(gTempIsCharging);
        //take the energy register of the main electricity meter and return the value in watt-hours
        return gTempIsCharging ? millis() : 0; });

    setPowerMeterInput([]()
                       { return gTempIsCharging ? 123 : 0; });

    setSmartChargingCurrentOutput([](float limit)
                                  {
        //set the SAE J1772 Control Pilot value here
        Serial.printf("[main] Smart Charging allows maximum charge current: %.0f\n", limit); });

    setSmartChargingPowerOutput([](float limit)
                                {
        //set the SAE J1772 Control Pilot value here
        Serial.printf("[main] Smart Charging allows maximum charge power: %.0f\n", limit); });

    setConnectorPluggedInput([]()
                             {
        // return true if an EV is plugged to this EVSE
        return true; });
}

void OCPPManager::Loop()
{
    if (!gIsOcppRunning)
        return;

    /*
     * Do all OCPP stuff (process WebSocket input, send recorded meter values to Central System, etc.)
     */
    mocpp_loop();

    /*
     * Energize EV plug if OCPP transaction is up and running
     */
    if (ocppPermitsCharge())
    {
        if (!gWasChargingAllowed)
        {
            Serial.println("Received request to START CHARGING");
        }

        gTempIsCharging = true;
        gWasChargingAllowed = true;
    }
    else
    {
        if (gWasChargingAllowed)
        {
            Serial.println("Received request to STOP CHARGING");
        }

        gTempIsCharging = false;
        gWasChargingAllowed = false;
    }
}

void OCPPManager::Shutdown()
{
    gIsOcppRunning = false;
    mocpp_deinitialize();
}