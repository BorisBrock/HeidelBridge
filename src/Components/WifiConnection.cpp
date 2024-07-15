#include <Arduino.h>
#include <WiFiManager.h>
#include "WifiConnection.h"

WiFiManager wifiManager;

void WifiConnection::Init()
{
    wifiManager.setConfigPortalTimeout(180);
    wifiManager.setTitle("HeidelBridge");
    wifiManager.setShowInfoUpdate(false);

    wifiManager.autoConnect("HeidelBridge", "heidelbridge");
}