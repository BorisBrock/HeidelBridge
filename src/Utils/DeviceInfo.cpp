#include <Arduino.h>
#include "DeviceInfo.h"

namespace DeviceInfo
{
    // Returns a stable, unique-per-device serial number string derived from the ESP32's factory eFuse MAC address
    String GetDeviceSerialNumber()
    {
        static String serial;
        if (serial.isEmpty())
        {
            const uint64_t efuseMac = ESP.getEfuseMac();
            char buffer[14];
            snprintf(buffer, sizeof(buffer), "%012llX", efuseMac);
            serial = String(buffer);
        }
        return serial;
    }
}
