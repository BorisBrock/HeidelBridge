#pragma once

class WifiManager
{
public:
    // Connects to a WiFi network or starts the captive portal
    void Start();

private:
    // Connects to a known WiFi network
    void ConnectToWifiNetwork();

    // Starts the captive portal
    void StartCaptivePortal();
};