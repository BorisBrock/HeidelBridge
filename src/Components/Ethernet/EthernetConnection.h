#pragma once

#include <IPAddress.h>

namespace EthernetConnection
{
    // Starts the LAN8720 Ethernet interface of the Olimex ESP32-POE-WROVER.
    // DHCP is used for the IP configuration.
    void Start();

    // Cyclic processing: monitors the link state and counts disconnects
    void Update();

    // Returns true once Ethernet has received a valid IP address.
    bool IsConnected();

    // Returns the currently assigned IP address (0.0.0.0 if not connected)
    IPAddress GetLocalIP();
}
