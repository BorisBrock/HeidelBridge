#include <Arduino.h>
#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "../Logger/Logger.h"
#include "../Statistics/Statistics.h"
#include "../../Configuration/Version.h"
#include "../../Configuration/Constants.h"
#include "../../Configuration/Settings.h"
#include "../Wallbox/IWallbox.h"
#include "../../Utils/PrefixedString.h"
#include "../../Utils/StringUtils.h"
#include "MQTTManager.h"

namespace MQTTManager
{
    AsyncMqttClient gMqttClient;
    TimerHandle_t gMqttReconnectTimer;
    TimerHandle_t gMqttPublishTimer;
    IWallbox *gWallbox = nullptr;
    uint8_t gCurValueIndex = 0;
    char TopicBuffer[128];
    char PayloadBuffer[512];
    PrefixedString gMqttTopic(128);

    constexpr uint16_t NumMqttPublishedValues = 10;
    enum MqttPublishedValues
    {
        VehicleState,
        ChargingCurrentLimit,
        ChargingPower,
        FailsafeCurrent,
        EnergyMeter,
        ChargingCurrent,
        ChargingVoltage,
        Temperature,
        Internals,
        Discovery
    };

    float gPreviousCurrentLimit = 16.0f; // Default value, adjust if necessary

    // Connects to the MQTT broker if not already connected
    void ConnectToMqtt()
    {
        if (!gMqttClient.connected())
        {
            Logger::Info("Connecting to MQTT broker...");
            gMqttClient.connect();
        }
    }

    // Publishes a single MQTT discovery message for Home Assistant integration
    void PublishHomeAssistantDiscoveryTopic(const char *topic, const char *payload)
    {
        StringUtils::InsertString(topic, TopicBuffer, sizeof(TopicBuffer), '%', Settings::Instance()->DeviceName.c_str());
        StringUtils::InsertString(payload, PayloadBuffer, sizeof(PayloadBuffer), '%', Settings::Instance()->DeviceName.c_str());

        gMqttClient.publish(TopicBuffer, 1, false, PayloadBuffer);
    }

    // Publishes MQTT discovery messages for Home Assistant integration
    void PublishHomeAssistantDiscovery()
    {
        // Publish Home Assistant MQTT discovery messages
        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/binary_sensor/%/is_vehicle_connected/config",
            R"({"name":"Vehicle connected","device_class":"plug","state_topic":"%/is_vehicle_connected","payload_on":"1","payload_off":"0","unique_id":"%_is_vehicle_connected","object_id":"is_vehicle_connected","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/binary_sensor/%/is_vehicle_charging/config",
            R"({"name":"Vehicle charging","device_class":"battery_charging","state_topic":"%/is_vehicle_charging","payload_on":"1","payload_off":"0","unique_id":"%_is_vehicle_charging","object_id":"is_vehicle_charging","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_power/config",
            R"({"name":"Charging power","device_class":"power","state_topic":"%/charging_power","unique_id":"%_charging_power","object_id":"charging_power","unit_of_measurement":"W","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_current/config",
            R"({"name":"Charging current","device_class":"current","state_topic":"%/charging_current/phase1","unique_id":"%_charging_current","object_id":"charging_current","unit_of_measurement":"A","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_current_limit/config",
            R"({"name":"Charging current limit","device_class":"current","state_topic":"%/charging_current_limit","unique_id":"%_charging_current_limit","object_id":"charging_current_limit","unit_of_measurement":"A","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/energy_meter/config",
            R"({"name":"Energy meter","device_class":"energy","state_topic":"%/energy_meter","state_class":"total_increasing","unique_id":"%_energy_meter","object_id":"energy_meter","unit_of_measurement":"kWh","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/temperature/config",
            R"({"name":"Temperature","device_class":"temperature","state_topic":"%/temperature","unique_id":"%_temperature","object_id":"temperature","unit_of_measurement":"°C","device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");
    
        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/switch/%/enable_charging/config",
            R"({
                "name": "Enable Charging",
                "state_topic": "%/enable_charging",
                "command_topic": "%/control/enable_charging",
                "unique_id": "%_enable_charging_switch",
                "object_id": "enable_charging",
                "payload_on": "ON",
                "payload_off": "OFF",
                 "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");
        
        PublishHomeAssistantDiscoveryTopic( 
            "homeassistant/number/%/charging_current_limit/config", 
            R"({"name": "Charging Current Limit", "command_topic": "%/control/charging_current_limit", "state_topic": "%/status/charging_current_limit", "min": 6, "max": 16, "step": 1, "unit_of_measurement": "A", "uniq_id": "heidelbridge_charging_current_limit", "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");
 }

