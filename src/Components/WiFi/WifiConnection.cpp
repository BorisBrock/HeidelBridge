#include <Arduino.h>
#include <WiFiManager.h>
#include "../../Configuration/Constants.h"
#include "WifiConnection.h"

namespace WifiConnection
{
    WiFiManager gWifiManager;

    void Init()
    {
        Serial.print("Starting WiFi Manager");

        WiFi.hostname(Constants::WiFi::HostName);
        gWifiManager.setDebugOutput(false);
        gWifiManager.setConfigPortalTimeout(180);
        gWifiManager.setTitle(Constants::WiFi::CaptivePortalTitle);

        // Hide everything except the wifi button
        std::vector<const char *> menu = {"wifi"};
        gWifiManager.setMenu(menu);

        // Open up the access point
        gWifiManager.autoConnect(Constants::WiFi::HotspotSSID, Constants::WiFi::HotspotPassword);

        // If we got here, connection was established successfully
        Serial.println("WiFi setup completed successfully");
        Serial.print("Local IP address: ");
        Serial.println(WiFi.localIP());
    }
};