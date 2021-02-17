#include "wifi_cloud.h"
#include <CayenneMQTTESP32.h>


// Name convention: Area_SensorType_Time_Type, ex.: DOOR_MAIN_MINUTES_OPENED, LR_HUMIDITY

#ifndef MY_DEVICES
#define MY_DEVICES

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char dvUsername[] = CAYENNE_USERNAME;
char dvPassword[] = CAYENNE_PASSWORD;
char dvClientID[] = CAYENNE_CLIENT_ID;

#define MESSAGE_DELAY                       100 // millisec
#define MAX_CAYENNE_SIGNAL                  21 // find the last index and update here

#define CH_BATT_VOLTAGE                     1
#define CH_BM_RUNTIME                       2 // basement node
#define CH_BM_TEMPERATURE                   3
#define CH_BM_HUMIDITY                      4
#define CH_ENTRANCE_MOTION_DETECTED_SECONDS 5
#define CH_DOOR_MAIN_OPENED_MINUTES         6
#define CH_DOOR_TO_BASEMENT_OPENED_MINUTES  7
#define CH_DOOR_BASEMENT_OPENED_MINUTES     8
#define CH_DOOR_BACK_OPENED_MINUTES         9

#define CH_LR_RUNTIME                       21   // living room node
#define CH_LR_TEMPERATURE                   22
#define CH_LR_HUMIDITY                      23

#define CH_PS_RUNTIME                       41   // power station
#define CH_PS_TEMPERATURE                   42
#define CH_PS_HUMIDITY                      43

// digital states
#define CH_DOOR_MAIN                51
#define CH_DOOR_TO_BASEMENT         52
#define CH_DOOR_BASEMENT            53
#define CH_DOOR_BACK                54
#define CH_ENTRANCE_MOTION          55
#define CH_LIGHT_BASEMENT           56
#define CH_LIGHT_STAIR_BASEMENT     57
#define CH_ENTRANCE_LIGHT           58
#define CH_POWER_RADIO              59
#define CH_WATER_SMOKE_BASEMENT     60
#define CH_WATER_LEAK_1             61

#define CH_FORCE_RADIO_POWER        100


void cayenneSetup(){
  Serial.println("Cayenne connecting...");
  Cayenne.begin(dvUsername, dvPassword, dvClientID, wifiSsid, wifiPassword);
  Serial.println("Cayenne connected!");
  delay(200);
}

CAYENNE_OUT_DEFAULT(){
  //blynkReconnect();

    Cayenne.virtualWrite(CH_BATT_VOLTAGE, ssBatteryVolt, "batt", "V");
    Cayenne.virtualWrite(CH_DOOR_MAIN_OPENED_MINUTES, doorMainOpenedMinutes, "counter");
//    Cayenne.virtualWrite(CH_DOOR_BACK_OPENED_MINUTES, doorBackOpenedMinutes, "counter");
    Cayenne.virtualWrite(CH_DOOR_TO_BASEMENT_OPENED_MINUTES, doorToBasementOpenedMinutes, "counter");
    Cayenne.virtualWrite(CH_DOOR_BASEMENT_OPENED_MINUTES, doorBasementOpenedMinutes, "counter");
    Cayenne.virtualWrite(CH_BM_RUNTIME, bmRuntimeMinutes, "counter");
//    Cayenne.virtualWrite(CH_LR_RUNTIME, lrRuntimeMinutes, "counter");
//    Cayenne.virtualWrite(CH_PS_RUNTIME, psRuntimeMinutes, "counter");
    Cayenne.virtualWrite(CH_ENTRANCE_MOTION_DETECTED_SECONDS, entranceMotionSeconds, "counter");
    Cayenne.celsiusWrite(CH_BM_TEMPERATURE, bmTemp);
    Cayenne.virtualWrite(CH_BM_HUMIDITY, bmHumidity, "rel_hum", "p");
//    Cayenne.celsiusWrite(CH_LR_TEMPERATURE, lrTemp);
//    Cayenne.virtualWrite(CH_LR_HUMIDITY, lrHumidity, "rel_hum", "p");
//    Cayenne.celsiusWrite(CH_PS_TEMPERATURE, psTemp);
//    Cayenne.virtualWrite(CH_PS_HUMIDITY, psHumidity, "rel_hum", "p");

  // digital signals
  Cayenne.virtualWrite(CH_DOOR_MAIN, ssDoorMain, "digital_sensor", "d");
  Cayenne.virtualWrite(CH_DOOR_TO_BASEMENT, ssDoorToBasement, "digital_sensor", "d");
  Cayenne.virtualWrite(CH_DOOR_BASEMENT, ssDoorBasement, "digital_sensor", "d");
//  Cayenne.virtualWrite(CH_DOOR_BACK, ssDoorBack, "digital_sensor", "d");
  Cayenne.virtualWrite(CH_ENTRANCE_MOTION, ssEntranceMotion, "digital_sensor", "d");
  Cayenne.virtualWrite(CH_LIGHT_BASEMENT, ssLightBasementOn, "digital_sensor", "d");
  Cayenne.virtualWrite(CH_LIGHT_STAIR_BASEMENT, ssDoorToBasement, "digital_sensor", "d");

}

void writeCayenneDigitalStates(int channelId, int value){
  delay(MESSAGE_DELAY);
  Cayenne.virtualWrite(channelId, value, "digital_sensor", "d");
}

CAYENNE_IN(CH_FORCE_RADIO_POWER)
{
  forceRadioPower = getValue.asInt(); // Get value as integer
//  Serial.println("Radio power force: " + String(forceRadioPower));
}


#endif
