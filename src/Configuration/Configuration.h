#pragma once

namespace Configuration
{
    namespace General
    {
        extern const char *DeviceName;
    }

    namespace WiFi
    {
        extern const char *SSID;
        extern const char *Password;
    }

    namespace MQTT
    {
        extern const bool Enabled;
        extern const char *Server;
        extern const char *UserName;
        extern const char *Password;
    }
}