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

    // Deinitializes the settings
    void DeInit();

    // Read all settings from storage
    void ReadFromPersistentMemory();

    // Write all settings to storage
    void WriteToPersistentMemory();

    // Prints all settings to the logger
    void Print();

    // Energy counter state, kept out of WriteToPersistentMemory() so a settings save cannot overwrite it with a stale snapshot
    bool ReadEnergyCounter(int64_t &offsetWh, uint32_t &publishedWh);
    bool WriteEnergyOffset(int64_t offsetWh);
    bool WriteEnergyPublished(uint32_t publishedWh);

public:
    String DeviceName{};
    String WifiSsid{};
    String WifiPassword{};
    bool IsMqttEnabled{false};
    String MqttServer{};
    uint16_t MqttPort{1883};
    String MqttUser{};
    String MqttPassword{};
    String BoardType{"generic"};
};