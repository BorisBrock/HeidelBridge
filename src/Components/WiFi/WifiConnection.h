#pragma once

namespace WifiConnection
{
    // Connects to the given SSID with the given password
    void ConnectToSsid(const String &ssid, const String &password);

    // Starts the captive portal
    void StartCaptivePortal();

    // Cyclic processing
    void Update();
};