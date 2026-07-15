#pragma once

#include <Arduino.h>

namespace DeviceInfo
{
    // Returns a stable, unique-per-device serial number string derived from the ESP32's factory eFuse MAC address
    String GetDeviceSerialNumber();
}
