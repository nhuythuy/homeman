#include <BlynkSimpleEsp32.h>
#include "wifi_cloud.h"
#include "global_vars.h"

int blynkCounter = 0;

#ifndef BLYNK
#define BLYNK

#define MESSAGE_DELAY                       2
#define MAX_BLYNK_SIGNAL                    17 // find the last index and update here

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

#define VP_PS_RUNTIME                       V41   // power station node
#define VP_PS_TEMPERATURE                   V42
#define VP_PS_HUMIDITY                      V43

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
#define VP_FORCE_CAMERA_POWER       V101

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

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(VP_FORCE_RADIO_POWER)
{
  int pinValue = param.asInt(); // assigning incoming value from pin VP_FORCE_RADIO_POWER to a variable
  // process received value
}

BLYNK_WRITE(VP_FORCE_CAMERA_POWER)
{
  int pinValue = param.asInt(); // assigning incoming value from pin VP_FORCE_CAMERA_POWER to a variable
  // process received value
}

BLYNK_READ(VP_DOOR_MAIN)
{
  // This command writes Arduino's uptime in seconds to VP_DOOR_MAIN
  Blynk.virtualWrite(VP_DOOR_MAIN, (ssDoorMain ? 255 : 0));
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void blynkTimerEvent()
{
  blynkReconnect();
  if(blynkCounter++ > MAX_BLYNK_SIGNAL)
    blynkCounter = 0;
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Serial.println("Sending to Blynk...");

  // for all signals to be sent at once
//  Blynk.virtualWrite(VP_DOOR_MAIN, (ssDoorMain ? 255 : 0));
//  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_TO_BASEMENT, (ssDoorToBasement ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BASEMENT, (ssDoorBasement ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BACK, (ssDoorBack ? 255 : 0));
  delay(MESSAGE_DELAY);


  // for all other signals: slow response
  if(blynkCounter == 0)
    Blynk.virtualWrite(VP_BATT_VOLTAGE, ssBatteryVolt);
  if(blynkCounter == 1)
    Blynk.virtualWrite(VP_BM_RUNTIME, bmRuntimeMinutes);
  if(blynkCounter == 2)
    Blynk.virtualWrite(VP_BM_TEMPERATURE, bmTemp);
  if(blynkCounter == 3)
    Blynk.virtualWrite(VP_BM_HUMIDITY, bmTempX); // bmHumidity); temp. borrow this for showing temp. using LM35 sensor
  if(blynkCounter == 4)
//  Blynk.virtualWrite(VP_ENTRANCE_MOTION_DETECTED_SECONDS, entranceMotionSeconds);

  if(blynkCounter == 5)
    Blynk.virtualWrite(VP_DOOR_MAIN_OPENED_MINUTES, doorMainOpenedMinutes);
  if(blynkCounter == 6)
    Blynk.virtualWrite(VP_DOOR_TO_BASEMENT_OPENED_MINUTES, doorToBasementOpenedMinutes);
  if(blynkCounter == 7)
    Blynk.virtualWrite(VP_DOOR_BASEMENT_OPENED_MINUTES, doorBasementOpenedMinutes);
  if(blynkCounter == 8)
    Blynk.virtualWrite(VP_DOOR_BACK_OPENED_MINUTES, doorBackOpenedMinutes);

  if(blynkCounter == 9)
    Blynk.virtualWrite(VP_LR_RUNTIME, lrRuntimeMinutes);
  if(blynkCounter == 10)
    Blynk.virtualWrite(VP_LR_TEMPERATURE, lrTemp);
  if(blynkCounter == 11)
    Blynk.virtualWrite(VP_LR_HUMIDITY, lrHumidity);

  if(blynkCounter == 12)
    Blynk.virtualWrite(VP_PS_RUNTIME, psRuntimeMinutes);
  if(blynkCounter == 13)
    Blynk.virtualWrite(VP_PS_TEMPERATURE, psTemp);
  if(blynkCounter == 14)
    Blynk.virtualWrite(VP_PS_HUMIDITY, psHumidity);

  if(blynkCounter == 15)
    Blynk.virtualWrite(VP_ENTRANCE_MOTION, (ssEntranceMotion ? 255 : 0));
  if(blynkCounter == 16)
    Blynk.virtualWrite(CH_LIGHT_BASEMENT, (ssLightBasementOn ? 255 : 0));
  if(blynkCounter == 17)
    Blynk.virtualWrite(CH_LIGHT_STAIR_BASEMENT, (ssDoorToBasement ? 255 : 0)); // same as this signal

  Serial.println("Sent to Blynk...");
}

void blynkSetup(){
  Serial.println("Connecting to Blynk ...");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, blynkTimerEvent);
  Serial.println("Connected to Blynk !");
  delay(200);
}

void blynkLoop(){
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

#endif
