#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "../../Configuration/Constants.h"
#include "../Logger/Logger.h"
#include "WebServer.h"

namespace WebServer
{
    DNSServer gDnsServer;
    AsyncWebServer gWebServer(Constants::WebServer::Port);

    // Initializes the asynchronous web server
    void Init()
    {
        if(!Constants::WebServer::Enabled)
        {
            Logger::Info("Web server is disabled");
            return;
        }

        Logger::Info("Initializing web server");
    }

    // Updates the web/DNS server
    void Update()
    {
        // Todo
        gDnsServer.processNextRequest();
    }
}