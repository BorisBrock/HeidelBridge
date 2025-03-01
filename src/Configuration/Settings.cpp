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

    WifiSsid = gPreferences.getString("wifi_ssid");
    WifiPassword = gPreferences.getString("wifi_password");
    IsMqttEnabled = gPreferences.getBool("mqtt_port");
    MqttPort = gPreferences.getUShort("mqtt_port", 1833);
    MqttServer = gPreferences.getString("mqtt_server");
    MqttUser = gPreferences.getString("mqtt_user");
    MqttPassword = gPreferences.getString("mqtt_password");
}