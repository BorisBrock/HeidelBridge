#pragma once

namespace MQTTManager
{
    // Initializes the MQTT manager
    void Init(IWallbox* wallbox);

    // Updates the MQTT manager
    void Update();
};