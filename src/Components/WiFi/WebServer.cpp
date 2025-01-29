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

    // Required
    gWebServer.on("/connecttest.txt", [](AsyncWebServerRequest *request)
                  { request->redirect("http://logout.net"); }); // windows 11 captive portal workaround
    gWebServer.on("/wpad.dat", [](AsyncWebServerRequest *request)
                  { request->send(404); }); // Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

    // Background responses: Probably not all are Required, but some are. Others might speed things up?
    // A Tier (commonly used by modern systems)
    gWebServer.on("/generate_204", [](AsyncWebServerRequest *request)
                  { request->redirect(Constants::WebServer::LocalIpUrl); }); // android captive portal redirect
    gWebServer.on("/redirect", [](AsyncWebServerRequest *request)
                  { request->redirect(Constants::WebServer::LocalIpUrl); }); // microsoft redirect
    gWebServer.on("/hotspot-detect.html", [](AsyncWebServerRequest *request)
                  { request->redirect(Constants::WebServer::LocalIpUrl); }); // apple call home
    gWebServer.on("/canonical.html", [](AsyncWebServerRequest *request)
                  { request->redirect(Constants::WebServer::LocalIpUrl); }); // firefox captive portal call home
    gWebServer.on("/success.txt", [](AsyncWebServerRequest *request)
                  { request->send(200); }); // firefox captive portal call home
    gWebServer.on("/ncsi.txt", [](AsyncWebServerRequest *request)
                  { request->redirect(Constants::WebServer::LocalIpUrl); }); // windows call home

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