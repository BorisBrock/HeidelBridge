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
#include "../../Configuration/Credentials.h"
#include "../Wallbox/IWallbox.h"
#include "MQTTManager.h"

namespace MQTTManager
{
    AsyncMqttClient gMqttClient;
    TimerHandle_t gMqttReconnectTimer;
    TimerHandle_t gMqttPublishTimer;
    IWallbox *gWallbox = nullptr;
    uint8_t gCurValueIndex = 0;

    // Topics
    String ChargingCurrentControl = "heidelbridge/control/charging_current_limit";

    constexpr uint16_t NumMqttPublishedValues = 8;
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
    };

    void ConnectToMqtt()
    {
        if (!gMqttClient.connected())
        {
            Logger::Info("Connecting to MQTT broker...");
            gMqttClient.connect();
        }
    }

    void PublishMessages()
    {
        if (gMqttClient.connected())
        {
            switch (gCurValueIndex)
            {
            case (MqttPublishedValues::VehicleState):
                switch (gWallbox->GetState())
                {
                case (VehicleState::Disconnected):
                    gMqttClient.publish("heidelbridge/is_vehicle_connected", 0, false, "0");
                    gMqttClient.publish("heidelbridge/is_vehicle_charging", 0, false, "0");
                    gMqttClient.publish("heidelbridge/vehicle_state", 0, false, "disconnected");
                    break;
                case (VehicleState::Connected):
                    gMqttClient.publish("heidelbridge/is_vehicle_connected", 0, false, "1");
                    gMqttClient.publish("heidelbridge/is_vehicle_charging", 0, false, "0");
                    gMqttClient.publish("heidelbridge/vehicle_state", 0, false, "connected");
                    break;
                case (VehicleState::Charging):
                    gMqttClient.publish("heidelbridge/is_vehicle_connected", 0, false, "1");
                    gMqttClient.publish("heidelbridge/is_vehicle_charging", 0, false, "1");
                    gMqttClient.publish("heidelbridge/vehicle_state", 0, false, "charging");
                    break;
                }
                break;
            case (MqttPublishedValues::ChargingCurrentLimit):
                gMqttClient.publish("heidelbridge/charging_current_limit", 0, false, String(gWallbox->GetChargingCurrentLimit()).c_str());
                break;
            case (MqttPublishedValues::ChargingPower):
                gMqttClient.publish("heidelbridge/charging_power", 0, false, String(gWallbox->GetChargingPower()).c_str());
                break;
            case (MqttPublishedValues::FailsafeCurrent):
                gMqttClient.publish("heidelbridge/failsafe_current", 0, false, String(gWallbox->GetFailsafeCurrent()).c_str());
                break;
            case (MqttPublishedValues::EnergyMeter):
                gMqttClient.publish("heidelbridge/energy_meter", 0, false, String(gWallbox->GetEnergyMeterValue() * Constants::General::FactorWhToKWh).c_str());
                break;
            case (MqttPublishedValues::ChargingCurrent):
                float c1, c2, c3;
                if (gWallbox->GetChargingCurrents(c1, c2, c3))
                {
                    gMqttClient.publish("heidelbridge/charging_current/phase1", 0, false, String(c1).c_str());
                    gMqttClient.publish("heidelbridge/charging_current/phase2", 0, false, String(c2).c_str());
                    gMqttClient.publish("heidelbridge/charging_current/phase3", 0, false, String(c3).c_str());
                }
                break;
            case (MqttPublishedValues::ChargingVoltage):
                float v1, v2, v3;
                if (gWallbox->GetChargingVoltages(v1, v2, v3))
                {
                    gMqttClient.publish("heidelbridge/charging_voltage/phase1", 0, false, String(v1).c_str());
                    gMqttClient.publish("heidelbridge/charging_voltage/phase2", 0, false, String(v2).c_str());
                    gMqttClient.publish("heidelbridge/charging_voltage/phase3", 0, false, String(v3).c_str());
                }
                break;
            case (MqttPublishedValues::Temperature):
                gMqttClient.publish("heidelbridge/temperature", 0, false, String(gWallbox->GetTemperature()).c_str());
                break;
            }
            gCurValueIndex = (gCurValueIndex + 1) % NumMqttPublishedValues;

            // Always publish internals
            gMqttClient.publish("heidelbridge/internal/uptime", 0, false, String(gStatistics.UptimeS).c_str());
            gMqttClient.publish("heidelbridge/internal/wifi_disconnects", 0, false, String(gStatistics.NumWifiDisconnects).c_str());
            gMqttClient.publish("heidelbridge/internal/mqtt_disconnects", 0, false, String(gStatistics.NumMqttDisconnects).c_str());
            gMqttClient.publish("heidelbridge/internal/modbus_read_rrors", 0, false, String(gStatistics.NumModbusReadErrors).c_str());
            gMqttClient.publish("heidelbridge/internal/modbus_write_errors", 0, false, String(gStatistics.NumModbusWriteErrors).c_str());
        }
    }

    void OnMqttConnect(bool sessionPresent)
    {
        Logger::Info("Connected to MQTT");

        // Subscribe to control topics
        gMqttClient.subscribe(ChargingCurrentControl.c_str(), 2);

        // Publish version information
        String versionString = String(Version::Major) + "." + String(Version::Minor) + "." + String(Version::Patch);
        gMqttClient.publish("heidelbridge/version", 0, true, versionString.c_str());
        gMqttClient.publish("heidelbridge/build_date", 0, true, __DATE__);
        gMqttClient.publish("heidelbridge/ip_address", 0, true, WiFi.localIP().toString().c_str());
    }

    void OnMqttDisconnect(AsyncMqttClientDisconnectReason reason)
    {
        Logger::Warning("Disconnected from MQTT");
        gStatistics.NumMqttDisconnects++;
    }

    void OnMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
    {
        if (ChargingCurrentControl == topic)
        {
            float current = String(payload, len).toFloat();
            Logger::Trace("Received MQTT control command: charging current limit = %f\n", current);
            gWallbox->SetChargingCurrentLimit(current);
        }
    }

    void OnMqttPublish(uint16_t packetId)
    {
        // Message was sent successfully
    }

    void Init(IWallbox *wallbox)
    {
        Logger::Info("Initializing MQTT");

        gWallbox = wallbox;

        // Register event callbacks
        gMqttClient.onConnect(OnMqttConnect);
        gMqttClient.onDisconnect(OnMqttDisconnect);
        gMqttClient.onMessage(OnMqttMessage);
        gMqttClient.onPublish(OnMqttPublish);

        // Configure the server
        gMqttClient.setServer(Credentials::MQTT::Server, Constants::MQTT::Port);

        // Set credentials
        if (strlen(Credentials::MQTT::UserName) > 0)
            gMqttClient.setCredentials(Credentials::MQTT::UserName, Credentials::MQTT::Password);

        // Create the reconnection timer
        gMqttReconnectTimer = xTimerCreate(
            "mqttConnect",
            pdMS_TO_TICKS(Constants::MQTT::ReconnectIntervalMs),
            pdTRUE,
            (void *)0,
            reinterpret_cast<TimerCallbackFunction_t>(ConnectToMqtt));
        xTimerStart(gMqttReconnectTimer, 0);

        // Create the publish timer
        gMqttPublishTimer = xTimerCreate(
            "mqttPublish",
            pdMS_TO_TICKS(Constants::MQTT::PublishIntervalMs),
            pdTRUE,
            (void *)0,
            reinterpret_cast<TimerCallbackFunction_t>(PublishMessages));
        xTimerStart(gMqttPublishTimer, pdMS_TO_TICKS(Constants::MQTT::PublishIntervalMs));
    }
}