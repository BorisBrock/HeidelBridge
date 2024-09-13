#include <Arduino.h>
#include <WiFi.h>
#include "../../Configuration/Constants.h"
#include "WifiConnection.h"

namespace WifiConnection
{
    const char *ssid = "YourSSID";
    const char *password = "YourPassword";

    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.println("Connected to AP successfully!");
    }

    void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }

    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        Serial.println("Disconnected from WiFi access point");
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.wifi_sta_disconnected.reason);
        Serial.println("Trying to reconnect");
        WiFi.begin(ssid, password);
    }

    void Init()
    {
        // Delete old config
        Serial.println("Preparing Wifi");
        WiFi.disconnect(true);
        delay(100);

        // Register WiFi events
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        // Start Wifi connection
        Serial.print("Connecting WiFi, SSID: ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
    }
};