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

// Checks if the device has been configured with WiFi credentials
bool Settings::HasWifiCredentials()
{
    return gPreferences.getString("wifi_ssid") != "" && gPreferences.getString("wifi_password") != "";
}