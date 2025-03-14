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

// Deinitializes the settings
void Settings::DeInit()
{
    Logger::Debug("Deinitializing persistent settings");
    gPreferences.end();
}

// Read all settings from SPIFstorageFS
void Settings::ReadFromPersistentMemory()
{
    Logger::Trace("Reading settings from persistent memory");

    DeviceName = gPreferences.getString("device_name", "HeidelBridge");
    WifiSsid = gPreferences.getString("wifi_ssid");
    WifiPassword = gPreferences.getString("wifi_password");
    IsMqttEnabled = gPreferences.getBool("mqtt_enabled");
    MqttPort = gPreferences.getUShort("mqtt_port", 1883);
    MqttServer = gPreferences.getString("mqtt_server");
    MqttUser = gPreferences.getString("mqtt_user");
    MqttPassword = gPreferences.getString("mqtt_password");
}

// Write all settings to storage
void Settings::WriteToPersistentMemory()
{
    Logger::Trace("Writing settings to persistent memory");

    gPreferences.putString("device_name", DeviceName);
    gPreferences.putString("wifi_ssid", WifiSsid);
    gPreferences.putString("wifi_password", WifiPassword);
    gPreferences.putBool("mqtt_enabled", IsMqttEnabled);
    gPreferences.putUShort("mqtt_port", MqttPort);
    gPreferences.putString("mqtt_server", MqttServer);
    gPreferences.putString("mqtt_user", MqttUser);
    gPreferences.putString("mqtt_password", MqttPassword);
}

// Prints all settings to the logger
void Settings::Print()
{
    Logger::Debug("Using the following device settings:");

    Logger::Debug(" > Device name: %s", DeviceName.c_str());
    Logger::Debug(" > WiFi SSID: %s", WifiSsid.c_str());
    Logger::Debug(" > MQTT enabled: %s", IsMqttEnabled ? "yes" : "no");
    Logger::Debug(" > MQTT server: %s", MqttServer.c_str());
    Logger::Debug(" > MQTT port: %d", MqttPort);
    Logger::Debug(" > MQTT user: %s", MqttUser.c_str());
}