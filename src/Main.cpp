#include <Arduino.h>
#include "Components/WifiConnection.h"
#include "Components/OTAUpdater.h"
#include "Components/Modbus.h"
#include "Components/OCPPManager.h"

void setup()
{
  Serial.begin(115200);
  // Modbus::Init();
  WifiConnection::Init();
  OTAUpdater::Init();
  OCPPManager::Init();
}

void loop()
{
  OTAUpdater::Loop();
  OCPPManager::Loop();
  delay(1);
}