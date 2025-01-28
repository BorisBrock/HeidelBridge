#pragma once

class WifiManager
{
public:
    // Gets the singleton instance
    static WifiManager *Instance();

    // Connects to a WiFi network or starts the captive portal
    void Start();

private:
    // Constructor
    WifiManager() {};

    // Connects to a known WiFi network
    void ConnectToWifiNetwork();

    // Starts the captive portal
    void StartCaptivePortal();
};