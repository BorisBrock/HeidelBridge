#pragma once

namespace Constants
{
    namespace Pins
    {
        extern const uint8_t PinRX;
        extern const uint8_t PinTX;
        extern const uint8_t PinRTS;
    };

    namespace WiFi
    {
        extern const char *HotspotSSID;
        extern const char *HotspotPassword;
        extern const char *CaptivePortalTitle;
    };

    namespace OCPP
    {
        extern const char *ChargeBoxId;
        extern const char *ChargePointModel;
        extern const char *ChargePointVendor;
        extern const char *OCPPGUIParameterTitle;
        extern const char *OCPPGUIParameterInitValue;
    };

    namespace Wallbox
    {
        extern const float FailSafeCurrentA;
    };
};
