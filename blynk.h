#include <BlynkSimpleEsp8266.h>
#include "wifi_pw.h"
#include "global_vars.h"

#define MESSAGE_DELAY                       200

#define VP_BATT_VOLTAGE                     V1
#define VP_BM_RUNTIME                       V2   // basement node
#define VP_BM_TEMPERATURE                   V3
#define VP_BM_HUMIDITY                      V4
#define VP_ENTRANCE_MOTION_DETECTED_SECONDS V5
#define VP_DOOR_MAIN_OPENED_MINUTES         V6
#define VP_DOOR_BASEMENT_OPENED_MINUTES     V7
#define VP_DOOR_BACK_OPENED_MINUTES         V8

#define VP_LR_RUNTIME                       V11   // living room node
#define VP_LR_TEMPERATURE                   V12
#define VP_LR_HUMIDITY                      V13

// digital states
#define VP_DOOR_MAIN                V51
#define VP_DOOR_BASEMENT            V52
#define VP_DOOR_BACK                V53
#define VP_ENTRANCE_MOTION          V54
#define VP_LIGHT_BASEMENT           V55
#define VP_LIGHT_STAIR_BASEMENT     V56
#define VP_ENTRANCE_LIGHT           V57
#define VP_POWER_RADIO              V58
#define VP_WATER_LEAK_0             V59
#define VP_WATER_LEAK_1             V60

#define VP_FORCE_RADIO_POWER        V100

#define BLYNK_PRINT Serial

#ifndef WIFI_INFO
#define WIFI_INFO
char ssid[] = "VNNO";
char pass[] = WIFI_PW;
#endif

char auth[] = "79pVr226PqcM-DKB7ldkaWVULxeAC5i4";

BlynkTimer timer;

void blynkReconnect() {
  if (!Blynk.connected()) {
    if (Blynk.connect()) {
      BLYNK_LOG("Reconnected");
    }
    else {
      BLYNK_LOG("Not reconnected");
    }
  }
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void blynkTimerEvent()
{
  blynkReconnect();

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Serial.println("Sending to Blynk...");
  Blynk.virtualWrite(VP_BATT_VOLTAGE, ssBatteryVolt);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_BM_RUNTIME, bmRuntimeMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_BM_TEMPERATURE, bmTemp);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_BM_HUMIDITY, bmHumidity);
  delay(MESSAGE_DELAY);
//  Blynk.virtualWrite(VP_ENTRANCE_MOTION_DETECTED_SECONDS, entranceMotionSeconds);

  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_MAIN_OPENED_MINUTES, bmTemp);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BASEMENT_OPENED_MINUTES, bmTemp);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK_OPENED_MINUTES, bmTemp);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LR_RUNTIME, lrRuntimeMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LR_TEMPERATURE, lrTemp);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LR_HUMIDITY, lrHumidity);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_MAIN, ssDoorMain);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BASEMENT, ssDoorBasement);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK, ssDoorBack);

  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK, ssEntranceMotion);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK, ssLightBasementOn);

  Serial.println("Sent to Blynk...");
}

void blynkSetup(){
  Serial.println("Connecting to Blynk ...");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, blynkTimerEvent);
  Serial.println("Connected to Blynk !");
}

void blynkLoop(){
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
