#pragma once

class IWallbox;

namespace MQTTManager
{
    // Initializes the MQTT manager
    void Init(IWallbox* wallbox);

    // Updates the MQTT manager
    void Update();

    // Checks if MQTT is actually connected
    bool IsConnected();
};