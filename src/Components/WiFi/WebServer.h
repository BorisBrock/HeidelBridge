#pragma once

class AsyncWebServerRequest;

class WebServer
{
public:
    // Gets the singleton instance
    static WebServer *Instance();

    // Initializes the asynchronous web server
    void Init();

private:
    // Handles the API request
    String HandleApiRequestGetVersion();

    // Handles the API request
    String HandleApiRequestGetWifiScanStatus();

    // Handles the API request
    String HandleApiRequestStartWifiScan();

    // Handles the API request
    String HandleApiRequestConnectWifi(AsyncWebServerRequest *request);

    // Handles the API request
    String HandleApiRequestSettingsRead(AsyncWebServerRequest *request);

    // Handles the API request
    String HandleApiRequestSettingsWrite(AsyncWebServerRequest *request);

    // Handles the API request
    String HandleApiRequestReboot();
};