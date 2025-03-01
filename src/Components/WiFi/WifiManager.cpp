#include <Arduino.h>
#include <ArduinoJson.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "../Logger/Logger.h"
#include "WifiConnection.h"
#include "CaptivePortal.h"
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

    // Check if WiFi credentials are available and try to connect
    bool isConnectedToWifi = false;
    if (Settings::Instance()->WifiSsid.length() > 0)
    {
        isConnectedToWifi = ConnectToWifiNetwork();
    }

    // Fallback in case no WiFi connection could be established
    if(!isConnectedToWifi)
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

    CaptivePortal::Start();
}

// Connects to a known WiFi network
bool WifiManager::ConnectToWifiNetwork()
{
    Logger::Info("Connecting to configured WiFi network");
    
    WifiConnection::Connect(Settings::Instance()->WifiSsid, Settings::Instance()->WifiPassword);

    // Wait for connection
    uint32_t startTimeMs = millis();
    while (!WifiConnection::IsConnected())
    {
        delay(100);
        if (millis() - startTimeMs > Constants::WiFi::ConnectionTimeoutMs)
        {
            Logger::Error(" -> Failed to connect to WiFi network");
            return false;
        }
    }

    Logger::Trace(" -> WiFi connection established");
    return true;
}