#include <Arduino.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "../Logger/Logger.h"
#include "WifiConnection.h"
#include "WebServer.h"
#include "Wifimanager.h"

void WifiManager::Start()
{
    Logger::Info("Starting WiFi manager");

    // Check if WiFi credentials are available
    if (Settings::Instance()->HasWifiCredentials())
    {
        ConnectToWifiNetwork();
    }
    else
    {
        // Start captive portal
        StartCaptivePortal();
    }
}

// Starts the captive portal
void WifiManager::StartCaptivePortal()
{
    Logger::Info("Starting captive portal");
    WifiConnection::StartCaptivePortal();
}

// Connects to a known WiFi network
void WifiManager::ConnectToWifiNetwork()
{
    Logger::Info("Connecting to configured WiFi network");
    WifiConnection::ConnectToSsid(Settings::Instance().GetWifiSsid(), Settings::Instance().GetWifiPassword());
}