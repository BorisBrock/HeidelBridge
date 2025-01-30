#pragma once

namespace WifiConnection
{
    enum class NetworkScanStatus
    {
        None,
        Scanning,
        Completed
    };


    // Connects to the given SSID with the given password
    void ConnectToSsid(const String &ssid, const String &password);

    // Starts the captive portal
    void StartCaptivePortal();

    // Cyclic processing
    void Update();

    // Checks if the device is connected to a WiFi network
    bool IsConnectedToWifi();

    // Scans for available WiFi networks
    void StartNetworkScan();

    // Checks if the scan has finished
    bool IsNetworkScanFinished();

    // Gets the network scan results
    void GetNetworkScanResults(JsonDocument &jsonDoc);
};