    // Publishes various MQTT status messages based on the current value index.
    // This function cycles through different types of data (e.g., vehicle state, charging current, temperature)
    // and publishes the corresponding values to the MQTT broker. It ensures that all relevant data points
    // are periodically updated and sent to the MQTT broker for monitoring and control purposes.
    void PublishStatusMessages()
    {
        if (gMqttClient.connected())
        {
            switch (gCurValueIndex)
            {
            case (MqttPublishedValues::VehicleState):
                switch (gWallbox->GetState())
                {
                case (VehicleState::Disconnected):
                    gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_connected"), 0, false, "0");
                    gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_charging"), 0, false, "0");
                    gMqttClient.publish(gMqttTopic.SetString("/vehicle_state"), 0, false, "disconnected");
                    break;
                case (VehicleState::Connected):
                    gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_connected"), 0, false, "1");
                    gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_charging"), 0, false, "0");
                    gMqttClient.publish(gMqttTopic.SetString("/vehicle_state"), 0, false, "connected");
                    break;
                case (VehicleState::Charging):
                    gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_connected"), 0, false, "1");
                    gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_charging"), 0, false, "1");
                    gMqttClient.publish(gMqttTopic.SetString("/vehicle_state"), 0, false, "charging");
                    break;
                }
                break;
            case (MqttPublishedValues::ChargingCurrentLimit):
                gMqttClient.publish(gMqttTopic.SetString("/charging_current_limit"), 0, false, String(gWallbox->GetChargingCurrentLimit()).c_str());
                break;
            case (MqttPublishedValues::ChargingPower):
                gMqttClient.publish(gMqttTopic.SetString("/charging_power"), 0, false, String(gWallbox->GetChargingPower()).c_str());
                break;
            case (MqttPublishedValues::FailsafeCurrent):
                gMqttClient.publish(gMqttTopic.SetString("/failsafe_current"), 0, false, String(gWallbox->GetFailsafeCurrent()).c_str());
                break;
            case (MqttPublishedValues::EnergyMeter):
                gMqttClient.publish(gMqttTopic.SetString("/energy_meter"), 0, false, String(gWallbox->GetEnergyMeterValue() * Constants::General::FactorWhToKWh).c_str());
                break;
            case (MqttPublishedValues::ChargingCurrent):
                float c1, c2, c3;
                if (gWallbox->GetChargingCurrents(c1, c2, c3))
                {
                    gMqttClient.publish(gMqttTopic.SetString("/charging_current/phase1"), 0, false, String(c1).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/charging_current/phase2"), 0, false, String(c2).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/charging_current/phase3"), 0, false, String(c3).c_str());
                }
                break;
            case (MqttPublishedValues::ChargingVoltage):
                float v1, v2, v3;
                if (gWallbox->GetChargingVoltages(v1, v2, v3))
                {
                    gMqttClient.publish(gMqttTopic.SetString("/charging_voltage/phase1"), 0, false, String(v1).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/charging_voltage/phase2"), 0, false, String(v2).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/charging_voltage/phase3"), 0, false, String(v3).c_str());
                }
                break;
            case (MqttPublishedValues::Temperature):
                gMqttClient.publish(gMqttTopic.SetString("/temperature"), 0, false, String(gWallbox->GetTemperature()).c_str());
                break;

            case (MqttPublishedValues::Internals):
                gMqttClient.publish(gMqttTopic.SetString("/internal/wifi_disconnects"), 0, false, String(gStatistics.NumWifiDisconnects).c_str());
                gMqttClient.publish(gMqttTopic.SetString("/internal/mqtt_disconnects"), 0, false, String(gStatistics.NumMqttDisconnects).c_str());
                gMqttClient.publish(gMqttTopic.SetString("/internal/modbus_read_errors"), 0, false, String(gStatistics.NumModbusReadErrors).c_str());
                gMqttClient.publish(gMqttTopic.SetString("/internal/modbus_write_errors"), 0, false, String(gStatistics.NumModbusWriteErrors).c_str());
                break;

            case (MqttPublishedValues::Discovery):
                PublishHomeAssistantDiscovery();
                break;
            }

            // Increment the current value index and wrap around if it exceeds the number of published values
            gCurValueIndex = (gCurValueIndex + 1) % NumMqttPublishedValues;

            // These values are published every cycle
            gMqttClient.publish(gMqttTopic.SetString("/internal/uptime"), 0, false, String(gStatistics.UptimeS).c_str());


