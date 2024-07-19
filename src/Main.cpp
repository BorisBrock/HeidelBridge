#include <Arduino.h>
#include "Configuration/PersistentSettings.h"
#include "Components/WifiConnection.h"
#include "Components/OTAUpdater.h"
#include "Components/Modbus.h"
#include "Components/OCPPManager.h"
#include "Components/Wallbox.h"

void setup()
{
  // Configure serial communication
  Serial.begin(115200);

  // Print version info
  Serial.println("Booting HeidelBridge");
  Serial.printf("Build date: %s\n", __DATE__);
  Serial.println("");

  // Initialize persistent settings
  PersistentSettings::Init();

  // Make sure WiFi connection is up and running
  WifiConnection::Init();

  // Initialize all other modules
  Modbus::Init();
  Wallbox::PrintWallboxInfo();
  Wallbox::WriteInitialConfiguration();
  OTAUpdater::Init();
  OCPPManager::Init();
}

void loop()
{
  // Update components
  OTAUpdater::Loop();
  OCPPManager::Loop();
  delay(1);
}