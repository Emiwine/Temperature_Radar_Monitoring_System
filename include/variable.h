#ifndef VARIABLE_H
#define VARIABLE_H

#include <Arduino.h>

#include <ArduinoJson.h>

#define DEBUG_ENABLED

#define EEPROM_SIZE 512
// Starting addresses for each variable
#define SSID_ADDR      100
#define PASSWORD_ADDR  150
#define GROUPID_ADDR   200
#define DEVICEID_ADDR  250
#define MFCODE_ADDR    300


#define led_Pin 2
 

extern bool shouldRestart;
extern bool Dflag;
extern bool Mflag;
extern bool Sflag;

extern unsigned long lastPublishTime;

extern const char *currentVersion;

#define MAX_LENGTH 50

#endif