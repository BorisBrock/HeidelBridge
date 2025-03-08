#pragma once

namespace WifiConnection
{
    // Connects to the given SSID with the given password
    void Connect(const String &ssid, const String &password);

    // Disconnect and stop all reconnect attempts
    void Disconnect();

    // Checks if the device is connected to a WiFi network
    bool IsConnected();
};