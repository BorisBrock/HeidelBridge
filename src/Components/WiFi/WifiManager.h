#pragma once

class WifiManager
{
public:
    // Gets the singleton instance
    static WifiManager *Instance();

    // Connects to a WiFi network or starts the captive portal
    void Start();

    // Cyclic processing
    void Update();

private:
    // Constructor
    WifiManager() {};

    // Connects to a known WiFi network
    bool ConnectToWifiNetwork();

    // Starts the captive portal
    void StartCaptivePortal();
};