#pragma once

class WebServer
{
public:
    // Gets the singleton instance
    static WebServer *Instance();

    // Initializes the asynchronous web server
    void Init();

private:
    // Handles the API request
    String HandleApiRequestVersion();

    // Handles the API request
    String HandleApiRequestWifiStatus();

    // Handles the API request
    String HandleApiRequestWifiScan();
};