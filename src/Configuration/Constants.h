#pragma once

namespace Constants
{
    namespace Pins
    {
        // Pin connections:
        // ESP32 GPIO18 -> MAXRS485 RO (Receiver Output)
        // ESP32 GPIO19 -> MAXRS485 DI (Driver Input)
        // ESP32 GPIO21 -> MAXRS485 DE+RE
        constexpr uint8_t PinRX = GPIO_NUM_18;
        constexpr uint8_t PinTX = GPIO_NUM_19;
        constexpr uint8_t PinRTS = GPIO_NUM_21;
    };

    namespace WiFi
    {
        constexpr const char *HotspotSSID = "HeidelBridge";
        constexpr const char *HotspotPassword = "heidelbridge";
        constexpr const char *CaptivePortalTitle = "WiFi Setup";
        constexpr const char *HostName = "heidelbridge";
    };

    namespace HeidelbergWallbox
    {
        constexpr float FailSafeCurrentA = 16.0f;
        constexpr float CurrentFactor = 0.1f;
        constexpr float VoltageFactor = 1.0f;
        constexpr float InitialChargingCurrentLimitA = 16.0f;
        constexpr uint8_t ModbusServerId = 1;
        constexpr uint16_t ModbusBaudrate = 19200;
        constexpr uint16_t ModbusTimeoutMs = 2000;
    };

    namespace HeidelbergRegisters
    {
        constexpr uint16_t DisableStandby = 258;
        constexpr uint16_t ChargingState = 5;
        constexpr uint16_t MaximalCurrent = 261;
        constexpr uint16_t Power = 14;
        constexpr uint16_t Energy = 15;
        constexpr uint16_t FailsafeCurrent = 262;
        constexpr uint16_t Currents = 6;
        constexpr uint16_t Voltages = 10;
    };

    namespace DummyWallbox
    {
        constexpr float ChargingVoltageV = 230.0;
        constexpr uint8_t NumPhases = 3;
        constexpr float MaxChargingCurrentA = 16.0;
        constexpr float FailSafeCurrentA = 16.0f;
    };

    namespace DaheimladenRegisters
    {
        constexpr uint16_t Status = 0;
        constexpr uint16_t LimitChargingCurrent = 91;
        constexpr uint16_t ConnectionTimeoutTime = 89;
        constexpr uint16_t EnergyMeter = 28;
        constexpr uint16_t TotalChargingPower = 12;
        constexpr uint16_t ChargeCurrents = 5;
        constexpr uint16_t ChargeVoltages = 108;
        constexpr uint16_t MaxChargingCurrentAfterConnectionLoss = 87;
        constexpr uint16_t RfidStationId = 38;
        constexpr uint16_t RfidCardId = 54;
    };

    namespace DaheimladenWallbox
    {
        constexpr uint8_t ServerId = 0xFF;
        constexpr uint16_t TCPPort = 502;
        constexpr uint8_t MaxClients = 8;
        constexpr uint32_t Timeout = 2000;
        constexpr float VoltageFactor = 10.0f;
        constexpr float CurrentFactor = 10.0f;
        constexpr float EnergyFactor = 10.0f;
    };
};