            gMqttClient.publish(gMqttTopic.SetString("/enable_charging"), 0, true,
                gWallbox->GetChargingCurrentLimit() > 0.1f ? "ON" : "OFF");
        }
    }

    // Callback for MQTT connection
    void OnMqttConnect(bool sessionPresent)
    {
        Logger::Info("Connected to MQTT");

        // Subscribe to control topics
        gMqttClient.subscribe(gMqttTopic.SetString("/control/charging_current_limit"), 2);
        gMqttClient.subscribe(gMqttTopic.SetString("/control/enable_charging"), 2);


        // Publish version information
        String versionString = String(Version::Major) + "." + String(Version::Minor) + "." + String(Version::Patch);
        gMqttClient.publish(gMqttTopic.SetString("/version"), 0, true, versionString.c_str());
        gMqttClient.publish(gMqttTopic.SetString("/build_date"), 0, true, __DATE__);
        gMqttClient.publish(gMqttTopic.SetString("/ip_address"), 0, true, WiFi.localIP().toString().c_str());

        // Publish discovery data
        PublishHomeAssistantDiscovery();
    }

    // Callback for MQTT disconnection
    void OnMqttDisconnect(AsyncMqttClientDisconnectReason reason)
    {
        Logger::Warning("Disconnected from MQTT. Reason: %d", reason);
        gStatistics.NumMqttDisconnects++;
    }
    // Add this helper function to check if charging is enabled
    bool IsChargingEnabled() {
        return gWallbox->GetChargingCurrentLimit() > 0.1f;
    }
    // Callback for MQTT messages
    void OnMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
    {
        if (strcmp(gMqttTopic.SetString("/control/charging_current_limit"), topic) == 0)
        {
            float current = String(payload, len).toFloat();
            Logger::Trace("Received MQTT control command: charging current limit = %f\n", current);
            // If current is between 0 and 6, set to 0
            if (current > 0.0f && current < 6.0f) {
                current = 0.0f;
            }
            // Only set the wallbox current if charging is enabled
            if (IsChargingEnabled()) {
                gWallbox->SetChargingCurrentLimit(current);
            }
            // Always update the previous value if not 0
            if (current > 0.1f) {
                gPreviousCurrentLimit = current;
            }
        }
        else if (strcmp(gMqttTopic.SetString("/control/charging_enable"), topic) == 0)
        {
            String cmd(payload, len);
            cmd.trim();
            Logger::Trace("Received MQTT control command: charging_enable = %s\n", cmd.c_str());
            if (cmd.equalsIgnoreCase("ON"))
            {
                // ON now means charging is enabled (restore previous value)
                gWallbox->SetChargingCurrentLimit(gPreviousCurrentLimit);
            }
            else if (cmd.equalsIgnoreCase("OFF"))
            {
                // OFF now means charging is disabled (set current to 0)
                if (gWallbox->GetChargingCurrentLimit() > 0.1f) {
                    gPreviousCurrentLimit = gWallbox->GetChargingCurrentLimit();
                }
                gWallbox->SetChargingCurrentLimit(0);
            }
        }
        else if (strcmp(gMqttTopic.SetString("/control/enable_charging"), topic) == 0)
        {
            String cmd(payload, len);
            cmd.trim();
            Logger::Trace("Received MQTT control command: enable_charging = %s\n", cmd.c_str());
            if (cmd.equalsIgnoreCase("ON"))
            {
                // ON now means charging is enabled (restore previous value)
                gWallbox->SetChargingCurrentLimit(gPreviousCurrentLimit);
            }
            else if (cmd.equalsIgnoreCase("OFF"))
            {
                // OFF now means charging is disabled (set current to 0)
                if (gWallbox->GetChargingCurrentLimit() > 0.1f) {
                    gPreviousCurrentLimit = gWallbox->GetChargingCurrentLimit();
                }
                gWallbox->SetChargingCurrentLimit(0);
            }
        }
    }

    // Callback for MQTT publish
    void OnMqttPublish(uint16_t packetId)
    {
        // Message was sent successfully
    }

    // Initializes the MQTT manager with the provided wallbox instance and sets up MQTT client configurations and event callbacks
    void Init(IWallbox *wallbox)
    {
        Logger::Info("Initializing MQTT");

        gMqttTopic.SetPrefix(Settings::Instance()->DeviceName.c_str());

        gWallbox = wallbox;

        // Register event callbacks
        gMqttClient.onConnect(OnMqttConnect);
        gMqttClient.onDisconnect(OnMqttDisconnect);
        gMqttClient.onMessage(OnMqttMessage);
        gMqttClient.onPublish(OnMqttPublish);

        // Configure the server
        Logger::Trace("Using MQTT server %s:%d", Settings::Instance()->MqttServer.c_str(), Settings::Instance()->MqttPort);
        gMqttClient.setServer(Settings::Instance()->MqttServer.c_str(), Settings::Instance()->MqttPort);

        // Set credentials
        if (Settings::Instance()->MqttUser.length() > 0)
        {
            Logger::Trace("Using MQTT credentials");
            gMqttClient.setCredentials(Settings::Instance()->MqttUser.c_str(), Settings::Instance()->MqttPassword.c_str());
        }
    }

    // Updates the MQTT connection and publishes messages
    void Update()
    {
        Logger::Trace("Publishing MQTT messages");

        // Ensure MQTT is still connected
        ConnectToMqtt();

        // Publish data set
        PublishStatusMessages();
    }

    // Checks if MQTT is actually connected
    bool IsConnected()
    {
        return gMqttClient.connected();
    }


}
