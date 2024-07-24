#include <Arduino.h>
#include "Components/WiFi/WifiConnection.h"
#include "Components/Modbus/ModbusRTU.h"
#include "Components/Modbus/ModbusTCP.h"
#include "Components/Wallbox/DummyWallbox.h"
#include "Components/Wallbox/HeidelbergWallbox.h"

IWallbox *gWallbox{nullptr};

void setup()
{
  // Configure serial communication
  Serial.begin(115200);

  // Print version info
  Serial.println("Booting HeidelBridge");
  Serial.printf("Build date: %s\n", __DATE__);
  Serial.println("");

  // Make sure WiFi connection is up and running
  WifiConnection::Init();

  // Initialize all other modules
#ifndef DUMMY_WALLBOX
  Serial.println("Starting with Heidelberg wallbox in real mode");
  ModbusRTU::Instance()->Init();
  gWallbox = HeidelbergWallbox::Instance();
#else
  Serial.println("Starting with dummy wallbox in simulated mode");
  gWallbox = DummyWallbox::Instance();
#endif

  gWallbox->Init();
  ModbusTCP::Init(gWallbox);

  Serial.println("Setup complete");
}

void loop()
{
  delay(1);
}