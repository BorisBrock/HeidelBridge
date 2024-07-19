#include <Arduino.h>
#include <WiFiManager.h>
#include "../Configuration/Constants.h"
#include "../Configuration/PersistentSettings.h"
#include "WifiConnection.h"

namespace WifiConnection
{
    constexpr uint16_t MaxOcppServerlength = 32;

    WiFiManager gWifiManager;
    WiFiManagerParameter gOcppParameter(
        "ocpp",
        Constants::OCPP::OCPPGUIParameterTitle,
        Constants::OCPP::OCPPGUIParameterInitValue,
        MaxOcppServerlength);

    void saveParamsCallback()
    {
        Serial.print("Storing OCPP server: '");
        Serial.print(gOcppParameter.getValue());
        Serial.println("'");

        PersistentSettings::WriteOCPPServer(String(gOcppParameter.getValue()));
    }

    void Init()
    {
        gWifiManager.setDebugOutput(false);

        gWifiManager.setConfigPortalTimeout(180);
        gWifiManager.setTitle(Constants::WiFi::CaptivePortalTitle);

        // Allow the user to enter the address of the OCPP server web socket
        gWifiManager.addParameter(&gOcppParameter);
        gWifiManager.setSaveParamsCallback(saveParamsCallback);

        // Hide everything except the wifi button
        std::vector<const char *> menu = {"wifi"};
        gWifiManager.setMenu(menu);

        // Open up the access point
        gWifiManager.autoConnect(Constants::WiFi::HotspotSSID, Constants::WiFi::HotspotPassword);

        // If we got here, connection was established successfully
    }
};