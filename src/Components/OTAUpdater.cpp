#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "OTAUpdater.h"

void OTAUpdater::Init()
{
    Serial.println("Starting OTA server");
    ArduinoOTA.begin();
}

void OTAUpdater::Loop()
{
    ArduinoOTA.handle();
}