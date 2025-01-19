#ifndef DEVICECONTROL_H
#define  DEVICECONTROL_H
  #define MONITOR_SERIAL Serial
 #define RADAR_SERIAL Serial1
      #define RADAR_RX_PIN 16
      #define RADAR_TX_PIN 17
// #define RADAR_SERIAL Serial1

#include <ld2410.h>
#include <Adafruit_AHTX0.h> 
extern Adafruit_AHTX0 aht;
extern ld2410 radar;


extern uint32_t mov_tgt;
extern uint32_t  stat_target;
extern uint32_t no_target;

// bool radarConnected = false;


void initTemperature();
float instantTemperature ();
void initPresenceSensor();
void instantPresenceData();


#endif