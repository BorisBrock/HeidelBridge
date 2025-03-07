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

    // Read all settings from SPIFFS
    void ReadFromPersistentMemory();

public:
    String DeviceName{};
    String WifiSsid{};
    String WifiPassword{};
    bool IsMqttEnabled{false};
    String MqttServer{};
    uint16_t MqttPort{1833};
    String MqttUser{};
    String MqttPassword{};
};