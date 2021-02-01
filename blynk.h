#include <BlynkSimpleEsp32.h>
#include "wifi_cloud.h"
#include "global_vars.h"

#ifndef BLYNK
#define BLYNK

#define MESSAGE_DELAY                       200

#define VP_BATT_VOLTAGE                     V1
#define VP_BM_RUNTIME                       V2   // basement node
#define VP_BM_TEMPERATURE                   V3
#define VP_BM_HUMIDITY                      V4
#define VP_ENTRANCE_MOTION_DETECTED_SECONDS V5
#define VP_DOOR_MAIN_OPENED_MINUTES         V6
#define VP_DOOR_TO_BASEMENT_OPENED_MINUTES  V7
#define VP_DOOR_BASEMENT_OPENED_MINUTES     V8
#define VP_DOOR_BACK_OPENED_MINUTES         V9

#define VP_LR_RUNTIME                       V21   // living room node
#define VP_LR_TEMPERATURE                   V22
#define VP_LR_HUMIDITY                      V23

// digital states
#define VP_DOOR_MAIN                V51
#define VP_DOOR_TO_BASEMENT         V52
#define VP_DOOR_BASEMENT            V53
#define VP_DOOR_BACK                V54
#define VP_ENTRANCE_MOTION          V55
#define VP_LIGHT_BASEMENT           V56
#define VP_LIGHT_STAIR_BASEMENT     V57
#define VP_ENTRANCE_LIGHT           V58
#define VP_POWER_RADIO              V59
#define CH_WATER_SMOKE_BASEMENT     V60
#define VP_WATER_LEAK_1             V61

#define VP_FORCE_RADIO_POWER        V100

#define BLYNK_PRINT Serial

#ifndef WIFI_INFO
#define WIFI_INFO
char ssid[] = WIFI_AP;
char pass[] = WIFI_PW;
#endif

char auth[] = BLYNK_PW;

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
  Blynk.virtualWrite(VP_DOOR_MAIN_OPENED_MINUTES, doorMainOpenedMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_TO_BASEMENT_OPENED_MINUTES, doorToBasementOpenedMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BASEMENT_OPENED_MINUTES, doorBasementOpenedMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK_OPENED_MINUTES, doorBackOpenedMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LR_RUNTIME, lrRuntimeMinutes);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LR_TEMPERATURE, lrTemp);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LR_HUMIDITY, lrHumidity);
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_MAIN, (ssDoorMain ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_TO_BASEMENT, (ssDoorToBasement ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BASEMENT, (ssDoorBasement ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK, (ssDoorBack ? 255 : 0));

  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_ENTRANCE_MOTION, (ssEntranceMotion ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(CH_LIGHT_BASEMENT, (ssLightBasementOn ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(CH_LIGHT_STAIR_BASEMENT, (ssDoorToBasement ? 255 : 0)); // same as this signal

  Serial.println("Sent to Blynk...");
}

void blynkSetup(){
  Serial.println("Connecting to Blynk ...");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, blynkTimerEvent);
  Serial.println("Connected to Blynk !");
  delay(500);
}

void blynkLoop(){
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

#endif
