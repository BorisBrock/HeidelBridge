#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "SPIFFS.h"
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "../../Configuration/Version.h"
#include "NetworkScanner.h"
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
    gWebServer.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->redirect("/update.html"); });

    // Handle API requests
    gWebServer.on("/api/version", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", HandleApiRequestGetVersion()); });
    gWebServer.on("/api/wifi_scan_start", HTTP_POST, [this](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", HandleApiRequestStartWifiScan()); });
    gWebServer.on("/api/wifi_scan_status", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", HandleApiRequestGetWifiScanStatus()); });
    gWebServer.on("/api/settings_read", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", HandleApiRequestSettingsRead(request)); });
    gWebServer.onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                             {
                    if (request->url() == "/api/settings_write") {
                        request->send(200, "application/json", HandleApiRequestSettingsWrite(request, data));
                    } });

    gWebServer.on("/api/reboot", HTTP_POST, [this](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", HandleApiRequestReboot()); });
    gWebServer.on("/api/update", HTTP_POST, [this](AsyncWebServerRequest *request)
                  { request->send(200, "text/plain", Update.hasError() ? R"({"status": "error"})" : R"({"status": "ok"})");
                    delay(500);
                    ESP.restart(); }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
                  { HandleFirmwareUpload(request, filename, index, data, len, final); });

    // handle 404 errors
    gWebServer.onNotFound([&](AsyncWebServerRequest *request)
                          { 
                            // Print all important request information
                            Logger::Debug("404: %s", request->url().c_str());
                            Logger::Debug(" > Method: %s", request->methodToString());
                            Logger::Debug(" > Content type: %s", request->contentType().c_str());
                            Logger::Debug(" > Content length: %d", request->contentLength());
                            Logger::Debug(" > Query string: %s", request->url().c_str());
                            request->send(404, "text/plain", "This resource does not exist"); });

    gWebServer.begin();
}

// Handles the API request
String WebServer::HandleApiRequestGetVersion()
{
    Logger::Debug("Received REST API request: get version");

    JsonDocument doc;
    doc["version"] = String(Version::Major) + "." + String(Version::Minor) + "." + String(Version::Patch);
    doc["build_date"] = __DATE__;

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}

// Handles the API request
String WebServer::HandleApiRequestStartWifiScan()
{
    Logger::Debug("Received REST API request: start WiFi scan");

    NetworkScanner::StartNetworkScan();

    JsonDocument doc;
    doc["status"] = "ok";

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}

// Handles the API request
String WebServer::HandleApiRequestGetWifiScanStatus()
{
    Logger::Debug("Received REST API request: get WiFi scan status");

    JsonDocument doc;
    NetworkScanner::GetNetworkScanResults(doc);

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}

// Handles the API request
String WebServer::HandleApiRequestSettingsRead(AsyncWebServerRequest *request)
{
    Logger::Debug("Received REST API request: read settings");

    // Return settings as JSON
    JsonDocument doc;

    doc["device-name"] = Settings::Instance()->DeviceName;
    doc["wifi-ssid"] = Settings::Instance()->WifiSsid;
    doc["wifi-password"] = Settings::Instance()->WifiPassword;
    doc["mqtt-enabled"] = Settings::Instance()->IsMqttEnabled;
    doc["mqtt-server"] = Settings::Instance()->MqttServer;
    doc["mqtt-port"] = Settings::Instance()->MqttPort;
    doc["mqtt-user"] = Settings::Instance()->MqttUser;
    doc["mqtt-password"] = Settings::Instance()->MqttPassword;

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    return jsonResponse;
}

// Handles the API request
String WebServer::HandleApiRequestSettingsWrite(AsyncWebServerRequest *request, uint8_t *data)
{
    Logger::Debug("Received REST API request: write settings");

    JsonDocument jsonBuffer;
    DeserializationError error = deserializeJson(jsonBuffer, (const char *)data);
    if (error)
    {
        Logger::Error("Failed to parse JSON");
        return R"({"status": "error", "message": "Failed to parse JSON"})";
    }
    JsonObject doc = jsonBuffer.as<JsonObject>();

    if (doc["device-name"].is<String>())
    {
        Settings::Instance()->DeviceName = doc["device-name"].as<String>();
    }
    if (doc["wifi-ssid"].is<String>())
    {
        Settings::Instance()->WifiSsid = doc["wifi-ssid"].as<String>();
    }
    if (doc["wifi-password"].is<String>())
    {
        Settings::Instance()->WifiPassword = doc["wifi-password"].as<String>();
    }
    if (doc["mqtt-enabled"].is<bool>())
    {
        Settings::Instance()->IsMqttEnabled = doc["mqtt-enabled"].as<bool>();
    }
    if (doc["mqtt-server"].is<String>())
    {
        Settings::Instance()->MqttServer = doc["mqtt-server"].as<String>();
    }
    if (doc["mqtt-port"].is<uint16_t>())
    {
        Settings::Instance()->MqttPort = doc["mqtt-port"].as<uint16_t>();
    }
    if (doc["mqtt-user"].is<String>())
    {
        Settings::Instance()->MqttUser = doc["mqtt-user"].as<String>();
    }
    if (doc["mqtt-password"].is<String>())
    {
        Settings::Instance()->MqttPassword = doc["mqtt-password"].as<String>();
    }

    Settings::Instance()->WriteToPersistentMemory();
    Settings::Instance()->Print();

    return R"({"status": "ok"})";
}

// Handles the API request
String WebServer::HandleApiRequestReboot()
{
    Logger::Info("Received REST API request: restarting ESP32");

    Settings::Instance()->DeInit();
    ESP.restart();

    return R"({"status": "ok"})";
}

// Handles firmware upload for OTA flashing
void WebServer::HandleFirmwareUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (index == 0)
    {
        // First packet
        Logger::Info("Updating firmware: %s", filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        {
            Logger::Error("Starting firmware update failed: %s", Update.errorString());
        }
    }

    if (Update.write(data, len) != len)
    {
        Logger::Error("Processing firmware update failed: %s", Update.errorString());
    }

    if (final)
    {
        // Last packet
        if (Update.end(true))
        {
            Logger::Info("Firmware update complete");
        }
        else
        {
            Logger::Error("Finishing firmware update failed: %s", Update.errorString());
        }
    }
}