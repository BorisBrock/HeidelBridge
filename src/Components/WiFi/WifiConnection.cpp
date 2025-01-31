#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "WifiConnection.h"
#include "../Logger/Logger.h"
#include "../Statistics/Statistics.h"

namespace WifiConnection
{
    bool gIsCaptivePortalActive = false;
    DNSServer gDnsServer;

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

        WiFi.begin(Settings::Instance()->WifiSsid, Settings::Instance()->WifiPassword);
    }

    // Connects to the given SSID with the given password
    void ConnectToSsid(const String &ssid, const String &password)
    {
        // Delete old config
        Logger::Trace("Preparing Wifi");
        WiFi.disconnect(true);
        delay(100);

        // Set host name
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.mode(WIFI_MODE_NULL);
        WiFi.setHostname("HeidelBridge");

        // Register WiFi events
        WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        // Start Wifi connection
        Logger::Info("Connecting to WiFi SSID '%s'", ssid);
        WiFi.begin(ssid, password);
        gStatistics.NumWifiDisconnects = 0;
    }

    // Starts the captive portal
    void StartCaptivePortal()
    {
        // Local IP used in captive portal mode
        const IPAddress localIp(4, 3, 2, 1);
        const IPAddress gatewayIp(4, 3, 2, 1);
        const IPAddress subnetMask(255, 255, 255, 0);

        // Configure the soft access point with a specific IP and subnet mask
        WiFi.softAPConfig(localIp, gatewayIp, subnetMask);

        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(Constants::WebServer::CaptivePortalName);

        gDnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        gDnsServer.setTTL(300);
        gDnsServer.start(53, "*", localIp);

        gIsCaptivePortalActive = true;
    }

    // Cyclic processing
    void Update()
    {
        if (gIsCaptivePortalActive)
        {
            gDnsServer.processNextRequest();
        }
    }

    // Checks if the device is connected to a WiFi network
    bool IsConnectedToWifi()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    // Scans for available WiFi networks
    void StartNetworkScan()
    {
        Logger::Trace("Network scan requested");
        if (WiFi.scanComplete() == WIFI_SCAN_FAILED)
        {
            Logger::Trace("Starting network scan");
            WiFi.scanNetworks(true); // Start async scan
        }
    }

    // Checks if the scan has finished
    bool IsNetworkScanRunning()
    {
        return WiFi.scanComplete() < 0;
    }

    // Gets the network scan results
    void GetNetworkScanResults(JsonDocument &jsonDoc)
    {
        if (WiFi.scanComplete() <= 0)
        {
            Logger::Trace("WiFi scan not yet finished");
            return;
        }

        JsonArray networks = jsonDoc["networks"].to<JsonArray>();

        for (int32_t i = 0; i < WiFi.scanComplete(); i++)
        {
            JsonObject network = networks.add<JsonObject>();
            network["ssid"] = WiFi.SSID(i);
            network["rssi"] = WiFi.RSSI(i);
        }

        WiFi.scanDelete(); // Clear scan results
    }
};
