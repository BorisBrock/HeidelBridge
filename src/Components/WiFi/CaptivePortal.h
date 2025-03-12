#pragma once

namespace CaptivePortal
{
    // Starts the captive portal
    void Start();

    // Cyclic processing
    void Update();

    // Gets the uptime in milliseconds
    uint32_t GetUptime();
};