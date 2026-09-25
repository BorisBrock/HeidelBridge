#include <Arduino.h>
#include <WiFi.h>
#include "../../Configuration/Settings.h"
#include "../Logger/Logger.h"
#include "../Ethernet/EthernetConnection.h"
#include "../WiFi/WifiConnection.h"
#include "../WiFi/WifiManager.h"
#include "Network.h"

namespace Network
{
    // Checks if the configured board uses wired Ethernet instead of WiFi
    bool IsEthernet()
    {
        return Settings::Instance()->BoardType == "olimex";
    }

    // Starts the network interface (Ethernet or WiFi) depending on the board type
    void Start()
    {
        if (IsEthernet())
        {
            EthernetConnection::Start();
        }
        else
        {
            WifiManager::Instance()->Start();
        }
    }

    // Cyclic processing
    void Update()
    {
        if (IsEthernet())
        {
            EthernetConnection::Update();
        }
        else
        {
            WifiManager::Instance()->Update();
        }
    }

    // Checks if the device is connected to a network
    bool IsConnected()
    {
        if (IsEthernet())
        {
            return EthernetConnection::IsConnected();
        }
        return WifiConnection::IsConnected();
    }

    // Returns the IP address of the device
    IPAddress GetLocalIP()
    {
        if (IsEthernet())
        {
            return EthernetConnection::GetLocalIP();
        }
        return WiFi.localIP();
    }
}
