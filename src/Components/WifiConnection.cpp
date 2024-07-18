#include <Arduino.h>
#include <WiFiManager.h>
#include "WifiConnection.h"

namespace WifiConnection
{
    WiFiManager gWifiManager;

    void Init()
    {
        gWifiManager.setConfigPortalTimeout(180);
        gWifiManager.setTitle("HeidelBridge");
        gWifiManager.setShowInfoUpdate(false);

        gWifiManager.autoConnect("HeidelBridge", "heidelbridge");
    }
};