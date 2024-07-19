#include <Arduino.h>
#include "Preferences.h"
#include "PersistentSettings.h"

namespace PersistentSettings
{
    Preferences gPreferences;

    // Constants
    const char* PrefSectionName = "heidelbridge";
    const char* SettingNameOcpp = "ocpp";

    void Init()
    {
        // Open up shred prefs in RW mode
        Serial.println("Initializing persistent settings");
        gPreferences.begin(PrefSectionName, false);
    }

    void WriteOCPPServer(const String& server)
    {
        gPreferences.putString(SettingNameOcpp, server);
    }

    String ReadOCPPServer()
    {
        return gPreferences.getString(SettingNameOcpp, String());
    }
};