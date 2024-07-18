#include <Arduino.h>
#include "Components/WifiConnection.h"
#include "Components/OTAUpdater.h"
#include "Components/Modbus.h"
#include "Components/OCPPManager.h"

void setup()
{
  // Configure serial communication
  Serial.begin(115200);

  // Initialize all modules
  WifiConnection::Init();
  Modbus::Init();
  OTAUpdater::Init();
  OCPPManager::Init();
}

void loop()
{
  // Update components
  OTAUpdater::Loop();
  OCPPManager::Loop();
}