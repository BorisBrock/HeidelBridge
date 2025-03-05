#include <Arduino.h>
#include "Configuration.h"

namespace Configuration
{
    namespace General
    {
        const char *DeviceName = "HeidelBridge"; // Only use letters, numbers and _ (underscore)
    }

    namespace WiFi
    {
        const char *SSID = "YourWiFiSSID";
        const char *Password = "YourWiFiPassword";
    }

    namespace MQTT
    {
        const bool Enabled = true;
        const char *Server = "YourMqttServer";
        const char *UserName = ""; // Can be left empty if authentication is not required
        const char *Password = ""; // Can be left empty if authentication is not required
    }
}