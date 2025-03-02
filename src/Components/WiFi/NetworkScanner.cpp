#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "NetworkScanner.h"
#include "../Logger/Logger.h"

namespace NetworkScanner
{
    // Scans for available WiFi networks
    void StartNetworkScan()
    {
        Logger::Trace("Network scan requested");
        if (WiFi.scanComplete() == WIFI_SCAN_RUNNING)
        {
            Logger::Trace(" -> Scan already running");
            return;
        }
        else
        {
            Logger::Trace(" -> Starting network scan");
            WiFi.scanNetworks(true); // Start async scan
        }
    }

    // Checks if the scan has finished
    bool IsNetworkScanRunning()
    {
        Logger::Debug("Current WiFi scanning status: %i", WiFi.scanComplete());

        return WiFi.scanComplete() == WIFI_SCAN_RUNNING;
    }

    // Gets the network scan results
    void GetNetworkScanResults(JsonDocument &jsonDoc)
    {
        bool isScanning = NetworkScanner::IsNetworkScanRunning();

        jsonDoc["status"] = isScanning ? "scanning" : "idle";

        if (isScanning)
        {
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
