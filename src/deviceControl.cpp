#include "eepromFile.h"
#include <Arduino.h>
#include "variable.h"
#include "deviceControl.h"
#include <Adafruit_AHTX0.h>
#include <ld2410.h>

Adafruit_AHTX0 aht; // Initialize the AHT sensor object
ld2410 radar;
uint32_t lastReading1 = 0;
uint32_t mov_tgt;
uint32_t stat_target;
uint32_t no_target;
int flag = 0;
void initTemperature()
{
  Serial.begin(115200);
  delay(500);

  // AHT setup
  Serial.println("Initializing AHT10/AHT20 sensor...");
  if (!aht.begin())
  {
    Serial.println("Could not find AHT? Check wiring");
    while (1)
      delay(10); // Stay in an infinite loop if sensor is not found
  }
  Serial.println("AHT10 or AHT20 found");
}

float instantTemperature()
{
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  // Print sensor data
  // Serial.print("Temperature: ");
  // Serial.print(temp.temperature);
  // Serial.println(" °C");

  // Serial.print("Humidity: ");
  // Serial.print(humidity.relative_humidity);
  // Serial.println(" %");

  return temp.temperature; // Return the temperature value
}

void initPresenceSensor()
{
  MONITOR_SERIAL.begin(115200); // Feedback over Serial Monitor
// radar.debug(MONITOR_SERIAL); // Uncomment to show debug information from the library on the Serial Monitor.
#if defined(ESP32)
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN); // UART for monitoring the radar
#elif defined(__AVR_ATmega32U4__)
  RADAR_SERIAL.begin(256000); // UART for monitoring the radar
#endif
  delay(500);
  MONITOR_SERIAL.print(F("\nConnect LD2410 radar TX to GPIO:"));
  MONITOR_SERIAL.println(RADAR_RX_PIN);
  MONITOR_SERIAL.print(F("Connect LD2410 radar RX to GPIO:"));
  MONITOR_SERIAL.println(RADAR_TX_PIN);
  MONITOR_SERIAL.print(F("LD2410 radar sensor initializing: "));

  if (radar.begin(RADAR_SERIAL))
  {
    MONITOR_SERIAL.println(F("OK"));
    MONITOR_SERIAL.print(F("LD2410 firmware version: "));
    MONITOR_SERIAL.print(radar.firmware_major_version);
    MONITOR_SERIAL.print('.');
    MONITOR_SERIAL.print(radar.firmware_minor_version);
    MONITOR_SERIAL.print('.');
    MONITOR_SERIAL.println(radar.firmware_bugfix_version, HEX);
  }
  else
  {
    MONITOR_SERIAL.println(F("not connected"));
  }
}

void instantPresenceData()
{
  // int flag=0;
  radar.read();
  if (radar.isConnected() && millis() - lastReading1 > 1000) // Report every 1000ms
  {
    lastReading1 = millis();
    if (radar.presenceDetected())
    {
      if (radar.stationaryTargetDetected())
      {
        Serial.print(F("Stationary target: "));
        Serial.print(radar.stationaryTargetDistance());
        Serial.print(F("cm energy:"));
        Serial.print(radar.stationaryTargetEnergy());
        Serial.print(' ');
        stat_target = radar.stationaryTargetDistance();
      }
      if (radar.movingTargetDetected())
      {
        Serial.print(F("Moving target: "));
        Serial.print(radar.movingTargetDistance());
        Serial.print(F("cm energy:"));
        Serial.print(radar.movingTargetEnergy());
        mov_tgt = radar.movingTargetDistance();
      }
      Serial.println();
      // if(mov_tgt==0)
      // {
      //   flag++;
      //   if(flag==20)
      //   {
      //     Serial.println("no target found turning off relay--------------------------------------------------->");
      //     flag=0;
      //   }
      // }
      // else
      // {
      //   flag=0;
      // }
    }
    else
    {
      no_target = Serial.println(F("No target"));
      flag++;
      if (flag == 20)
      {
        Serial.println("no target found turning off relay--------------------------------------------------->");
        flag = 0;
      }
    }
  }
}
