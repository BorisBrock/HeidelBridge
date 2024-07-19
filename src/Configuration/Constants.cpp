#include <Arduino.h>
#include "Constants.h"

namespace Constants
{
    namespace Pins
    {
        // Pin connections:
        // ESP32 GPIO18 -> MAXRS485 RO (Receiver Output)
        // ESP32 GPIO19 -> MAXRS485 DI (Driver Input)
        // ESP32 GPIO21 -> MAXRS485 DE+RE
        const uint8_t PinRX = GPIO_NUM_18;
        const uint8_t PinTX = GPIO_NUM_19;
        const uint8_t PinRTS = GPIO_NUM_21;
    };

    namespace WiFi
    {
        const char *HotspotSSID = "HeidelBridge";
        const char *HotspotPassword = "heidelbridge";
        const char *CaptivePortalTitle = "WiFi Setup";
    };

    namespace OCPP
    {
        const char *ChargeBoxId = "heidelberg-ec";
        const char *ChargePointModel = "Heidelberg EC";
        const char *ChargePointVendor = "Heidelberg";
        const char *OCPPGUIParameterTitle = "OCPP Server";
        const char* OCPPGUIParameterInitValue = "ws://192.168.178.90:8887";
    };
};
