#include <Arduino.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "../Logger/Logger.h"
#include "WifiConnection.h"
#include "WebServer.h"
#include "WifiManager.h"

WifiManager *WifiManager::Instance()
{
    static WifiManager instance;
    return &instance;
}

void WifiManager::Start()
{
    Logger::Info("Starting WiFi manager");

    // Check if WiFi credentials are available
    if (Settings::Instance()->WifiSsid.length() > 0)
    {
        ConnectToWifiNetwork();
    }
    else
    {
        // Start captive portal
        StartCaptivePortal();
    }

    // Start the web server
    WebServer::Instance()->Init();
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
    
    WifiConnection::ConnectToSsid(Settings::Instance()->WifiSsid, Settings::Instance()->WifiPassword);
}