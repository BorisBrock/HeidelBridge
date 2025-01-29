#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "../../Configuration/Version.h"
#include "WifiConnection.h"
#include "../MQTT/MQTTManager.h"
#include "../Logger/Logger.h"
#include "WebServer.h"

AsyncWebServer gWebServer(Constants::WebServer::Port);

WebServer *WebServer::Instance()
{
    static WebServer instance;
    return &instance;
}

// Initializes the asynchronous web server
void WebServer::Init()
{
    if (!Constants::WebServer::Enabled)
    {
        Logger::Info("Web server is disabled");
        return;
    }

    Logger::Info("Initializing SPIFFS");
    if (!SPIFFS.begin(true))
    {
        Logger::Error("Failed to initialize SPIFFS");
        return;
    }

    Logger::Info("Initializing web server");

    // Serve static files from SPIFFS
    gWebServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    // Handle API requests
    gWebServer.on("/api/index", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", HandleIndexApiRequest()); });

    // handle 404 errors
    gWebServer.onNotFound([&](AsyncWebServerRequest *request)
                          { request->send(404, "text/plain", "This resource does not exist"); });

    gWebServer.begin();
}

// Handles the index API request
String WebServer::HandleIndexApiRequest()
{
    JsonDocument doc;

    doc["wifi_ssid"] = Settings::Instance()->WifiSsid;
    doc["wifi_connected"] = WifiConnection::IsConnectedToWifi();
    doc["mqtt_server"] = Settings::Instance()->MqttServer;
    doc["mqtt_enabled"] = Settings::Instance()->IsMqttEnabled;
    doc["mqtt_connected"] = MQTTManager::IsConnected();
    doc["version"] = String(Version::Major) + "." + String(Version::Minor) + "." + String(Version::Patch);

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}