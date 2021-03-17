#include <BlynkSimpleEsp32.h>
#include "wifi_cloud.h"
#include "global_vars.h"

int blynkCounter = 0;

#ifndef BLYNK
#define BLYNK

#define MESSAGE_DELAY                       100

#define VP_PS_BATT_VOLTAGE                  V0
#define VP_BATT_VOLTAGE                     V1
#define VP_SR_RUNTIME                       V2   // basement node
#define VP_ENTRANCE_MOTION_DETECTED_SECONDS V5
#define VP_DOOR_MAIN_OPENED_MINUTES         V6
#define VP_DOOR_TO_BASEMENT_OPENED_MINUTES  V7
#define VP_DOOR_BASEMENT_OPENED_MINUTES     V8
//#define VP_DOOR_BACK_OPENED_MINUTES         V9

#define VP_SYSTEM_TIME                      V10

#define VP_BM_TEMPERATURE                   V20
#define VP_BM_HUMIDITY                      V21
#define VP_ST_TEMPERATURE                   V22
#define VP_SR_TEMPERATURE                   V23

// digital states
#define VP_DOOR_MAIN                V51
#define VP_DOOR_TO_BASEMENT         V52
#define VP_DOOR_BASEMENT            V53
//#define VP_DOOR_BACK                V54
#define VP_ENTRANCE_MOTION          V55
#define VP_LIGHT_BASEMENT           V56
#define VP_LIGHT_STAIR_BASEMENT     V57
#define VP_ENTRANCE_LIGHT           V58
#define VP_POWER_RADIO              V59
#define VP_WATER_SMOKE_BASEMENT     V60
#define VP_WATER_LEAK_1             V61
#define VP_SWITCH_TO_SOLAR_POWER    V62

#define VP_ENABLE_SPEAKER           V70

#define VP_FORCE_RADIO_POWER        V100
#define VP_FORCE_CAMERA_POWER       V101
#define VP_FORCE_BLUETOOTH_DEBUG    V102
#define VP_FORCE_UDP_DEBUG          V103

//#define BLYNK_PRINT Serial

#ifndef WIFI_INFO
#define WIFI_INFO
char ssid[] = WIFI_AP;
char pass[] = WIFI_PW;
#endif

char auth[] = BLYNK_PW;

BlynkTimer timer;

void blynkReconnect() {
  if (!Blynk.connected()) {
    yield();
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
  int pinVal = param.asInt(); // assigning incoming value from pin VP_FORCE_RADIO_POWER to a variable
  // process received value
}

BLYNK_WRITE(VP_FORCE_CAMERA_POWER)
{
  int pinVal = param.asInt();
}

BLYNK_WRITE(VP_ENABLE_SPEAKER)
{
  int pinVal = param.asInt();
  enableSpeaker = (boolean)pinVal;
  Serial.println("Enable speaker: " + String(enableSpeaker));
}

BLYNK_WRITE(VP_FORCE_BLUETOOTH_DEBUG)
{
  int pinVal = param.asInt();
  enableBluetoothDebug = (boolean)pinVal;
  Serial.println("Bluetooth debug: " + String(enableBluetoothDebug));
}

BLYNK_WRITE(VP_FORCE_UDP_DEBUG)
{
  int pinVal = param.asInt();
  enableUdpDebug = (boolean)pinVal;
  Serial.println("UDP debug: " + String(enableUdpDebug));
}

// for all signals requested by Blynk app (slow response)
BLYNK_READ(VP_BATT_VOLTAGE){
  Blynk.virtualWrite(VP_BATT_VOLTAGE, ssBatteryVolt);
}

BLYNK_READ(VP_SR_RUNTIME){
  // This command writes Arduino's uptime in minutes to VP_SR_RUNTIME
  Blynk.virtualWrite(VP_SR_RUNTIME, runtimeMinutes);
}

BLYNK_READ(VP_BM_TEMPERATURE){
  Blynk.virtualWrite(VP_BM_TEMPERATURE, bmTemp);
}

BLYNK_READ(VP_BM_HUMIDITY){
  Blynk.virtualWrite(VP_BM_HUMIDITY, bmHumidity);
}

BLYNK_READ(VP_ST_TEMPERATURE){
  Blynk.virtualWrite(VP_ST_TEMPERATURE, stTemp);
}

BLYNK_READ(VP_SR_TEMPERATURE){
  Blynk.virtualWrite(VP_SR_TEMPERATURE, srTemp);
}

BLYNK_READ(VP_DOOR_MAIN_OPENED_MINUTES){
  Blynk.virtualWrite(VP_DOOR_MAIN_OPENED_MINUTES, doorMainOpenedMinutes);
}

BLYNK_READ(VP_DOOR_TO_BASEMENT_OPENED_MINUTES){
  Blynk.virtualWrite(VP_DOOR_TO_BASEMENT_OPENED_MINUTES, doorToBasementOpenedMinutes);
}

BLYNK_READ(VP_DOOR_BASEMENT_OPENED_MINUTES){
  Blynk.virtualWrite(VP_DOOR_BASEMENT_OPENED_MINUTES, doorBasementOpenedMinutes);
}

//BLYNK_READ(VP_DOOR_BACK_OPENED_MINUTES){
//  Blynk.virtualWrite(VP_DOOR_BACK_OPENED_MINUTES, doorBackOpenedMinutes);
//}

BLYNK_READ(VP_ENTRANCE_MOTION_DETECTED_SECONDS){
  Blynk.virtualWrite(VP_ENTRANCE_MOTION_DETECTED_SECONDS, entranceMotionSeconds);
}

BLYNK_READ(VP_SYSTEM_TIME){
  Blynk.virtualWrite(VP_SYSTEM_TIME, systemHourMinute);
}


// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void blynkTimerEvent()
{
  blynkReconnect();
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Serial.println("Blynk timer triggered...");
  // for all signals to be sent at once

  Blynk.virtualWrite(VP_DOOR_MAIN, (ssDoorMain ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_TO_BASEMENT, (ssDoorToBasement ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_DOOR_BASEMENT, (ssDoorBasement ? 255 : 0));
  delay(MESSAGE_DELAY);
//  Blynk.virtualWrite(VP_DOOR_BACK, (ssDoorBack ? 255 : 0));
//  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_ENTRANCE_MOTION, (ssEntranceMotion ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LIGHT_BASEMENT, (ssLightBasementOn ? 255 : 0));
  delay(MESSAGE_DELAY);
  Blynk.virtualWrite(VP_LIGHT_STAIR_BASEMENT, (ssDoorToBasement ? 255 : 0)); // same as this signal
  delay(MESSAGE_DELAY);
}

void blynkSetup(){
  Serial.println("Connecting to Blynk ...");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, blynkTimerEvent);
  Serial.println("Connected to Blynk !");
  delay(200);
}

void blynkLoop(){
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

#endif
