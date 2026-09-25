#include <Arduino.h>

// Olimex ESP32-POE-WROVER Ethernet configuration.
// These definitions must be available before ETH.h is included.
#ifndef ETH_PHY_TYPE
#define ETH_PHY_TYPE  ETH_PHY_LAN8720
#define ETH_PHY_ADDR  0
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER 12
#define ETH_CLK_MODE  ETH_CLOCK_GPIO0_OUT
#endif

#include <ETH.h>
#include "EthernetConnection.h"
#include "../Logger/Logger.h"
#include "../Statistics/Statistics.h"
#include "../../Configuration/Settings.h"

namespace EthernetConnection
{
    bool gConnected = false;
    bool gLinkUp = false;

    void Start()
    {
        Logger::Info("Starting Ethernet (Olimex ESP32-POE-WROVER)");
        Logger::Info("  PHY: LAN8720, address: 0");
        Logger::Info("  MDC: GPIO23, MDIO: GPIO18");
        Logger::Info("  PHY power: GPIO12, RMII clock: GPIO0");

        if (!ETH.begin(
                ETH_PHY_ADDR,
                ETH_PHY_POWER,
                ETH_PHY_MDC,
                ETH_PHY_MDIO,
                ETH_PHY_TYPE,
                ETH_CLK_MODE))
        {
            Logger::Error("Failed to start Ethernet");
            gConnected = false;
            return;
        }

        ETH.setHostname(Settings::Instance()->DeviceName.c_str());

        // DHCP is used by default. Give the interface some time to obtain an IP.
        const uint32_t startTimeMs = millis();
        constexpr uint32_t IpTimeoutMs = 15000;

        while (ETH.localIP() == IPAddress(0, 0, 0, 0) &&
               (millis() - startTimeMs) < IpTimeoutMs)
        {
            delay(100);
        }

        if (ETH.localIP() != IPAddress(0, 0, 0, 0))
        {
            gConnected = true;
            Logger::Info("Ethernet connected");
            Logger::Info("  IP address: %s", ETH.localIP().toString().c_str());
            Logger::Info("  Gateway: %s", ETH.gatewayIP().toString().c_str());
            Logger::Info("  Subnet: %s", ETH.subnetMask().toString().c_str());
            Logger::Info("  MAC: %s", ETH.macAddress().c_str());
        }
        else
        {
            gConnected = false;
            Logger::Warning("Ethernet started, but no DHCP address was received");
            Logger::Warning("Check the Ethernet cable/network connection");
        }
    }

    void Update()
    {
        bool linkUp = ETH.linkUp();

        if (!linkUp && gLinkUp)
        {
            Logger::Warning("Ethernet link lost");
            gStatistics.NumEthernetDisconnects++;
        }
        else if (linkUp && !gLinkUp)
        {
            Logger::Info("Ethernet link established");
        }
        gLinkUp = linkUp;
    }

    bool IsConnected()
    {
        return ETH.linkUp() && ETH.localIP() != IPAddress(0, 0, 0, 0);
    }

    IPAddress GetLocalIP()
    {
        return ETH.localIP();
    }
}
