#include <Arduino.h>
#include <Preferences.h>
#include "../Components/Logger/Logger.h"
#include "Constants.h"
#include "Settings.h"

Preferences gPreferences;

Settings *Settings::Instance()
{
    static Settings instance;
    return &instance;
}

void Settings::Init()
{
    Logger::Debug("Initializing persistent settings");
    gPreferences.begin("heidelbridge", false);
}

// Read all settings from SPIFFS
void Settings::ReadFromPersistentMemory()
{
    Logger::Trace("Reading settings from persistent memory");

    DeviceName = gPreferences.getString("device_name", "HeidelBridge");
    WifiSsid = gPreferences.getString("wifi_ssid");
    WifiPassword = gPreferences.getString("wifi_password");
    IsMqttEnabled = gPreferences.getBool("mqtt_enabled");
    MqttPort = gPreferences.getUShort("mqtt_port", 1833);
    MqttServer = gPreferences.getString("mqtt_server");
    MqttUser = gPreferences.getString("mqtt_user");
    MqttPassword = gPreferences.getString("mqtt_password");
}

// Prints all settings to the logger
void Settings::Print()
{
    Logger::Debug("Using the following device settings:");

    Logger::Debug(" > Device name: %s", DeviceName.c_str());
    Logger::Debug(" > WiFi SSID: %s", WifiSsid.c_str());
    Logger::Debug(" > MQTT enabled: %s", IsMqttEnabled ? "yes" : "no");
    Logger::Debug(" > MQTT server: %s:%d", MqttServer.c_str(), MqttPort);
    Logger::Debug(" > MQTT user: %s", MqttUser.c_str());
}