#pragma once

class AsyncWebServerRequest;

class WebServer
{
public:
    // Gets the singleton instance
    static WebServer *Instance();

    // Initializes the asynchronous web server
    void Init();

    // Checks if any REST API calls have been made
    bool HadActivity();

private:
    // Handles the API request
    String HandleApiRequestGetVersion();

    // Handles the API request
    String HandleApiRequestGetWifiScanStatus();

    // Handles the API request
    String HandleApiRequestStartWifiScan();

    // Handles the API request
    String HandleApiRequestSettingsRead(AsyncWebServerRequest *request);

    // Handles the API request
    String HandleApiRequestSettingsWrite(AsyncWebServerRequest *request, uint8_t *data);

    // Handles the API request
    String HandleApiRequestReboot();

    // Handles firmware upload for OTA flashing
    bool HandleFirmwareUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

private:
    bool mHadActivity{};
};