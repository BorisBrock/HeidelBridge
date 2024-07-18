#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "OCPPManager.h"
#include "OTAUpdater.h"

namespace OTAUpdater
{
    void Init()
    {
        ArduinoOTA.onStart([]()
                           { Serial.println("OTA update starting"); 
                       OCPPManager::Shutdown(); });

        ArduinoOTA.onEnd([]()
                         { Serial.println("OTA update finished"); });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                              { Serial.printf("OTA update progress: %u%%\r", (progress / (total / 100))); });

        ArduinoOTA.onError([](ota_error_t error)
                           {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

        Serial.println("Starting OTA server");
        ArduinoOTA.begin();
    }

    void Loop()
    {
        ArduinoOTA.handle();
    }
};