#include <Arduino.h>
#include <ArduinoJson.h>
#include "Configuration/Constants.h"
#include "Configuration/Settings.h"
#include "Configuration/Version.h"
#include "Components/Logger/Logger.h"
#include "Components/Statistics/Statistics.h"
#include "Components/WiFi/WifiConnection.h"
#include "Components/WiFi/WifiManager.h"
#include "Components/Modbus/ModbusRTU.h"
#include "Components/Modbus/ModbusTCP.h"
#include "Components/Wallbox/DummyWallbox.h"
#include "Components/Wallbox/HeidelbergWallbox.h"
#include "Components/MQTT/MQTTManager.h"
#include "Components/AsyncDelay/AsyncDelay.h"

IWallbox *gWallbox{nullptr};
AsyncDelay gUptimeCounter(Constants::General::MillisPerSecond);
AsyncDelay gMqttUpdater(Constants::MQTT::PublishIntervalMs);

void setup()
{
  // Configure serial communication
  Serial.begin(115200);

  // Print startup information
  Logger::Print("Booting HeidelBridge");
  Logger::Print("  Version: %d.%d.%d", Version::Major, Version::Minor, Version::Patch);
  Logger::Print("  Build date: %s", __DATE__);
  Logger::Print("");
  Logger::Print("Logging level: %s", Logger::GetLogLevel());
  Logger::Print("");
#ifndef DUMMY_WALLBOX
  Logger::Print("Wallbox mode: real hardware (Heidelberg)");
#else
  Logger::Print("Wallbox mode: simulated (dummy)");
#endif
  Logger::Print("");

  // Read persistent settings
  Settings::Instance()->Init();
  Settings::Instance()->ReadFromPersistentMemory();
  Settings::Instance()->Print();

  // Make sure WiFi connection is up and running
  WifiManager::Instance()->Start();

  // Initialize wallbox
#ifndef DUMMY_WALLBOX
  Logger::Info("Starting with Heidelberg wallbox in real mode");
  ModbusRTU::Instance()->Init();
  gWallbox = HeidelbergWallbox::Instance();
#else
  Logger::Info("Starting with dummy wallbox in simulated mode");
  gWallbox = DummyWallbox::Instance();
#endif
  gWallbox->Init();

  // Start Modbus TCP server
  ModbusTCP::Init(gWallbox);

  // Set up MQTT
  if (Settings::Instance()->IsMqttEnabled)
  {
    MQTTManager::Init(gWallbox);
  }

  // Start async delays
  gUptimeCounter.Restart();
  gMqttUpdater.Restart();

  Logger::Info("Setup complete");
}

void loop()
{
  if (gUptimeCounter.IsElapsed())
  {
    gUptimeCounter.Restart();
    gStatistics.UptimeS++;
  }

  if (Settings::Instance()->IsMqttEnabled && gMqttUpdater.IsElapsed())
  {
    gMqttUpdater.Restart();
    MQTTManager::Update();
  }

  WifiManager::Instance()->Update();

  yield();
}