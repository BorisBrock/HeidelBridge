#include <Arduino.h>
#include <WiFi.h>
#include "../../Configuration/Constants.h"
#include "WifiConnection.h"
#include "../../Configuration/Configuration.h"
#include "../Logger/Logger.h"
#include "../Statistics/Statistics.h"

namespace WifiConnection
{
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

        WiFi.begin(Configuration::WiFi::SSID, Configuration::WiFi::Password);
    }

    void Init()
    {
        // Delete old config
        Logger::Trace("Preparing Wifi");
        WiFi.disconnect(true);
        delay(100);

        // Set host name
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.mode(WIFI_MODE_NULL);
        WiFi.setHostname(Configuration::General::DeviceName);

        // Register WiFi events
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        // Start Wifi connection
        Logger::Info("Connecting to WiFi SSID '%s'", Configuration::WiFi::SSID);
        WiFi.begin(Configuration::WiFi::SSID, Configuration::WiFi::Password);
        gStatistics.NumWifiDisconnects = 0;
    }
};
