#pragma once

class Settings
{
private:
    Settings() {};

public:
    // Gets the singleton instance of the settings
    static Settings *Instance();

    // Initializes the settings
    void Init();

    // Checks if the device has been configured with WiFi credentials
    bool HasWifiCredentials();
};