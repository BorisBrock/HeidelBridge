#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "WifiConnection.h"
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
    gWebServer.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    // Example JSON response
    String jsonResponse = "{\"message\": \"Hello, this is your API response!\", \"status\": \"success\"}";
    request->send(200, "application/json", jsonResponse); });

    // handle 404 errors
    gWebServer.onNotFound([&](AsyncWebServerRequest *request)
                          { request->send(404, "text/plain", "This resource does not exist"); });

    gWebServer.begin();
}
