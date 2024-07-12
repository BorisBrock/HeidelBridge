#include <Arduino.h>
#include <WiFi.h>
#include "HardwareSerial.h"
#include "ModbusBridgeWiFi.h"
#include "ModbusClientRTU.h"
#include "../Configuration/Pins.h"
#include "Modbus.h"

// Modbus TCP: Port 502, 4 simultaneous clients allowed, 600ms inactivity to disconnect client
constexpr uint16_t ModbusTcpPort = 502;
constexpr uint16_t ModbusTcpMaxClients = 4;
constexpr uint16_t ModbusTcpClientTimeoutMs = 600;
constexpr uint8_t ModbusAliasId = 1;
constexpr uint8_t ModbusServerId = 1;

ModbusClientRTU gModbusRTU(PIN_RTS); // Create a ModbusRTU client instance
ModbusBridgeWiFi gModbusBridge;      // Create bridge
HardwareSerial gRs485Serial(1);      // define a Serial for UART1

void Modbus::Init()
{
    // Init serial conneted to the RTU Modbus
    Serial.println("Starting RS485 hardware serial");
    RTUutils::prepareHardwareSerial(gRs485Serial);
    gRs485Serial.begin(19200, SERIAL_8N1, PIN_RX, PIN_TX);

    // Start Modbus RTU
    Serial.println("Creating Modbus RTU instance");
    gModbusRTU.setTimeout(2000);    // Set RTU Modbus message timeout to 2000ms
    gModbusRTU.begin(gRs485Serial); // Start ModbusRTU background task

    // Define and start WiFi bridge
    Serial.println("Creating modbus bridge");
    gModbusBridge.attachServer(ModbusAliasId, ModbusServerId, ANY_FUNCTION_CODE, &gModbusRTU);
    gModbusBridge.listServer(); // Check: print out all combinations served to Serial

    // Start the bridge. Port 502, 4 simultaneous clients allowed, 600ms inactivity to disconnect client
    Serial.println("Starting Modbus bridge");
    gModbusBridge.start(ModbusTcpPort, ModbusTcpMaxClients, ModbusTcpClientTimeoutMs);
}