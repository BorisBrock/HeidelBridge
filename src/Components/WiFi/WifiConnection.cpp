#include <Arduino.h>
#include <WiFi.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "WifiConnection.h"
#include "../Logger/Logger.h"
#include "../Statistics/Statistics.h"

namespace WifiConnection
{
    bool gReconnect = true;

    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Logger::Info("Connected to AP successfully!");
    }

    void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Logger::Info("WiFi connected");
        Logger::Info("IP address: %s", WiFi.localIP().toString().c_str());
    }

    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Logger::Warning("Disconnected from WiFi access point");
        Logger::Warning("WiFi lost connection. Reason: %d", info.wifi_sta_disconnected.reason);
        Logger::Warning("Trying to reconnect");

        gStatistics.NumWifiDisconnects++;

        if (gReconnect)
        {
            WiFi.begin(Settings::Instance()->WifiSsid, Settings::Instance()->WifiPassword);
        }
    }

    // Connects to the given SSID with the given password
    void Connect(const String &ssid, const String &password)
    {
        // Delete old config
        Logger::Trace("Preparing Wifi");
        WiFi.disconnect(true);
        delay(100);

        // Set host name
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.mode(WIFI_MODE_NULL);
        WiFi.setHostname(Settings::Instance()->DeviceName.c_str());

        // Register WiFi events
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        // Start Wifi connection
        Logger::Info("Connecting to WiFi SSID '%s'", ssid);
        gReconnect = true;
        WiFi.begin(ssid, password);
        gStatistics.NumWifiDisconnects = 0;
    }

    // Checks if the device is connected to a WiFi network
    bool IsConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    // Disconnect and stop all reconnect attempts
    void Disconnect()
    {
        gReconnect = false;
        WiFi.disconnect(true);
    }
};
