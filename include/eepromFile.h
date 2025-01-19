#ifndef EEPROMFILE_H
#define EEPROMFILE_H

#include <EEPROM.h>

void initEEPROM();
void CheckEpromData();
String readStringFromEEPROM(int startAddr);
void saveToEEPROM(int startAddr, String data);
bool isValidString(String str);
String Subtopic();

#endif