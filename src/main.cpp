#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <ld2410.h>
#include "variable.h"
#include "eepromFile.h"
#include "deviceControl.h"
#include "network.h"
#include "otaControl.h"
#include "Mqtt.h"
#include "httpRoutes.h"

bool shouldRestart = false;
bool Hflag = false;
const String &versionUrl = "link";
const char *currentVersion = "1.0.0";

unsigned long currentMillis;
unsigned long previousMillis = 0;              // Stores the last time the internet was checked
const unsigned long wifiCheckInterval = 60000; // 1 minute in milliseconds
unsigned long previousMillisPublishIrms = 0;

void checkWiFiConnection()
{
  if (currentMillis - previousMillis >= wifiCheckInterval)
  {
    previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED)
    {
      if (Hflag)
      {
        deactivateHotspot();
        Hflag = false;
      }
    }
    else
    {
      if (!connectToWiFi())
      {
        if (!Hflag)
        {
          initHotspot();
          Hflag = true;
        }
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  initEEPROM();
  CheckEpromData();
  initTemperature();
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  initPresenceSensor();
  if (!Sflag && !connectToWiFi())
  {
    initHotspot();
    Hflag = true;
  }
  else
  {
    checkAndUpdateFirmware(versionUrl, currentVersion);
    if (!Dflag && !Mflag)
    {
      initMqtt();
      reconnect();
    }
  }
  initOTA();
  initializeRoutes();
}

void loop()
{
  currentMillis = millis();
  checkWiFiConnection();

  if (currentMillis - previousMillisPublishIrms >= 5000)
  {
    previousMillisPublishIrms = currentMillis;
    if (publishTemperatureData())
    {
      Serial.println("Temperature Data Published to Mqtt Successfully");
    }
    else
    {
      Serial.println("Temperature data doesn't send to Mqtt");
    }
    if (publishPresenceData())
    {
      Serial.println("Presence Data Published to Mqtt Successfully");
    }
    else
    {
      Serial.println("Presence  data doesn't send to Mqtt");
    }
  }
  if (shouldRestart)
  {
    delay(1000);
    ESP.restart();
  }
  mqttLoop();
  otaport();
  handleHttpClient();
  instantPresenceData();
}