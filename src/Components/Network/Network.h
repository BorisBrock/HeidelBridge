#pragma once

#include <IPAddress.h>

namespace Network
{
    // Checks if the configured board uses wired Ethernet instead of WiFi
    bool IsEthernet();

    // Starts the network interface (Ethernet or WiFi) depending on the board type
    void Start();

    // Cyclic processing
    void Update();

    // Checks if the device is connected to a network
    bool IsConnected();

    // Returns the IP address of the device
    IPAddress GetLocalIP();
}
