#pragma once

class IWallbox;

namespace MQTTManager
{
    // Initializes the MQTT manager
    void Init(IWallbox* wallbox);

    // Updates the MQTT manager
    void Update();

    // Applies commands and connection events received by the MQTT callbacks.
    // Call from the main loop as often as possible.
    void ProcessEvents();

    // Checks if MQTT is actually connected
    bool IsConnected();
};