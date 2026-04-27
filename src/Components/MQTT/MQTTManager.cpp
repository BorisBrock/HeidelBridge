#include <Arduino.h>
#include <WiFi.h>
#include <cmath>
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
    char PayloadBuffer[1024];
    PrefixedString gMqttTopic(128);

    // Cache variables for change detection
    int gLastPublishedVehicleState = 1;  // Disconnected = 1
    float gLastPublishedChargingCurrentLimit = -1.0f;
    float gLastPublishedChargingPower = -1.0f;
    float gLastPublishedFailsafeCurrent = -1.0f;
    float gLastPublishedEnergyMeter = -1.0f;
    float gLastPublishedChargingCurrent1 = -1.0f;
    float gLastPublishedChargingCurrent2 = -1.0f;
    float gLastPublishedChargingCurrent3 = -1.0f;
    float gLastPublishedChargingVoltage1 = -1.0f;
    float gLastPublishedChargingVoltage2 = -1.0f;
    float gLastPublishedChargingVoltage3 = -1.0f;
    float gLastPublishedTemperature = -1.0f;
    bool gLastPublishedChargingEnabled = false;
    bool gLastPublishedStandbyEnabled = false;
    uint32_t gLastPublishedUptimeS = 0;
    uint32_t gLastUptimePublishTimeMs = 0;
    uint32_t gLastWifiDisconnects = 0;
    uint32_t gLastMqttDisconnects = 0;
    uint32_t gLastModbusReadErrors = 0;
    uint32_t gLastModbusWriteErrors = 0;

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
            R"({
                "name":"Vehicle connected",
                "device_class":"plug",
                "state_topic":"%/is_vehicle_connected",
                "payload_on":"1",
                "payload_off":"0",
                "unique_id":"%_is_vehicle_connected",
                "default_entity_id":"%_is_vehicle_connected",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/binary_sensor/%/is_vehicle_charging/config",
            R"({
                "name":"Vehicle charging",
                "device_class":"battery_charging",
                "state_topic":"%/is_vehicle_charging",
                "payload_on":"1",
                "payload_off":"0",
                "unique_id":"%_is_vehicle_charging",
                "default_entity_id":"%_is_vehicle_charging",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_power/config",
            R"({
                "name":"Charging power",
                "device_class":"power",
                "state_class":"measurement",
                "state_topic":"%/charging_power",
                "unique_id":"%_charging_power",
                "default_entity_id":"%_charging_power",
                "unit_of_measurement":"W",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_current_phase1/config",
            R"({
                "name":"Charging current phase 1",
                "device_class":"current",
                "state_class":"measurement",
                "state_topic":"%/charging_current/phase1",
                "unique_id":"%_charging_current_phase1",
                "default_entity_id":"%_charging_current_phase1",
                "unit_of_measurement":"A",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_current_phase2/config",
            R"({
                "name":"Charging current phase 2",
                "device_class":"current",
                "state_class":"measurement",
                "state_topic":"%/charging_current/phase2",
                "unique_id":"%_charging_current_phase2",
                "default_entity_id":"%_charging_current_phase2",
                "unit_of_measurement":"A",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_current_phase3/config",
            R"({
                "name":"Charging current phase 3",
                "device_class":"current",
                "state_class":"measurement",
                "state_topic":"%/charging_current/phase3",
                "unique_id":"%_charging_current_phase3",
                "default_entity_id":"%_charging_current_phase3",
                "unit_of_measurement":"A",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_current_limit/config",
            R"({
                "name":"Charging current limit",
                "device_class":"current",
                "state_class":"measurement",
                "state_topic":"%/charging_current_limit",
                "unique_id":"%_charging_current_limit",
                "default_entity_id":"%_charging_current_limit",
                "unit_of_measurement":"A",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/energy_meter/config",
            R"({
                "name":"Energy meter",
                "device_class":"energy",
                "state_topic":"%/energy_meter",
                "state_class":"total_increasing",
                "unique_id":"%_energy_meter",
                "default_entity_id":"%_energy_meter",
                "unit_of_measurement":"kWh",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/temperature/config",
            R"({
                "name":"Temperature",
                "device_class":"temperature",
                "state_class":"measurement",
                "state_topic":"%/temperature",
                "unique_id":"%_temperature",
                "default_entity_id":"%_temperature",
                "unit_of_measurement":"°C",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_voltage_phase1/config",
            R"({
                "name":"Charging voltage phase 1",
                "device_class":"voltage",
                "state_class":"measurement",
                "state_topic":"%/charging_voltage/phase1",
                "unique_id":"%_charging_voltage_phase1",
                "default_entity_id":"%_charging_voltage_phase1",
                "unit_of_measurement":"V",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_voltage_phase2/config",
            R"({
                "name":"Charging voltage phase 2",
                "device_class":"voltage",
                "state_class":"measurement",
                "state_topic":"%/charging_voltage/phase2",
                "unique_id":"%_charging_voltage_phase2",
                "default_entity_id":"%_charging_voltage_phase2",
                "unit_of_measurement":"V",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/sensor/%/charging_voltage_phase3/config",
            R"({
                "name":"Charging voltage phase 3",
                "device_class":"voltage",
                "state_class":"measurement",
                "state_topic":"%/charging_voltage/phase3",
                "unique_id":"%_charging_voltage_phase3",
                "default_entity_id":"%_charging_voltage_phase3",
                "unit_of_measurement":"V",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/switch/%/control_enable_charging/config",
            R"({
                "name":"Enable Charging",
                "state_topic":"%/enable_charging",
                "command_topic":"%/control/enable_charging",
                "unique_id":"%control_enable_charging",
                "default_entity_id":"%control_enable_charging",
                "payload_on":"ON",
                "payload_off":"OFF",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/switch/%/control_standby/config",
            R"({
                "name":"Standby Mode",
                "state_topic":"%/standby_enabled",
                "command_topic":"%/control/standby",
                "unique_id":"%_control_standby",
                "default_entity_id":"%_control_standby",
                "payload_on":"ON",
                "payload_off":"OFF",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");

        PublishHomeAssistantDiscoveryTopic(
            "homeassistant/number/%/control_charging_current_limit/config",
            R"({
                "name":"Charging Current Limit",
                "command_topic":"%/control/charging_current_limit",
                "state_topic":"%/charging_current_limit",
                "min":6,
                "max":16,
                "step":1,
                "unit_of_measurement":"A",
                "unique_id":"%_control_charging_current_limit",
                "default_entity_id":"%_control_charging_current_limit",
                "device":{"identifiers":["%"],"name":"%","model":"EnergyControl","manufacturer":"Heidelberg"}})");
    }

    // Helper function for safe float comparison with tolerance
    bool FloatChanged(float newValue, float lastValue)
    {
        if (lastValue < 0) return true; // First time
        return fabs(newValue - lastValue) > Constants::MQTT::FloatComparisonTolerance;
    }

    // Publishes various MQTT status messages based on the current value index.
    // This function cycles through different types of data (e.g., vehicle state, charging current, temperature)
    // and publishes the corresponding values to the MQTT broker.
    // Change detection ensures that values are only published when they actually change,
    // reducing unnecessary MQTT traffic. The uptime value is published less frequently (every 10 seconds).
    void PublishStatusMessages()
    {
        if (gMqttClient.connected())
        {
            uint32_t currentTimeMs = millis();

            switch (gCurValueIndex)
            {
            case (MqttPublishedValues::VehicleState):
            {
                int currentState = (int)gWallbox->GetState();
                if (currentState != gLastPublishedVehicleState)
                {
                    gLastPublishedVehicleState = currentState;
                    if (currentState == 1)  // Disconnected
                    {
                        gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_connected"), 0, false, "0");
                        gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_charging"), 0, false, "0");
                        gMqttClient.publish(gMqttTopic.SetString("/vehicle_state"), 0, false, "disconnected");
                    }
                    else if (currentState == 2)  // Connected
                    {
                        gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_connected"), 0, false, "1");
                        gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_charging"), 0, false, "0");
                        gMqttClient.publish(gMqttTopic.SetString("/vehicle_state"), 0, false, "connected");
                    }
                    else if (currentState == 4)  // Charging
                    {
                        gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_connected"), 0, false, "1");
                        gMqttClient.publish(gMqttTopic.SetString("/is_vehicle_charging"), 0, false, "1");
                        gMqttClient.publish(gMqttTopic.SetString("/vehicle_state"), 0, false, "charging");
                    }
                }
            }
            break;
            case (MqttPublishedValues::ChargingCurrentLimit):
            {
                float chargingCurrentLimit = gWallbox->GetChargingCurrentLimit();
                if (chargingCurrentLimit >= 6.0f && chargingCurrentLimit <= 16.0f &&
                    FloatChanged(chargingCurrentLimit, gLastPublishedChargingCurrentLimit))
                {
                    gLastPublishedChargingCurrentLimit = chargingCurrentLimit;
                    gMqttClient.publish(gMqttTopic.SetString("/charging_current_limit"), 0, false, String(chargingCurrentLimit).c_str());
                }
            }
            break;
            case (MqttPublishedValues::ChargingPower):
            {
                float chargingPower = gWallbox->GetChargingPower();
                if (FloatChanged(chargingPower, gLastPublishedChargingPower))
                {
                    gLastPublishedChargingPower = chargingPower;
                    gMqttClient.publish(gMqttTopic.SetString("/charging_power"), 0, false, String(chargingPower).c_str());
                }
            }
            break;
            case (MqttPublishedValues::FailsafeCurrent):
            {
                float failsafeCurrent = gWallbox->GetFailsafeCurrent();
                if (FloatChanged(failsafeCurrent, gLastPublishedFailsafeCurrent))
                {
                    gLastPublishedFailsafeCurrent = failsafeCurrent;
                    gMqttClient.publish(gMqttTopic.SetString("/failsafe_current"), 0, false, String(failsafeCurrent).c_str());
                }
            }
            break;
            case (MqttPublishedValues::EnergyMeter):
            {
                float energyMeter = gWallbox->GetEnergyMeterValue() * Constants::General::FactorWhToKWh;
                if (FloatChanged(energyMeter, gLastPublishedEnergyMeter))
                {
                    gLastPublishedEnergyMeter = energyMeter;
                    gMqttClient.publish(gMqttTopic.SetString("/energy_meter"), 0, false, String(energyMeter).c_str());
                }
            }
            break;
            case (MqttPublishedValues::ChargingCurrent):
            {
                float c1, c2, c3;
                if (gWallbox->GetChargingCurrents(c1, c2, c3))
                {
                    bool changed = FloatChanged(c1, gLastPublishedChargingCurrent1) ||
                                   FloatChanged(c2, gLastPublishedChargingCurrent2) ||
                                   FloatChanged(c3, gLastPublishedChargingCurrent3);
                    if (changed)
                    {
                        gLastPublishedChargingCurrent1 = c1;
                        gLastPublishedChargingCurrent2 = c2;
                        gLastPublishedChargingCurrent3 = c3;
                        gMqttClient.publish(gMqttTopic.SetString("/charging_current/phase1"), 0, false, String(c1).c_str());
                        gMqttClient.publish(gMqttTopic.SetString("/charging_current/phase2"), 0, false, String(c2).c_str());
                        gMqttClient.publish(gMqttTopic.SetString("/charging_current/phase3"), 0, false, String(c3).c_str());
                    }
                }
            }
            break;
            case (MqttPublishedValues::ChargingVoltage):
            {
                float v1, v2, v3;
                if (gWallbox->GetChargingVoltages(v1, v2, v3))
                {
                    bool changed = FloatChanged(v1, gLastPublishedChargingVoltage1) ||
                                   FloatChanged(v2, gLastPublishedChargingVoltage2) ||
                                   FloatChanged(v3, gLastPublishedChargingVoltage3);
                    if (changed)
                    {
                        gLastPublishedChargingVoltage1 = v1;
                        gLastPublishedChargingVoltage2 = v2;
                        gLastPublishedChargingVoltage3 = v3;
                        gMqttClient.publish(gMqttTopic.SetString("/charging_voltage/phase1"), 0, false, String(v1).c_str());
                        gMqttClient.publish(gMqttTopic.SetString("/charging_voltage/phase2"), 0, false, String(v2).c_str());
                        gMqttClient.publish(gMqttTopic.SetString("/charging_voltage/phase3"), 0, false, String(v3).c_str());
                    }
                }
            }
            break;
            case (MqttPublishedValues::Temperature):
            {
                float temperature = gWallbox->GetTemperature();
                if (FloatChanged(temperature, gLastPublishedTemperature))
                {
                    gLastPublishedTemperature = temperature;
                    gMqttClient.publish(gMqttTopic.SetString("/temperature"), 0, false, String(temperature).c_str());
                }
            }
            break;

            case (MqttPublishedValues::Internals):
            {
                // Only publish if values have changed
                bool statsChanged = (gStatistics.NumWifiDisconnects != gLastWifiDisconnects) ||
                                    (gStatistics.NumMqttDisconnects != gLastMqttDisconnects) ||
                                    (gStatistics.NumModbusReadErrors != gLastModbusReadErrors) ||
                                    (gStatistics.NumModbusWriteErrors != gLastModbusWriteErrors);
                if (statsChanged)
                {
                    gLastWifiDisconnects = gStatistics.NumWifiDisconnects;
                    gLastMqttDisconnects = gStatistics.NumMqttDisconnects;
                    gLastModbusReadErrors = gStatistics.NumModbusReadErrors;
                    gLastModbusWriteErrors = gStatistics.NumModbusWriteErrors;
                    gMqttClient.publish(gMqttTopic.SetString("/internal/wifi_disconnects"), 0, false, String(gStatistics.NumWifiDisconnects).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/internal/mqtt_disconnects"), 0, false, String(gStatistics.NumMqttDisconnects).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/internal/modbus_read_errors"), 0, false, String(gStatistics.NumModbusReadErrors).c_str());
                    gMqttClient.publish(gMqttTopic.SetString("/internal/modbus_write_errors"), 0, false, String(gStatistics.NumModbusWriteErrors).c_str());
                }
            }
            break;

            case (MqttPublishedValues::Discovery):
                PublishHomeAssistantDiscovery();
                break;
            }

            // Increment the current value index and wrap around
            gCurValueIndex = (gCurValueIndex + 1) % NumMqttPublishedValues;

            // Publish uptime only every 10 seconds (not at every update)
            if ((currentTimeMs - gLastUptimePublishTimeMs) >= Constants::MQTT::UptimePublishIntervalMs)
            {
                gLastUptimePublishTimeMs = currentTimeMs;
                gMqttClient.publish(gMqttTopic.SetString("/internal/uptime"), 0, false, String(gStatistics.UptimeS).c_str());
            }

            // Publish enable_charging and standby_enabled only when they change
            bool chargingEnabled = gWallbox->IsChargingEnabled();
            if (chargingEnabled != gLastPublishedChargingEnabled)
            {
                gLastPublishedChargingEnabled = chargingEnabled;
                gMqttClient.publish(gMqttTopic.SetString("/enable_charging"), 0, true,
                                   chargingEnabled ? "ON" : "OFF");
            }

            bool standbyEnabled = gWallbox->GetStandbyEnabled();
            if (standbyEnabled != gLastPublishedStandbyEnabled)
            {
                gLastPublishedStandbyEnabled = standbyEnabled;
                gMqttClient.publish(gMqttTopic.SetString("/standby_enabled"), 0, true,
                                   standbyEnabled ? "ON" : "OFF");
            }
        }
    }

    // Callback for MQTT connection
    void OnMqttConnect(bool sessionPresent)
    {
        Logger::Info("Connected to MQTT");

        // Subscribe to control topics
        gMqttClient.subscribe(gMqttTopic.SetString("/control/charging_current_limit"), 2);
        gMqttClient.subscribe(gMqttTopic.SetString("/control/enable_charging"), 2);
        gMqttClient.subscribe(gMqttTopic.SetString("/control/standby"), 2);

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

    // Callback for MQTT messages
    void OnMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
    {
        if (strcmp(gMqttTopic.SetString("/control/charging_current_limit"), topic) == 0)
        {
            float current = String(payload, len).toFloat();
            Logger::Trace("Received MQTT control command: charging current limit = %f\n", current);
            gWallbox->SetChargingCurrentLimit(current);
        }
        else if (strcmp(gMqttTopic.SetString("/control/enable_charging"), topic) == 0)
        {
            String cmd(payload, len);
            cmd.trim();
            Logger::Trace("Received MQTT control command: enable_charging = %s", cmd.c_str());
            bool enableCharging = cmd.equalsIgnoreCase("ON");
            gWallbox->SetChargingEnabled(enableCharging);
        }
        else if (strcmp(gMqttTopic.SetString("/control/standby"), topic) == 0)
        {
            String cmd(payload, len);
            cmd.trim();
            Logger::Trace("Received MQTT control command: standby = %s", cmd.c_str());
            bool enableStandby = cmd.equalsIgnoreCase("ON");
            gWallbox->SetStandbyEnabled(enableStandby);
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
