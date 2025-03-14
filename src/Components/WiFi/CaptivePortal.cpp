#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "CaptivePortal.h"
#include "../Logger/Logger.h"

namespace CaptivePortal
{
    uint32_t gStartTimeMs = 0;
    bool gIsCaptivePortalActive = false;
    DNSServer gDnsServer;

    // Starts the captive portal
    void Start()
    {
        // Local IP used in captive portal mode
        const IPAddress localIp(4, 3, 2, 1);
        const IPAddress gatewayIp(4, 3, 2, 1);
        const IPAddress subnetMask(255, 255, 255, 0);

        // Configure the soft access point with a specific IP and subnet mask
        WiFi.softAPConfig(localIp, gatewayIp, subnetMask);

        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(Constants::WebServer::CaptivePortalName);

        gDnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        gDnsServer.setTTL(300);
        gDnsServer.start(53, "*", localIp);

        gIsCaptivePortalActive = true;
        gStartTimeMs = millis();
    }

    // Cyclic processing
    void Update()
    {
        if (gIsCaptivePortalActive)
        {
            gDnsServer.processNextRequest();
        }
    }

    // Gets the uptime in milliseconds
    uint32_t GetUptime()
    {
        return gIsCaptivePortalActive ? (millis() - gStartTimeMs) : 0;
    }
};
