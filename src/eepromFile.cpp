#include "eepromFile.h"
#include <Arduino.h>
#include "variable.h"

#include <EEPROM.h>
#include <WiFi.h>

bool Dflag = false;   // device id
bool Mflag = false;   // mf code
bool Sflag = false;   // ssid   


void initEEPROM() {
    if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialize EEPROM");
    return;
  }
}

void CheckEpromData() {
  String ssid = readStringFromEEPROM(SSID_ADDR);
  String  pwd = readStringFromEEPROM(PASSWORD_ADDR);
  String deviceId = readStringFromEEPROM(DEVICEID_ADDR);
  String mfid = readStringFromEEPROM(MFCODE_ADDR);
  if (!isValidString(ssid)) {
    saveToEEPROM(SSID_ADDR,"EPVI");
    Sflag = true;
    
  }
  if (!isValidString(pwd)) {
    saveToEEPROM(PASSWORD_ADDR,"EPVI12345");
  }
  if (!isValidString(deviceId)) {
    Dflag = true;
    Serial.println("deviceidFlag "+String(Dflag));
  }
  if (!isValidString(mfid)) {
    Mflag = true; 
    Serial.println("MFidFlag "+String(Mflag));
  }
}

String readStringFromEEPROM(int startAddr) {
  char data[MAX_LENGTH + 1];  // +1 for null terminator
  int len = 0;
  
  for (int i = 0; i < MAX_LENGTH; i++) {
    data[i] = EEPROM.read(startAddr + i);
    if (data[i] == '\0') break;
    len++;
  }
  data[len] = '\0';  // Null-terminate the string
  return (data);
}

bool isValidString(String str) {
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    // Check for valid printable ASCII characters (32-126) and common control characters like newline
    if (!(c >= 32 && c <= 126)) {
      return false;  // Return false if any invalid character is found
    }
  }
  return true;  // All characters are valid
}

void saveToEEPROM(int startAddr, String data) {
  int len = data.length();
  if (len > MAX_LENGTH) len = MAX_LENGTH;
  
  for (int i = 0; i < len; i++) {
    EEPROM.write(startAddr + i, data[i]);
  }
  EEPROM.write(startAddr + len, '\0');  // Null-terminate the string
  EEPROM.commit();
}

String Subtopic() {
  String subTopic;
  if (Dflag) {
    subTopic = "device/" + readStringFromEEPROM(MFCODE_ADDR);
  }
  else if (Mflag){
    subTopic = "device/" + readStringFromEEPROM(DEVICEID_ADDR);
  }
  else {
    subTopic = "device/" + readStringFromEEPROM(DEVICEID_ADDR);
  }
  return subTopic;
}